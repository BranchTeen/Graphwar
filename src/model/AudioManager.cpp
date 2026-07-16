#include "AudioManager.h"
#include "common/AudioState.h"
#include <QUrl>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>
#include <QMap>
#include <QMediaDevices>
#include <QAudioDevice>
#include <cmath>
#include <random>

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

AudioManager::AudioManager(QObject *parent)
    : QObject(parent),
      m_bgmVolume(60),
      m_bgmMuted(false),
      m_bgmPlayer(nullptr),
      m_bgmAudioOutput(nullptr),
      m_sfxVolume(80),
      m_sfxMuted(false) {
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
    stopBackgroundMusic();

    if (!source.isValid() || !source.isLocalFile()) {
        qDebug() << "[AudioManager] Invalid or non-local URL:" << source;
        return;
    }

    QString localPath = source.toLocalFile();
    qDebug() << "[AudioManager] Playing BGM:" << localPath
             << "exists:" << QFileInfo::exists(localPath);

    if (!QFileInfo::exists(localPath)) return;

    m_bgmAudioOutput = new QAudioOutput(this);
    m_bgmAudioOutput->setVolume(m_bgmMuted ? 0.0f : (m_bgmVolume / 100.0f));

    m_bgmPlayer = new QMediaPlayer(this);
    m_bgmPlayer->setAudioOutput(m_bgmAudioOutput);

    connect(m_bgmPlayer, &QMediaPlayer::errorOccurred, this, [](QMediaPlayer::Error err, const QString &desc) {
        qDebug() << "[AudioManager] BGM error:" << err << desc;
    });

    connect(m_bgmPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        qDebug() << "[AudioManager] BGM status:" << status;
        if (status == QMediaPlayer::EndOfMedia) {
            m_bgmPlayer->setPosition(0);
            m_bgmPlayer->play();
        }
    });

    m_bgmPlayer->setSource(source);
    m_bgmPlayer->play();
}

void AudioManager::stopBackgroundMusic() {
    if (m_bgmPlayer) {
        m_bgmPlayer->stop();
        delete m_bgmPlayer;
        m_bgmPlayer = nullptr;
    }
    if (m_bgmAudioOutput) {
        delete m_bgmAudioOutput;
        m_bgmAudioOutput = nullptr;
    }
}

int AudioManager::bgmVolume() const { return m_bgmVolume; }

void AudioManager::setBgmVolume(int v) {
    int clamped = qBound(0, v, 100);
    if (clamped == m_bgmVolume) return;
    m_bgmVolume = clamped;
    if (m_bgmAudioOutput && !m_bgmMuted)
        m_bgmAudioOutput->setVolume(m_bgmVolume / 100.0f);
    emit bgmVolumeChanged(m_bgmVolume);
}

bool AudioManager::bgmMuted() const { return m_bgmMuted; }

void AudioManager::setBgmMuted(bool m) {
    if (m == m_bgmMuted) return;
    m_bgmMuted = m;
    if (m_bgmAudioOutput)
        m_bgmAudioOutput->setMuted(m_bgmMuted);
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
    for (auto *p : std::as_const(m_activeSfx)) {
        p->updateVolume(vf);
    }
    emit sfxVolumeChanged(m_sfxVolume);
}

bool AudioManager::sfxMuted() const { return m_sfxMuted; }

void AudioManager::setSfxMuted(bool m) {
    if (m == m_sfxMuted) return;
    m_sfxMuted = m;
    float vf = m_sfxMuted ? 0.0f : (m_sfxVolume / 100.0f);
    for (auto *p : std::as_const(m_activeSfx)) {
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
