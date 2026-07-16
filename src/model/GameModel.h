#pragma once
#include <QObject>
#include <QString>
#include <QVector>
#include <QPointF>
#include <QColor>
class QTimer;
#include "Player.h"
#include "common/SaveInfo.h"
#include "common/GamePhase.h"
#include "common/GameConfig.h"
#include "common/Square.h"
#include "common/AudioState.h"
#include "common/Particle.h"
#include "common/GameStatistics.h"

// GameModel：MVVM 的 Model 层
// - 持有所有游戏状态（玩家、方块、障碍物、轨迹、回合数等）
// - 实现核心业务逻辑（发射、动画推进、回合切换、胜负判断、JSON 序列化）
// - 通过 signals 通知上层（ViewModel）状态变化
// - 不依赖任何 View 层代码，不知道 UI 的存在
class GameModel : public QObject {
    Q_OBJECT
public:
    explicit GameModel(QObject *parent = nullptr);

    // ===== 状态查询（只读接口） =====
    int currentPlayer() const { return m_currentPlayer; }
    int roundNumber() const { return m_roundNumber; }
    int availablePoints() const { return 3 + m_pointsLevel * 2; }
    int pointsLevel() const { return m_pointsLevel; }
    GamePhase phase() const { return m_phase; }
    QString message() const { return m_message; }
    bool paused() const { return m_paused; }
    int selectedSquareIndex() const { return m_selectedSquare; }

    QColor playerColor(int player) const {
        if (player < 0 || player >= 2) return QColor(128, 128, 128);
        return m_players[player].color;
    }

    int aliveCount(int player) const {
        if (player < 0 || player >= 2) return 0;
        return m_players[player].aliveCount();
    }

    QVector<Square> playerSquares(int player) const {
        if (player < 0 || player >= 2) return {};
        return m_players[player].squares;  // 返回副本，外部不会修改内部状态
    }

    QVector<Square> obstacles() const { return m_obstacles; }
    QVector<QPointF> trajectory() const { return m_trajectory; }
    QVector<QVector<QPointF>> history() const { return m_history; }
    QVector<Particle> particles() const { return m_particles; }

    const GameStatistics &statistics() const { return m_stats; }

    const GameConfig &config() const { return m_config; }
    void setConfig(const GameConfig &cfg) { m_config = cfg; }

    bool isGameOver() const { return m_phase == GamePhase::GameOver; }
    bool isWaitingInput() const { return m_phase == GamePhase::WaitingInput; }
    bool isAnimating() const { return m_phase == GamePhase::Animating; }

    // ===== 业务操作（供 ViewModel 调用） =====
    void newGame(const GameConfig &cfg);
    int calculateCost(const QString &expr) const;   // 返回表达式消耗的点数
    bool launch(const QString &expr);                // 尝试发射，返回是否成功
    void stepAnimation();                            // 推进一帧动画
    void nextTurn();                                 // 结束当前回合，切到下一个玩家
    void pause();
    void resume();
    void togglePause();

    // ===== 序列化（Model 层自序列化 + 反序列化） =====
    QString toJson() const;
    bool fromJson(const QString &text);

    // ===== 存档管理（通过 SaveManager） =====
    bool saveToSlot(int slot);
    bool loadFromSlot(int slot);
    bool deleteSlot(int slot);
    static int slotCount();
    static SaveInfo slotInfo(int slot);
    static QVector<SaveInfo> slotInfos();

    // ===== 音频管理（通过 AudioManager） =====
    int bgmVolume() const;
    void setBgmVolume(int v);
    bool bgmMuted() const;
    void setBgmMuted(bool m);
    void toggleBgmMuted();
    int sfxVolume() const;
    void setSfxVolume(int v);
    bool sfxMuted() const;
    void setSfxMuted(bool m);
    void toggleSfxMuted();
    void playSfx(SfxType type);
    void playBackgroundMusic(const QUrl &source);
    void stopBackgroundMusic();

signals:
    void bgmVolumeChanged(int v);
    void bgmMutedChanged(bool m);
    void sfxVolumeChanged(int v);
    void sfxMutedChanged(bool m);
    void turnChanged(int player);
    void roundChanged(int round);
    void pointsChanged(int points);
    void phaseChanged(GamePhase phase);
    void messageChanged(const QString &msg);
    void squareHit(int playerId, int squareIndex);
    void trajectoryUpdated();
    void gameOver(const QString &winnerInfo);
    void pausedChanged(bool paused);

private:
    void generateSquares(int count, const QColor &p1Color, const QColor &p2Color);
    void generateObstacles(int count, double size);
    void pickRandomSquare();
    void spawnParticles(const QPointF &pos, const QColor &color, int count = 8);
    void updateParticles();
    void onParticleTimer();

    // ===== 持久化游戏状态 =====
    Player m_players[2];
    QVector<Square> m_obstacles;
    int m_currentPlayer = 0;
    int m_selectedSquare = -1;
    int m_roundNumber = 0;
    int m_pointsLevel = 0;
    GamePhase m_phase = GamePhase::WaitingInput;
    QVector<QPointF> m_trajectory;
    QVector<QVector<QPointF>> m_history;
    QString m_message;
    bool m_paused = false;
    GameConfig m_config;

    // ===== 动画/发射的临时状态 =====
    QString m_currentExpr;
    double m_animX = 0;
    double m_animStep = 0.15;
    double m_constAdjust = 0;
    bool m_hasHit = false;
    QTimer *m_animTimer = nullptr;   // Model 自身管理动画节奏
    QTimer *m_particleTimer = nullptr; // 粒子更新定时器

    // ===== 粒子效果 =====
    QVector<Particle> m_particles;

    // ===== 统计数据 =====
    GameStatistics m_stats;
};
