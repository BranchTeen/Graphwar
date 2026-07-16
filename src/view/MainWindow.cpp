#include "view/MainWindow.h"
#include "common/GameState.h"
#include "common/property_ids.h"
#include "widgets/GameCanvas.h"
#include "widgets/FunctionInput.h"
#include "widgets/ConfigPage.h"
#include "widgets/SaveManagerPage.h"
#include "widgets/PauseMenuPage.h"
#include "widgets/GuidePage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>
#include <QTimer>
#include <QDialog>
#include <QFont>
#include <QSlider>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Graphwar");
    resize(900, 700);
    setStyleSheet("background:#0f0f1a;color:#ddd;");

    m_canvas = new GameCanvas(this);
    m_input  = new FunctionInput(this);

    auto *gamePage = new QWidget(this);
    auto *gameLayout = new QVBoxLayout(gamePage);
    gameLayout->setContentsMargins(0, 0, 0, 0);
    gameLayout->setSpacing(0);

    auto *topBar = new QWidget(this);
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 5, 10, 5);

    m_coordLabel = new QLabel(this);
    m_coordLabel->setStyleSheet("color:#ffc;font-size:12px;");
    m_coordLabel->setFixedWidth(110);

    m_p1Label = new QLabel("Player 1", this);
    m_p1Label->setStyleSheet("color:#3c78dc;font-weight:bold;font-size:14px;");
    m_p1Label->setFixedWidth(80);
    m_p2Label = new QLabel("Player 2", this);
    m_p2Label->setStyleSheet("color:#dc3c3c;font-weight:bold;font-size:14px;");
    m_p2Label->setFixedWidth(80);

    m_coordLabel2 = new QLabel(this);
    m_coordLabel2->setStyleSheet("color:#ffc;font-size:12px;");
    m_coordLabel2->setFixedWidth(110);

    m_roundLabel = new QLabel(this);
    m_roundLabel->setStyleSheet("color:#ffcc00;font-size:14px;font-weight:bold;");
    m_pointsLabel = new QLabel(this);
    m_pointsLabel->setStyleSheet("color:#ffcc00;font-size:14px;");

    auto *centerWidget = new QWidget(this);
    auto *centerLayout = new QHBoxLayout(centerWidget);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(8);
    centerLayout->addWidget(m_roundLabel);
    centerLayout->addWidget(m_pointsLabel);

    topLayout->addWidget(m_p1Label);
    topLayout->addWidget(m_coordLabel);
    topLayout->addStretch(1);
    topLayout->addWidget(centerWidget);
    topLayout->addStretch(1);
    topLayout->addWidget(m_coordLabel2);
    topLayout->addWidget(m_p2Label);

    gameLayout->insertWidget(0, topBar);
    gameLayout->addWidget(m_canvas, 1);
    gameLayout->addWidget(m_input);

    m_replayBar = new QWidget(this);
    m_replayBar->setFixedHeight(50);
    m_replayBar->setStyleSheet("background:#1a1a2e; border-top:1px solid #333;");
    auto *replayLayout = new QHBoxLayout(m_replayBar);
    replayLayout->setContentsMargins(12, 6, 12, 6);

    m_replayPauseBtn = new QPushButton(QString::fromUtf8("⏸"), this);
    m_replayPauseBtn->setFixedSize(36, 36);
    m_replayPauseBtn->setCursor(Qt::PointingHandCursor);
    m_replayPauseBtn->setStyleSheet(
        "QPushButton { background: rgba(60, 60, 90, 180); color: white; border-radius: 18px; font-size: 18px; }"
        "QPushButton:hover { background: rgba(80, 130, 200, 200); }"
    );
    connect(m_replayPauseBtn, &QPushButton::clicked, this, [this]() {
        if (m_replayPauseBtn->text() == QString::fromUtf8("⏸")) {
            if (m_replayPauseCmd) m_replayPauseCmd();
            m_replayPauseBtn->setText(QString::fromUtf8("▶"));
        } else {
            if (m_replayResumeCmd) m_replayResumeCmd();
            m_replayPauseBtn->setText(QString::fromUtf8("⏸"));
        }
    });

    m_replayLabel = new QLabel("Replay", this);
    m_replayLabel->setStyleSheet("color: #e8e8ff; font-size: 14px; font-weight: bold;");

    m_replayExitBtn = new QPushButton("✕ Exit", this);
    m_replayExitBtn->setCursor(Qt::PointingHandCursor);
    m_replayExitBtn->setStyleSheet(
        "QPushButton { background: rgba(60, 30, 30, 180); color: white; border-radius: 8px;"
        "padding: 6px 16px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background: rgba(180, 40, 40, 200); }"
    );
    connect(m_replayExitBtn, &QPushButton::clicked, this, &MainWindow::exitReplayMode);

    replayLayout->addWidget(m_replayPauseBtn);
    replayLayout->addSpacing(8);
    replayLayout->addWidget(m_replayLabel, 1);
    replayLayout->addWidget(m_replayExitBtn);

    gameLayout->addWidget(m_replayBar);
    m_replayBar->hide();

    auto *startPage = new QWidget(this);
    startPage->setObjectName("startPage");
    startPage->setStyleSheet(
        "QWidget#startPage { border-image: url(:/start_background.png) 0 0 0 0 stretch stretch; }"
        "QWidget#startPage QLabel { background: rgba(0, 0, 0, 140); border-radius: 12px; }"
        "QWidget#startPage QPushButton {"
        "min-width: 300px; min-height: 56px; border-radius: 12px;"
        "border: 2px solid rgba(255, 255, 255, 220); font-weight: bold;"
        "color: white; font-size: 20px;"
        "background: rgba(40, 40, 60, 170);"
        "}"
        "QWidget#startPage QPushButton:hover {"
        "background: rgba(70, 130, 200, 200); border: 2px solid white;"
        "}"
        "QWidget#startPage QPushButton:pressed {"
        "background: rgba(50, 100, 180, 220);"
        "}"
    );
    auto *startLayout = new QVBoxLayout(startPage);
    startLayout->setAlignment(Qt::AlignCenter);
    startLayout->setSpacing(20);

    auto *titleLabel = new QLabel("GRAPH WAR", this);
    titleLabel->setStyleSheet(
        "color: white; font-size: 72px; font-weight: bold;"
        "padding: 20px 40px; letter-spacing: 4px;"
        "background: rgba(0, 0, 0, 140); border-radius: 16px;"
    );
    titleLabel->setAlignment(Qt::AlignCenter);

    auto *subtitleLabel = new QLabel("Two players, one curve. Fire functions at each other!", this);
    subtitleLabel->setStyleSheet(
        "color: #e8e8ff; font-size: 18px; padding: 10px 24px; border-radius: 10px;"
        "background: rgba(0, 0, 0, 120);"
    );
    subtitleLabel->setAlignment(Qt::AlignCenter);

    auto *newGameBtn = new QPushButton("NEW GAME", this);
    newGameBtn->setCursor(Qt::PointingHandCursor);
    auto *loadGameBtn = new QPushButton("LOAD GAME", this);
    loadGameBtn->setCursor(Qt::PointingHandCursor);
    auto *guideBtn = new QPushButton("HOW TO PLAY", this);
    guideBtn->setCursor(Qt::PointingHandCursor);

    startLayout->addStretch();
    startLayout->addWidget(titleLabel);
    startLayout->addWidget(subtitleLabel);
    startLayout->addWidget(newGameBtn, 0, Qt::AlignCenter);
    startLayout->addWidget(loadGameBtn, 0, Qt::AlignCenter);
    startLayout->addWidget(guideBtn, 0, Qt::AlignCenter);
    startLayout->addStretch();

    connect(newGameBtn, &QPushButton::clicked, this, &MainWindow::goToConfig);
    connect(loadGameBtn, &QPushButton::clicked, this, &MainWindow::goToSaveManager);
    connect(guideBtn, &QPushButton::clicked, this, &MainWindow::goToGuide);

    m_savePage = new SaveManagerPage(this);
    connect(m_savePage, &SaveManagerPage::backRequested, this, &MainWindow::backToStart);
    connect(m_savePage, &SaveManagerPage::loadRequested, this, [this]() { showPage(PageGame); });

    m_configPage = new ConfigPage(this);
    connect(m_configPage, &ConfigPage::backToStart, this, &MainWindow::backToStart);
    connect(m_configPage, &ConfigPage::configSaved, this, [this](const GameConfig &cfg) {
        if (m_setConfigCmd) m_setConfigCmd(cfg);
        startNewGame();
    });

    m_pausePage = new PauseMenuPage(this);
    connect(m_pausePage, &PauseMenuPage::backToTitle, this, &MainWindow::backToStart);

    m_guidePage = new GuidePage(this);
    connect(m_guidePage, &GuidePage::backRequested, this, &MainWindow::backToStart);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(startPage);
    m_stack->addWidget(gamePage);
    m_stack->addWidget(m_savePage);
    m_stack->addWidget(m_pausePage);
    m_stack->addWidget(m_configPage);
    m_stack->addWidget(m_guidePage);
    m_stack->setCurrentIndex(PageStart);

    setupAudioControls();

    auto *centralWidget = new QWidget(this);
    auto *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(m_audioBar);
    centralLayout->addWidget(m_stack);
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() noexcept {}

PropertyNotification MainWindow::get_notification() {
    return [this](uint32_t id) {
        if (!m_state) return;
        auto *s = m_state;
        switch (id) {
        case PROP_ID_TURN:
            updateTopBarColors();
            m_canvas->update();
            updateCoordLabels();
            QTimer::singleShot(0, [this]() { m_input->setFocus(); });
            break;
        case PROP_ID_ROUND:
            m_roundLabel->setText(QString("Round %1").arg(s->roundNumber));
            break;
        case PROP_ID_POINTS:
            m_pointsLabel->setText(QString("Points: %1").arg(s->availablePoints));
            break;
        case PROP_ID_PHASE: {
            m_canvas->update();
            updateCoordLabels();
            bool enabled = (s->phase == GamePhase::WaitingInput);
            m_input->setInputEnabled(enabled);
            if (s->phase == GamePhase::WaitingInput) {
                m_input->clear();
            }
            if (s->phase == GamePhase::RoundEnd) {
                QTimer::singleShot(1500, [this]() {
                    if (m_nextTurnCmd) m_nextTurnCmd();
                });
            }
            break;
        }
        case PROP_ID_TRAJECTORY:
            m_canvas->update();
            break;
        case PROP_ID_MESSAGE:
            m_input->setMessage(s->message);
            break;
        case PROP_ID_COST_PREVIEW:
            m_input->setCostPreview(m_costPreviewPtr ? *m_costPreviewPtr : 0);
            break;
        case PROP_ID_PAUSED:
            if (s->paused && m_stack->currentIndex() == PageGame) {
                showPage(PagePause);
            } else if (!s->paused && m_stack->currentIndex() == PagePause) {
                resumeFromPause();
            }
            break;
        case PROP_ID_GAME_OVER:
            m_canvas->update();
            onGameOver(s->message);
            break;
        case PROP_ID_REPLAY:
            if (s->inReplay) {
                enterReplayMode();
            }
            m_canvas->update();
            updateReplayUI();
            break;
        case PROP_ID_SAVE_RESULT:
            m_savePage->refreshSlots();
            m_pausePage->refreshSlots();
            break;
        case PROP_ID_BGM_VOLUME:
            if (m_state) onBgmVolumeChanged(m_state->bgmVolume);
            break;
        case PROP_ID_BGM_MUTED:
            if (m_state) onBgmMutedChanged(m_state->bgmMuted);
            break;
        case PROP_ID_SFX_VOLUME:
            if (m_state) onSfxVolumeChanged(m_state->sfxVolume);
            break;
        case PROP_ID_SFX_MUTED:
            if (m_state) onSfxMutedChanged(m_state->sfxMuted);
            break;
        }
    };
}

void MainWindow::showPage(PageIndex p) { m_stack->setCurrentIndex(p); }

void MainWindow::updateTopBarColors() {
    if (!m_state) return;
    const auto &cfg = m_state->config;
    auto makeStyle = [](const QColor &c) {
        return QString("color:rgba(%1,%2,%3,%4);font-weight:bold;font-size:14px;")
            .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
    };
    int cur = m_state->currentPlayer;
    m_p1Label->setStyleSheet(cur == 0 ? makeStyle(cfg.player1Color) : QString("color:#555;font-size:14px;"));
    m_p2Label->setStyleSheet(cur == 1 ? makeStyle(cfg.player2Color) : QString("color:#555;font-size:14px;"));
}

void MainWindow::updateCoordLabels() {
    if (!m_state) return;
    int cur = m_state->currentPlayer;
    int idx = m_state->selectedSquareIndex;
    auto sq = m_state->playerSquares[cur].value(idx);
    QString text = QString("(%1,%2)").arg(sq.rect.cx, 0, 'f', 1).arg(sq.rect.cy, 0, 'f', 1);
    if (m_state->config.showCoordinates && !sq.destroyed) {
        if (cur == 0) { m_coordLabel->setText(text); m_coordLabel2->setText(""); }
        else          { m_coordLabel2->setText(text); m_coordLabel->setText(""); }
    } else {
        m_coordLabel->setText(""); m_coordLabel2->setText("");
    }
}

void MainWindow::startNewGame() {
    if (m_newGameCmd) m_newGameCmd();
    updateTopBarColors();
    updateCoordLabels();
    showPage(PageGame);
    QTimer::singleShot(0, [this]() { m_input->setFocus(); });
}

void MainWindow::goToConfig() {
    if (m_state && m_configPage) m_configPage->refresh(m_state->config);
    showPage(PageConfig);
}

void MainWindow::goToSaveManager() { showPage(PageSaveMgr); }

void MainWindow::goToGuide() { showPage(PageGuide); }

void MainWindow::goToPause() {
    if (m_pauseCmd) m_pauseCmd();
    showPage(PagePause);
}

void MainWindow::resumeFromPause() {
    if (m_resumeCmd) m_resumeCmd();
    showPage(PageGame);
    QTimer::singleShot(0, [this]() { m_input->setFocus(); });
}

void MainWindow::backToStart() {
    showPage(PageStart);
    if (m_resumeCmd) m_resumeCmd();
}

void MainWindow::onGameLoaded() { 
    showPage(PageGame); 
    QTimer::singleShot(0, [this]() { m_input->setFocus(); });
}

void MainWindow::onGameOver(const QString &winnerInfo) {
    auto *dialog = new QDialog(this);
    dialog->setWindowTitle("Game Over");
    dialog->setStyleSheet("background: rgba(15, 15, 26, 240); color: #ddd;");
    dialog->setMinimumWidth(500);

    auto *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(16);

    auto *title = new QLabel(winnerInfo, dialog);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "color: white; font-size: 32px; font-weight: bold;"
        "padding: 16px 32px; letter-spacing: 2px;"
        "background: rgba(0, 0, 0, 140); border-radius: 16px;"
    );

    auto *statsWidget = new QWidget(dialog);
    auto *statsLayout = new QHBoxLayout(statsWidget);
    statsLayout->setSpacing(20);

    auto createPlayerStats = [this, dialog](int playerIdx) -> QWidget* {
        const auto &stats = m_state->statistics.player[playerIdx];
        const auto &cfg = m_state->config;
        auto *widget = new QWidget(dialog);
        auto *layout = new QVBoxLayout(widget);
        layout->setSpacing(8);

        QString colorStyle = QString("color:rgba(%1,%2,%3,%4);font-weight:bold;")
            .arg(playerIdx == 0 ? cfg.player1Color.red() : cfg.player2Color.red())
            .arg(playerIdx == 0 ? cfg.player1Color.green() : cfg.player2Color.green())
            .arg(playerIdx == 0 ? cfg.player1Color.blue() : cfg.player2Color.blue())
            .arg(255);

        auto *label = new QLabel(QString("Player %1").arg(playerIdx + 1), widget);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(colorStyle + "font-size: 18px;");

        auto *sep = new QFrame(widget);
        sep->setFrameShape(QFrame::HLine);
        sep->setStyleSheet("color: rgba(255,255,255,50);");

        auto *launchLabel = new QLabel(QString("Launches: %1").arg(stats.launchCount), widget);
        launchLabel->setAlignment(Qt::AlignCenter);
        launchLabel->setStyleSheet("color: #aaa; font-size: 14px;");

        auto *hitLabel = new QLabel(QString("Hits: %1").arg(stats.hitCount), widget);
        hitLabel->setAlignment(Qt::AlignCenter);
        hitLabel->setStyleSheet("color: #aaa; font-size: 14px;");

        auto *obstacleLabel = new QLabel(QString("Obstacles: %1").arg(stats.obstacleHitCount), widget);
        obstacleLabel->setAlignment(Qt::AlignCenter);
        obstacleLabel->setStyleSheet("color: #aaa; font-size: 14px;");

        auto *rateLabel = new QLabel(QString("Hit Rate: %1%").arg(stats.hitRate(), 0, 'f', 1), widget);
        rateLabel->setAlignment(Qt::AlignCenter);
        rateLabel->setStyleSheet("color: #ffcc00; font-size: 14px; font-weight: bold;");

        auto *pointsLabel = new QLabel(QString("Points Spent: %1").arg(stats.totalPointsSpent), widget);
        pointsLabel->setAlignment(Qt::AlignCenter);
        pointsLabel->setStyleSheet("color: #aaa; font-size: 14px;");

        layout->addWidget(label);
        layout->addWidget(sep);
        layout->addWidget(launchLabel);
        layout->addWidget(hitLabel);
        layout->addWidget(obstacleLabel);
        layout->addWidget(rateLabel);
        layout->addWidget(pointsLabel);
        return widget;
    };

    statsLayout->addWidget(createPlayerStats(0));
    statsLayout->addWidget(createPlayerStats(1));

    auto *roundLabel = new QLabel(QString("Total Rounds: %1").arg(m_state->statistics.totalRounds), dialog);
    roundLabel->setAlignment(Qt::AlignCenter);
    roundLabel->setStyleSheet("color: #e8e8ff; font-size: 14px;");

    auto *playAgainBtn = new QPushButton("PLAY AGAIN", dialog);
    playAgainBtn->setCursor(Qt::PointingHandCursor);
    playAgainBtn->setStyleSheet(
        "QPushButton {"
        "min-width: 300px; min-height: 56px; border-radius: 12px;"
        "border: 2px solid rgba(255, 255, 255, 220); font-weight: bold;"
        "color: white; font-size: 20px;"
        "background: rgba(40, 80, 100, 170);"
        "}"
        "QPushButton:hover {"
        "background: rgba(70, 130, 160, 200); border: 2px solid white;"
        "}"
        "QPushButton:pressed {"
        "background: rgba(50, 100, 140, 220);"
        "}"
    );

    auto *backToStartBtn = new QPushButton("BACK TO START", dialog);
    backToStartBtn->setCursor(Qt::PointingHandCursor);
    backToStartBtn->setStyleSheet(
        "QPushButton {"
        "min-width: 300px; min-height: 56px; border-radius: 12px;"
        "border: 2px solid rgba(255, 255, 255, 220); font-weight: bold;"
        "color: white; font-size: 20px;"
        "background: rgba(40, 40, 60, 170);"
        "}"
        "QPushButton:hover {"
        "background: rgba(70, 130, 200, 200); border: 2px solid white;"
        "}"
        "QPushButton:pressed {"
        "background: rgba(50, 100, 180, 220);"
        "}"
    );

    auto *viewReplayBtn = new QPushButton("VIEW REPLAY", dialog);
    viewReplayBtn->setCursor(Qt::PointingHandCursor);
    viewReplayBtn->setStyleSheet(
        "QPushButton {"
        "min-width: 300px; min-height: 48px; border-radius: 12px;"
        "border: 2px solid rgba(255, 200, 100, 220); font-weight: bold;"
        "color: #ffcc44; font-size: 18px;"
        "background: rgba(60, 40, 10, 170);"
        "}"
        "QPushButton:hover {"
        "background: rgba(100, 80, 20, 200); border: 2px solid #ffcc44;"
        "}"
        "QPushButton:pressed {"
        "background: rgba(80, 60, 15, 220);"
        "}"
    );

    layout->addWidget(title);
    layout->addWidget(statsWidget);
    layout->addWidget(roundLabel);
    layout->addWidget(playAgainBtn, 0, Qt::AlignCenter);
    layout->addWidget(viewReplayBtn, 0, Qt::AlignCenter);
    layout->addWidget(backToStartBtn, 0, Qt::AlignCenter);

    connect(playAgainBtn, &QPushButton::clicked, dialog, [=]() {
        dialog->accept();
        if (m_newGameCmd) m_newGameCmd();
        showPage(PageGame);
    });
    connect(viewReplayBtn, &QPushButton::clicked, dialog, [=]() {
        dialog->accept();
        if (m_startReplayCmd) m_startReplayCmd();
    });
    connect(backToStartBtn, &QPushButton::clicked, dialog, [=]() {
        dialog->accept();
        showPage(PageStart);
    });

    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::enterReplayMode() {
    showPage(PageGame);
    m_input->hide();
    m_replayBar->show();
    m_replayPauseBtn->setText(QString::fromUtf8("⏸"));
    updateReplayUI();
}

void MainWindow::exitReplayMode() {
    m_replayBar->hide();
    m_input->show();
    if (m_stopReplayCmd) m_stopReplayCmd();
    showPage(PageStart);
}

void MainWindow::updateReplayUI() {
    if (!m_state) return;
    if (m_state->inReplay) {
        m_replayLabel->setText(QString("Replay: Shot %1/%2")
            .arg(m_state->replayCurrentShot + 1)
            .arg(m_state->replayTotalShots));
        if (m_state->replayCurrentShot >= m_state->replayTotalShots - 1) {
            m_replayPauseBtn->setText(QString::fromUtf8("▶"));
        }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        int current = m_stack->currentIndex();
        if (current == PageGame) {
            goToPause();
        } else if (current == PagePause) {
            resumeFromPause();
        } else if (current == PageSaveMgr || current == PageConfig) {
            backToStart();
        } else {
            QMainWindow::keyPressEvent(event);
        }
        return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::setupAudioControls() {
    int bgmVol = m_state ? m_state->bgmVolume : 60;
    bool bgmMuted = m_state ? m_state->bgmMuted : false;
    int sfxVol = m_state ? m_state->sfxVolume : 80;
    bool sfxMuted = m_state ? m_state->sfxMuted : false;

    m_audioBar = new QWidget(this);
    m_audioBar->setFixedHeight(40);
    m_audioBar->setStyleSheet("QWidget { background:#15151f; border-bottom:1px solid #24243a; }");

    auto *barLayout = new QHBoxLayout(m_audioBar);
    barLayout->setContentsMargins(12, 4, 12, 4);
    barLayout->setSpacing(10);

    auto *iconLabel = new QLabel(QString::fromUtf8("♪"), this);
    iconLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold;");

    bool bothMuted = bgmMuted && sfxMuted;
    m_muteBtn = new QPushButton(bothMuted ? QString::fromUtf8("🔇 UNMUTE") : QString::fromUtf8("🔊 MUTE"), this);
    m_muteBtn->setCursor(Qt::PointingHandCursor);
    m_muteBtn->setStyleSheet(
        "QPushButton {"
        "background: rgba(40, 40, 60, 170); color: white; border: 1px solid rgba(255, 255, 255, 150);"
        "padding: 4px 12px; border-radius: 8px; font-size: 12px; font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background: rgba(70, 130, 200, 200); border: 1px solid white;"
        "}"
        "QPushButton:pressed {"
        "background: rgba(50, 100, 180, 220);"
        "}"
    );

    auto sliderStyle =
        "QSlider::groove:horizontal { height: 4px; background: rgba(40, 40, 60, 150); border-radius: 2px; }"
        "QSlider::handle:horizontal { background: white; width: 14px; height: 14px;"
        "margin: -6px 0; border-radius: 7px; }"
        "QSlider::handle:horizontal:hover { background: rgba(70, 130, 200, 200); }";

    auto *bgmTag = new QLabel("BGM", this);
    bgmTag->setStyleSheet("color: #e8e8ff; font-size: 12px; font-weight: bold; min-width: 32px;");
    m_bgmVolumeSlider = new QSlider(Qt::Horizontal, this);
    m_bgmVolumeSlider->setRange(0, 100);
    m_bgmVolumeSlider->setValue(bgmVol);
    m_bgmVolumeSlider->setMinimumWidth(120);
    m_bgmVolumeSlider->setStyleSheet(sliderStyle);
    m_bgmVolumeLabel = new QLabel(QString("%1%").arg(bgmVol), this);
    m_bgmVolumeLabel->setStyleSheet("color: #e8e8ff; font-size: 12px; font-weight: bold; min-width: 36px;");

    auto *sfxTag = new QLabel("SFX", this);
    sfxTag->setStyleSheet("color: #e8e8ff; font-size: 12px; font-weight: bold; min-width: 32px;");
    m_sfxVolumeSlider = new QSlider(Qt::Horizontal, this);
    m_sfxVolumeSlider->setRange(0, 100);
    m_sfxVolumeSlider->setValue(sfxVol);
    m_sfxVolumeSlider->setMinimumWidth(120);
    m_sfxVolumeSlider->setStyleSheet(sliderStyle);
    m_sfxVolumeLabel = new QLabel(QString("%1%").arg(sfxVol), this);
    m_sfxVolumeLabel->setStyleSheet("color: #e8e8ff; font-size: 12px; font-weight: bold; min-width: 36px;");

    barLayout->addWidget(iconLabel);
    barLayout->addWidget(m_muteBtn);
    barLayout->addSpacing(8);
    barLayout->addWidget(bgmTag);
    barLayout->addWidget(m_bgmVolumeSlider, 1);
    barLayout->addWidget(m_bgmVolumeLabel);
    barLayout->addSpacing(8);
    barLayout->addWidget(sfxTag);
    barLayout->addWidget(m_sfxVolumeSlider, 1);
    barLayout->addWidget(m_sfxVolumeLabel);

    connect(m_muteBtn, &QPushButton::clicked, this, &MainWindow::onGlobalMutedClicked);
    connect(m_bgmVolumeSlider, &QSlider::valueChanged, this, [this](int v) {
        if (m_setBgmVolumeCmd) m_setBgmVolumeCmd(v);
    });
    connect(m_sfxVolumeSlider, &QSlider::valueChanged, this, [this](int v) {
        if (m_setSfxVolumeCmd) m_setSfxVolumeCmd(v);
    });
}

void MainWindow::onGlobalMutedClicked() {
    if (!m_state) return;
    bool bothMuted = m_state->bgmMuted && m_state->sfxMuted;
    bool target = !bothMuted;
    if (m_setBgmMutedCmd) m_setBgmMutedCmd(target);
    if (m_setSfxMutedCmd) m_setSfxMutedCmd(target);
}

void MainWindow::onBgmVolumeChanged(int v) {
    if (m_bgmVolumeSlider && m_bgmVolumeSlider->value() != v) {
        m_bgmVolumeSlider->blockSignals(true);
        m_bgmVolumeSlider->setValue(v);
        m_bgmVolumeSlider->blockSignals(false);
    }
    if (m_bgmVolumeLabel) m_bgmVolumeLabel->setText(QString("%1%").arg(v));
}

void MainWindow::onSfxVolumeChanged(int v) {
    if (m_sfxVolumeSlider && m_sfxVolumeSlider->value() != v) {
        m_sfxVolumeSlider->blockSignals(true);
        m_sfxVolumeSlider->setValue(v);
        m_sfxVolumeSlider->blockSignals(false);
    }
    if (m_sfxVolumeLabel) m_sfxVolumeLabel->setText(QString("%1%").arg(v));
}

void MainWindow::onBgmMutedChanged(bool) {
    if (!m_state || !m_muteBtn) return;
    bool both = m_state->bgmMuted && m_state->sfxMuted;
    m_muteBtn->setText(both ? QString::fromUtf8("🔇 UNMUTE") : QString::fromUtf8("🔊 MUTE"));
}

void MainWindow::onSfxMutedChanged(bool) {
    if (!m_state || !m_muteBtn) return;
    bool both = m_state->bgmMuted && m_state->sfxMuted;
    m_muteBtn->setText(both ? QString::fromUtf8("🔇 UNMUTE") : QString::fromUtf8("🔊 MUTE"));
}
