#pragma once
#include <QObject>
#include <QString>
#include "model/GameModel.h"
#include "model/SaveInfo.h"
#include "common/EventBus.h"

class GameViewModel : public QObject {
    Q_OBJECT
public:
    explicit GameViewModel(QObject *parent = nullptr);
    ~GameViewModel();

private slots:
    void onCmdNewGame();
    void onCmdLaunch(const QString &expr);
    void onCmdUpdateCostPreview(const QString &expr);
    void onCmdNextTurn();
    void onCmdPause();
    void onCmdResume();
    void onCmdTogglePause();
    void onCmdSetConfig(const GameConfig &cfg);
    void onCmdSaveToSlot(int slot);
    void onCmdLoadFromSlot(int slot);
    void onCmdDeleteSlot(int slot);

private:
    void forwardModelSignals();
    void registerGetters();
    void connectCommands();

    GameModel *m_model = nullptr;
    int m_costPreview = 0;
};
