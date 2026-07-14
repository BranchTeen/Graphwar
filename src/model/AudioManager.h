#pragma once
#include <QObject>
#include <QAudioSink>
#include <QIODevice>
#include <QAudioFormat>
#include <QThread>
#include <QMutex>
#include <QUrl>
#include <QString>
#include <QDateTime>
#include <QCoreApplication>
#include <QVector>
#include "common/AudioState.h"

class AudioDecoderThread;
class SfxPlayer;

class AudioManager : public QObject {
    Q_OBJECT
public:
    static AudioManager &instance();

    void playBackgroundMusic(const QUrl &source);
    void stopBackgroundMusic();

    int bgmVolume() const;
    void setBgmVolume(int v);
    bool bgmMuted() const;
    void setBgmMuted(bool m);
    void toggleBgmMuted();

    void playSfx(SfxType type);

    void stopAllSfx();

    int sfxVolume() const;
    void setSfxVolume(int v);
    bool sfxMuted() const;
    void setSfxMuted(bool m);
    void toggleSfxMuted();

signals:
    void bgmVolumeChanged(int v);
    void bgmMutedChanged(bool m);
    void sfxVolumeChanged(int v);
    void sfxMutedChanged(bool m);

private:
    explicit AudioManager(QObject *parent = nullptr);
    ~AudioManager() override;

    bool ensureBgmSinkCreated(int sampleRate, int channels);
    void writeBgmPcm(const char *data, int bytes);

    struct WavData {
        int sampleRate = 0;
        int channels = 0;
        QAudioFormat::SampleFormat sampleFormat = QAudioFormat::Unknown;
        QByteArray pcm;
        bool valid() const { return sampleRate > 0 && !pcm.isEmpty(); }
    };

    static WavData synthesizeSfx(SfxType type);

    int m_bgmVolume;
    bool m_bgmMuted;
    QString m_currentFilePath;
    AudioDecoderThread *m_decoder;

    QMutex m_bgmSinkMutex;
    QAudioSink *m_bgmSink;
    QIODevice *m_bgmSinkIo;

    int m_sfxVolume;
    bool m_sfxMuted;

    QVector<SfxPlayer *> m_activeSfx;
    QMap<int, WavData> m_sfxCache;
    QMutex m_sfxCacheMutex;

    friend class AudioDecoderThread;
};