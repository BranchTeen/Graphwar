#include "AudioManager.h"
#include "common/AudioState.h"
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QThread>
#include <QTimer>
#include <QMap>
#include <atomic>
#include <cmath>
#include <random>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
}

namespace {
    void logLine(const QString &msg) {
        QFile f(QCoreApplication::applicationDirPath() + "/graphwar_audio.log");
        if (f.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream s(&f);
            s << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
              << "  " << msg << "\n";
        }
    }

    QString extractResourceToTemp(const QString &resourcePath) {
        QFile src(resourcePath);
        if (!src.exists()) {
            logLine(QString("[AudioManager] Resource not found: %1").arg(resourcePath));
            return QString();
        }
        QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        if (tempDir.isEmpty()) {
            tempDir = QDir::tempPath();
        }
        QFileInfo info(resourcePath);
        QString outPath = tempDir + "/graphwar_" + info.fileName();
        if (QFileInfo(outPath).exists() && QFileInfo(outPath).size() == src.size()) {
            return outPath;
        }
        if (QFile::exists(outPath)) {
            QFile::remove(outPath);
        }
        if (!src.copy(outPath)) {
            logLine(QString("[AudioManager] Failed copy to temp: %1").arg(outPath));
            return QString();
        }
        logLine(QString("[AudioManager] Extracted resource to temp: %1  size=%2 bytes")
                    .arg(outPath).arg(QFileInfo(outPath).size()));
        return outPath;
    }

    QString resolveUrlToLocalPath(const QUrl &source) {
        if (!source.isValid()) {
            logLine("[AudioManager] Invalid URL");
            return QString();
        }
        QString urlStr = source.toString();
        logLine(QString("[AudioManager] URL string: '%1'").arg(urlStr));
        logLine(QString("[AudioManager] URL scheme: '%1'").arg(source.scheme()));
        logLine(QString("[AudioManager] URL path: '%1'").arg(source.path()));
        
        if (urlStr.startsWith("qrc:/")) {
            QString resourcePath = ":" + urlStr.mid(4);
            logLine(QString("[AudioManager] Extracting qrc resource: %1").arg(resourcePath));
            return extractResourceToTemp(resourcePath);
        }
        if (urlStr.startsWith(":/")) {
            logLine(QString("[AudioManager] Extracting resource: %1").arg(urlStr));
            return extractResourceToTemp(urlStr);
        }
        if (source.isLocalFile()) {
            QString localPath = source.toLocalFile();
            logLine(QString("[AudioManager] Local file: %1").arg(localPath));
            return localPath;
        }
        logLine(QString("[AudioManager] Unknown URL type, returning as-is: %1").arg(urlStr));
        return urlStr;
    }
}

class SfxPlayer : public QObject {
    Q_OBJECT
public:
    explicit SfxPlayer(const QByteArray &pcm, const QAudioFormat &fmt, float volume, QObject *parent = nullptr)
        : QObject(parent), m_pcm(pcm), m_format(fmt) {
        m_sink = new QAudioSink(QMediaDevices::defaultAudioOutput(), fmt, this);
        m_sink->setVolume(volume);
        connect(m_sink, &QAudioSink::stateChanged, this, &SfxPlayer::onStateChanged);
        QTimer::singleShot(10000, this, [this]() { deleteLater(); });
        m_io = m_sink->start();
        if (m_io) {
            m_io->write(m_pcm);
        } else {
            logLine("[AudioManager] SfxPlayer: QAudioSink::start() returned null");
            deleteLater();
        }
    }

    ~SfxPlayer() override {
        if (m_sink) {
            m_sink->stop();
        }
    }

    void updateVolume(float v) {
        if (m_sink) m_sink->setVolume(v);
    }

private slots:
    void onStateChanged(QAudio::State state) {
        if (state == QAudio::IdleState || state == QAudio::StoppedState) {
            deleteLater();
        }
    }

private:
    QByteArray m_pcm;
    QAudioFormat m_format;
    QAudioSink *m_sink = nullptr;
    QIODevice *m_io = nullptr;
};

class AudioDecoderThread : public QThread {
    Q_OBJECT
public:
    explicit AudioDecoderThread(const QString &filePath, AudioManager *manager, QObject *parent = nullptr)
        : QThread(parent), m_filePath(filePath), m_manager(manager), m_stop(false), m_loop(true) {}

    ~AudioDecoderThread() override {
        requestStop();
        wait(3000);
    }

    void requestStop() { m_stop.store(true); }
    void setLoop(bool on) { m_loop.store(on); }

protected:
    void run() override {
        logLine(QString("[AudioManager] Decoder thread start: %1").arg(m_filePath));

        AVFormatContext *fmtCtx = nullptr;
        if (avformat_open_input(&fmtCtx, m_filePath.toUtf8().constData(), nullptr, nullptr) != 0) {
            logLine("[AudioManager] avformat_open_input FAILED — file missing or format unsupported");
            return;
        }
        if (avformat_find_stream_info(fmtCtx, nullptr) < 0) {
            logLine("[AudioManager] avformat_find_stream_info FAILED");
            avformat_close_input(&fmtCtx);
            return;
        }
        int audioStreamIdx = -1;
        for (unsigned i = 0; i < fmtCtx->nb_streams; ++i) {
            if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audioStreamIdx = static_cast<int>(i);
                break;
            }
        }
        if (audioStreamIdx < 0) {
            logLine("[AudioManager] No audio stream in file");
            avformat_close_input(&fmtCtx);
            return;
        }
        AVStream *audioStream = fmtCtx->streams[audioStreamIdx];

        const AVCodec *codec = avcodec_find_decoder(audioStream->codecpar->codec_id);
        if (!codec) {
            logLine(QString("[AudioManager] No decoder for codec_id=%1").arg(audioStream->codecpar->codec_id));
            avformat_close_input(&fmtCtx);
            return;
        }
        AVCodecContext *codecCtx = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codecCtx, audioStream->codecpar);
        if (avcodec_open2(codecCtx, codec, nullptr) < 0) {
            logLine("[AudioManager] avcodec_open2 FAILED");
            avcodec_free_context(&codecCtx);
            avformat_close_input(&fmtCtx);
            return;
        }
        logLine(QString("[AudioManager] Decoder: %1  sample_rate=%2 ch=%3 fmt=%4")
                    .arg(codec->name).arg(codecCtx->sample_rate)
                    .arg(codecCtx->ch_layout.nb_channels)
                    .arg(av_get_sample_fmt_name(codecCtx->sample_fmt)));

        const int outSampleRate = (codecCtx->sample_rate > 0) ? codecCtx->sample_rate : 44100;
        const int outChannels = 2;
        const AVSampleFormat outFmt = AV_SAMPLE_FMT_S16;

        SwrContext *swr = swr_alloc();
        av_opt_set_chlayout(swr, "in_chlayout", &codecCtx->ch_layout, 0);
        av_opt_set_int(swr, "in_sample_rate", codecCtx->sample_rate, 0);
        av_opt_set_sample_fmt(swr, "in_sample_fmt", codecCtx->sample_fmt, 0);
        AVChannelLayout outLayout = AV_CHANNEL_LAYOUT_STEREO;
        av_opt_set_chlayout(swr, "out_chlayout", &outLayout, 0);
        av_opt_set_int(swr, "out_sample_rate", outSampleRate, 0);
        av_opt_set_sample_fmt(swr, "out_sample_fmt", outFmt, 0);
        swr_init(swr);

        logLine(QString("[AudioManager] Output format: sampleRate=%1 channels=%2 fmt=S16")
                    .arg(outSampleRate).arg(outChannels));

        if (!m_manager->ensureBgmSinkCreated(outSampleRate, outChannels)) {
            logLine("[AudioManager] Failed to create QAudioSink");
            swr_free(&swr);
            avcodec_free_context(&codecCtx);
            avformat_close_input(&fmtCtx);
            return;
        }
        logLine("[AudioManager] QAudioSink created and started — audio should be playing");

        AVPacket *pkt = av_packet_alloc();
        AVFrame *frame = av_frame_alloc();
        qint64 decodedBytes = 0;
        int loopsDone = 0;

        while (!m_stop.load()) {
            int readErr = av_read_frame(fmtCtx, pkt);
            if (readErr < 0) {
                if (m_loop.load()) {
                    avformat_seek_file(fmtCtx, audioStreamIdx, 0, 0,
                                       fmtCtx->streams[audioStreamIdx]->duration, 0);
                    avcodec_flush_buffers(codecCtx);
                    loopsDone++;
                    logLine(QString("[AudioManager] Loop #%1 — total decoded %2 bytes")
                                .arg(loopsDone).arg(decodedBytes));
                    continue;
                } else {
                    break;
                }
            }
            if (pkt->stream_index != audioStreamIdx) {
                av_packet_unref(pkt);
                continue;
            }
            int sendErr = avcodec_send_packet(codecCtx, pkt);
            av_packet_unref(pkt);
            if (sendErr < 0) continue;

            while (!m_stop.load()) {
                int recvErr = avcodec_receive_frame(codecCtx, frame);
                if (recvErr < 0) break;

                uint8_t *outBuffer = nullptr;
                int outSamples = swr_get_out_samples(swr, frame->nb_samples);
                av_samples_alloc(&outBuffer, nullptr, outChannels, outSamples, outFmt, 0);
                int converted = swr_convert(swr, &outBuffer, outSamples,
                                          (const uint8_t **)frame->data, frame->nb_samples);
                int byteCount = converted * outChannels * av_get_bytes_per_sample(outFmt);

                if (byteCount > 0) {
                    m_manager->writeBgmPcm(reinterpret_cast<const char *>(outBuffer), byteCount);
                    decodedBytes += byteCount;
                }
                av_freep(&outBuffer);
            }
        }

        av_frame_free(&frame);
        av_packet_free(&pkt);
        swr_free(&swr);
        avcodec_free_context(&codecCtx);
        avformat_close_input(&fmtCtx);
        logLine(QString("[AudioManager] Decoder thread exit cleanly. total bytes=%1").arg(decodedBytes));
    }

private:
    QString m_filePath;
    AudioManager *m_manager;
    std::atomic<bool> m_stop;
    std::atomic<bool> m_loop;
};

AudioManager::AudioManager(QObject *parent)
    : QObject(parent),
      m_bgmVolume(60),
      m_bgmMuted(false),
      m_decoder(nullptr),
      m_bgmSink(nullptr),
      m_bgmSinkIo(nullptr),
      m_sfxVolume(80),
      m_sfxMuted(false) {
    const auto devices = QMediaDevices::audioOutputs();
    logLine(QString("[AudioManager] Available audio output devices: %1").arg(devices.size()));
    for (const auto &d : devices) {
        logLine(QString("  - %1").arg(d.description()));
    }
}

AudioManager::~AudioManager() {
    stopBackgroundMusic();
    stopAllSfx();
}

AudioManager &AudioManager::instance() {
    static AudioManager instance;
    return instance;
}

void AudioManager::playBackgroundMusic(const QUrl &source) {
    logLine(QString("[AudioManager] playBackgroundMusic called with: %1").arg(source.toString()));
    QString localPath = resolveUrlToLocalPath(source);
    logLine(QString("[AudioManager] Resolved path: %1").arg(localPath));
    if (localPath.isEmpty()) {
        logLine("[AudioManager] Empty path, returning");
        return;
    }

    stopBackgroundMusic();
    m_currentFilePath = localPath;
    m_decoder = new AudioDecoderThread(localPath, this, this);
    m_decoder->start();
    logLine(QString("[AudioManager] Starting decoder thread for: %1").arg(localPath));
}

void AudioManager::stopBackgroundMusic() {
    if (m_decoder) {
        m_decoder->requestStop();
        if (!m_decoder->wait(1500)) {
            logLine("[AudioManager] Decoder thread did not stop in time, terminating.");
            m_decoder->terminate();
            m_decoder->wait(500);
        }
        delete m_decoder;
        m_decoder = nullptr;
    }
    QMutexLocker lock(&m_bgmSinkMutex);
    if (m_bgmSink) {
        m_bgmSink->stop();
        delete m_bgmSink;
        m_bgmSink = nullptr;
    }
    if (m_bgmSinkIo) m_bgmSinkIo = nullptr;
    m_currentFilePath.clear();
}

bool AudioManager::ensureBgmSinkCreated(int sampleRate, int channels) {
    QMutexLocker lock(&m_bgmSinkMutex);

    if (m_bgmSink) {
        m_bgmSink->stop();
        delete m_bgmSink;
        m_bgmSink = nullptr;
        m_bgmSinkIo = nullptr;
    }

    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate);
    fmt.setChannelCount(channels);
    fmt.setSampleFormat(QAudioFormat::Int16);

    QAudioDevice dev = QMediaDevices::defaultAudioOutput();
    m_bgmSink = new QAudioSink(dev, fmt, this);
    m_bgmSink->setVolume(m_bgmMuted ? 0.0f : (m_bgmVolume / 100.0f));
    m_bgmSinkIo = m_bgmSink->start();
    if (!m_bgmSinkIo) {
        logLine("[AudioManager] QAudioSink::start() returned null (BGM)");
        return false;
    }
    logLine(QString("[AudioManager] QAudioSink (BGM) created: rate=%1 ch=%2 state=%3")
                .arg(sampleRate).arg(channels).arg(m_bgmSink->state()));
    return true;
}

void AudioManager::writeBgmPcm(const char *data, int bytes) {
    if (!data || bytes <= 0) return;
    QMutexLocker lock(&m_bgmSinkMutex);
    if (!m_bgmSinkIo || !m_bgmSink) return;

    while (m_bgmSink->bytesFree() < bytes && m_bgmSink->state() == QAudio::ActiveState) {
        lock.unlock();
        QThread::msleep(5);
        lock.relock();
        if (!m_bgmSinkIo) return;
    }
    m_bgmSinkIo->write(data, bytes);
}

int AudioManager::bgmVolume() const { return m_bgmVolume; }

void AudioManager::setBgmVolume(int v) {
    int clamped = qBound(0, v, 100);
    if (clamped == m_bgmVolume) return;
    m_bgmVolume = clamped;
    QMutexLocker lock(&m_bgmSinkMutex);
    if (m_bgmSink && !m_bgmMuted) m_bgmSink->setVolume(m_bgmVolume / 100.0f);
    emit bgmVolumeChanged(m_bgmVolume);
}

bool AudioManager::bgmMuted() const { return m_bgmMuted; }

void AudioManager::setBgmMuted(bool m) {
    if (m == m_bgmMuted) return;
    m_bgmMuted = m;
    QMutexLocker lock(&m_bgmSinkMutex);
    if (m_bgmSink) {
        m_bgmSink->setVolume(m_bgmMuted ? 0.0f : (m_bgmVolume / 100.0f));
    }
    emit bgmMutedChanged(m_bgmMuted);
}

void AudioManager::toggleBgmMuted() { setBgmMuted(!m_bgmMuted); }

void AudioManager::playSfx(SfxType type) {
    if (m_sfxMuted || m_sfxVolume <= 0) return;

    int key = static_cast<int>(type);
    WavData wav;
    {
        QMutexLocker lock(&m_sfxCacheMutex);
        auto it = m_sfxCache.find(key);
        if (it != m_sfxCache.end()) {
            wav = it.value();
        } else {
            wav = synthesizeSfx(type);
            m_sfxCache.insert(key, wav);
            if (wav.valid()) {
                logLine(QString("[AudioManager] Synthesized SFX #%1  sr=%2 ch=%3  bytes=%4")
                            .arg(key).arg(wav.sampleRate).arg(wav.channels).arg(wav.pcm.size()));
            }
        }
    }
    if (!wav.valid()) return;

    QAudioFormat fmt;
    fmt.setSampleRate(wav.sampleRate);
    fmt.setChannelCount(wav.channels);
    fmt.setSampleFormat(wav.sampleFormat);

    auto *player = new SfxPlayer(wav.pcm, fmt, m_sfxVolume / 100.0f, this);
    m_activeSfx.append(player);
    connect(player, &QObject::destroyed, this, [this, player]() {
        m_activeSfx.removeOne(player);
    });
}

void AudioManager::stopAllSfx() {
    auto list = m_activeSfx;
    for (auto *p : list) {
        p->deleteLater();
    }
    m_activeSfx.clear();
}

int AudioManager::sfxVolume() const { return m_sfxVolume; }

void AudioManager::setSfxVolume(int v) {
    int clamped = qBound(0, v, 100);
    if (clamped == m_sfxVolume) return;
    m_sfxVolume = clamped;
    float vf = m_sfxMuted ? 0.0f : (m_sfxVolume / 100.0f);
    for (auto *p : qAsConst(m_activeSfx)) {
        p->updateVolume(vf);
    }
    emit sfxVolumeChanged(m_sfxVolume);
}

bool AudioManager::sfxMuted() const { return m_sfxMuted; }

void AudioManager::setSfxMuted(bool m) {
    if (m == m_sfxMuted) return;
    m_sfxMuted = m;
    float vf = m_sfxMuted ? 0.0f : (m_sfxVolume / 100.0f);
    for (auto *p : qAsConst(m_activeSfx)) {
        p->updateVolume(vf);
    }
    emit sfxMutedChanged(m_sfxMuted);
}

void AudioManager::toggleSfxMuted() { setSfxMuted(!m_sfxMuted); }

static void writeInt16Mono(QByteArray &out, float sample) {
    qint16 v = static_cast<qint16>(qBound(-1.0f, sample, 1.0f) * 32767.0f);
    out.append(static_cast<char>(v & 0xff));
    out.append(static_cast<char>((v >> 8) & 0xff));
}

static constexpr int SFX_RATE = 44100;

static QByteArray toneSweep(float freqStart, float freqEnd, float durationSec,
                            float peakAmp = 0.6f, float attackRatio = 0.05f, float decayRatio = 0.3f)
{
    QByteArray out;
    int total = qMax(1, static_cast<int>(SFX_RATE * durationSec));
    int attack = qMax(1, static_cast<int>(total * attackRatio));
    int decay  = qMax(1, static_cast<int>(total * decayRatio));

    for (int i = 0; i < total; ++i) {
        float t = static_cast<float>(i) / SFX_RATE;
        float freq = freqStart + (freqEnd - freqStart) * static_cast<float>(i) / total;
        float phase = 2.0f * static_cast<float>(M_PI) * freq * t;
        float s = std::sin(phase);

        float env;
        if (i < attack) {
            env = static_cast<float>(i) / attack;
        } else if (i < total - decay) {
            env = 1.0f;
        } else {
            float d = static_cast<float>(total - 1 - i) / decay;
            env = d;
        }
        writeInt16Mono(out, s * env * peakAmp);
    }
    return out;
}

static QByteArray noiseBurst(float durationSec, float peakAmp = 0.7f) {
    QByteArray out;
    int total = qMax(1, static_cast<int>(SFX_RATE * durationSec));
    std::mt19937 rng(static_cast<quint32>(1337));
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    for (int i = 0; i < total; ++i) {
        float env = std::exp(-8.0f * static_cast<float>(i) / total);
        writeInt16Mono(out, dist(rng) * env * peakAmp);
    }
    return out;
}

static QByteArray mix(const QByteArray &a, const QByteArray &b, float wa = 0.5f, float wb = 0.5f) {
    QByteArray out;
    int n = qMin(a.size(), b.size()) / 2;
    const qint16 *pa = reinterpret_cast<const qint16 *>(a.constData());
    const qint16 *pb = reinterpret_cast<const qint16 *>(b.constData());
    for (int i = 0; i < n; ++i) {
        float s = (pa[i] / 32768.0f) * wa + (pb[i] / 32768.0f) * wb;
        writeInt16Mono(out, s);
    }
    return out;
}

static QByteArray concat(const QByteArray &a, const QByteArray &b) {
    QByteArray out = a;
    out.append(b);
    return out;
}

AudioManager::WavData AudioManager::synthesizeSfx(SfxType type) {
    WavData result;
    result.sampleRate = SFX_RATE;
    result.channels = 1;
    result.sampleFormat = QAudioFormat::Int16;

    switch (type) {
        case SfxType::Launch: {
            auto tone = toneSweep(300.0f, 1200.0f, 0.28f, 0.55f, 0.05f, 0.2f);
            auto noise = noiseBurst(0.12f, 0.25f);
            result.pcm = tone;
            int noiseStart = result.pcm.size() - noise.size();
            if (noiseStart > 0) {
                for (int i = 0; i < noise.size() / 2; ++i) {
                    qint16 *pDst = reinterpret_cast<qint16 *>(result.pcm.data() + noiseStart);
                    qint16 *pSrc  = reinterpret_cast<qint16 *>(noise.data());
                    float mixed = (pDst[i] / 32768.0f) + (pSrc[i] / 32768.0f) * 0.5f;
                    pDst[i] = static_cast<qint16>(qBound(-1.0f, mixed, 1.0f) * 32767.0f);
                }
            }
            break;
        }
        case SfxType::Hit: {
            auto tone = toneSweep(180.0f, 90.0f, 0.18f, 0.7f, 0.02f, 0.25f);
            auto noise = noiseBurst(0.08f, 0.6f);
            result.pcm = mix(tone, noise, 0.6f, 0.4f);
            break;
        }
        case SfxType::Obstacle: {
            auto tone = toneSweep(600.0f, 250.0f, 0.22f, 0.5f, 0.03f, 0.3f);
            auto noise = noiseBurst(0.12f, 0.45f);
            result.pcm = mix(tone, noise, 0.55f, 0.45f);
            break;
        }
        case SfxType::Miss: {
            result.pcm = toneSweep(500.0f, 120.0f, 0.35f, 0.45f, 0.05f, 0.4f);
            break;
        }
        case SfxType::TurnEnd: {
            auto t1 = toneSweep(880.0f, 880.0f, 0.08f, 0.5f, 0.05f, 0.3f);
            auto t2 = toneSweep(1320.0f, 1320.0f, 0.1f, 0.5f, 0.05f, 0.3f);
            result.pcm = concat(t1, t2);
            break;
        }
        case SfxType::GameOver: {
            auto t1 = toneSweep(523.25f, 523.25f, 0.18f, 0.5f, 0.05f, 0.35f);
            auto t2 = toneSweep(392.0f, 392.0f, 0.18f, 0.5f, 0.05f, 0.35f);
            auto t3 = toneSweep(261.63f, 261.63f, 0.32f, 0.5f, 0.05f, 0.5f);
            result.pcm = concat(concat(t1, t2), t3);
            break;
        }
        case SfxType::Button: {
            auto t = toneSweep(1200.0f, 800.0f, 0.06f, 0.4f, 0.05f, 0.3f);
            result.pcm = t;
            break;
        }
        default: {
            return result;
        }
    }

    return result;
}

#include "AudioManager.moc"