#pragma once
#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QAudioSink>
#include <QIODevice>
#include <QAudioFormat>
#include <QMutex>
#include <QUrl>
#include <QString>
#include <QVector>
#include <QMap>
#include "common/AudioState.h"

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
    QMediaPlayer *m_bgmPlayer;
    QAudioOutput *m_bgmAudioOutput;

    int m_sfxVolume;
    bool m_sfxMuted;

    QVector<class SfxPlayer *> m_activeSfx;
    QMap<int, WavData> m_sfxCache;
    QMutex m_sfxCacheMutex;
};
