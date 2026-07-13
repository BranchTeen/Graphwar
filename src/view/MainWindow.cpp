#include "view/MainWindow.h"
#include "viewmodel/GameViewModel.h"
#include "common/property_ids.h"
#include "widgets/GameCanvas.h"
#include "widgets/FunctionInput.h"
#include "widgets/ConfigPage.h"
#include "widgets/SaveManagerPage.h"
#include "widgets/PauseMenuPage.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QKeyEvent>
#include <QTimer>
#include <QDialog>
#include <QFont>

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
    m_coordLabel->hide();

    m_p1Label = new QLabel("Player 1", this);
    m_p1Label->setStyleSheet("color:#3c78dc;font-weight:bold;font-size:14px;");
    m_p2Label = new QLabel("Player 2", this);
    m_p2Label->setStyleSheet("color:#dc3c3c;font-weight:bold;font-size:14px;");

    m_roundLabel = new QLabel(this);
    m_roundLabel->setStyleSheet("color:#ffcc00;font-size:14px;font-weight:bold;");
    m_pointsLabel = new QLabel(this);
    m_pointsLabel->setStyleSheet("color:#ffcc00;font-size:14px;");

    m_coordLabel2 = new QLabel(this);
    m_coordLabel2->setStyleSheet("color:#ffc;font-size:12px;");
    m_coordLabel2->hide();

    topLayout->addWidget(m_p1Label);
    topLayout->addWidget(m_coordLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_roundLabel);
    topLayout->addWidget(m_pointsLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_coordLabel2);
    topLayout->addWidget(m_p2Label);

    gameLayout->insertWidget(0, topBar);
    gameLayout->addWidget(m_canvas, 1);
    gameLayout->addWidget(m_input);

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

    startLayout->addStretch();
    startLayout->addWidget(titleLabel);
    startLayout->addWidget(subtitleLabel);
    startLayout->addWidget(newGameBtn, 0, Qt::AlignCenter);
    startLayout->addWidget(loadGameBtn, 0, Qt::AlignCenter);
    startLayout->addStretch();

    connect(newGameBtn, &QPushButton::clicked, this, &MainWindow::goToConfig);
    connect(loadGameBtn, &QPushButton::clicked, this, &MainWindow::goToSaveManager);

    m_savePage = new SaveManagerPage(this);
    connect(m_savePage, &SaveManagerPage::backRequested, this, &MainWindow::backToStart);

    m_configPage = new ConfigPage(this);
    connect(m_configPage, &ConfigPage::backToStart, this, &MainWindow::backToStart);
    connect(m_configPage, &ConfigPage::configSaved, this, [this](const GameConfig &cfg) {
        if (m_setConfigCmd) m_setConfigCmd(cfg);
        startNewGame();
    });

    m_pausePage = new PauseMenuPage(this);
    connect(m_pausePage, &PauseMenuPage::backToTitle, this, &MainWindow::backToStart);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(startPage);
    m_stack->addWidget(gamePage);
    m_stack->addWidget(m_savePage);
    m_stack->addWidget(m_pausePage);
    m_stack->addWidget(m_configPage);
    m_stack->setCurrentIndex(PageStart);
    setCentralWidget(m_stack);
}

MainWindow::~MainWindow() noexcept {}

PropertyNotification MainWindow::get_notification() {
    return [this](uint32_t id) {
        if (!m_vm) return;
        const auto *model = m_vm->get_model();
        switch (id) {
        case PROP_ID_TURN:
            updateTopBarColors();
            m_canvas->update();
            updateCoordLabels(model);
            break;
        case PROP_ID_ROUND:
            m_roundLabel->setText(QString("Round %1").arg(model->roundNumber()));
            break;
        case PROP_ID_POINTS:
            m_pointsLabel->setText(QString("Points: %1").arg(model->availablePoints()));
            break;
        case PROP_ID_PHASE: {
            m_canvas->update();
            updateCoordLabels(model);
            bool enabled = (model->phase() == GamePhase::WaitingInput);
            m_input->setInputEnabled(enabled);
            if (model->phase() == GamePhase::RoundEnd) {
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
            m_input->setMessage(model->message());
            break;
        case PROP_ID_COST_PREVIEW:
            m_input->setCostPreview(m_vm->costPreview());
            break;
        case PROP_ID_PAUSED:
            if (model->paused() && m_stack->currentIndex() == PageGame) {
                showPage(PagePause);
            } else if (!model->paused() && m_stack->currentIndex() == PagePause) {
                resumeFromPause();
            }
            break;
        case PROP_ID_GAME_OVER:
            m_canvas->update();
            onGameOver(model->message());
            break;
        case PROP_ID_SAVE_RESULT:
            m_savePage->refreshSlots();
            m_pausePage->refreshSlots();
            break;
        }
    };
}

void MainWindow::showPage(PageIndex p) { m_stack->setCurrentIndex(p); }

void MainWindow::updateTopBarColors() {
    if (!m_vm) return;
    const auto *model = m_vm->get_model();
    const auto &cfg = model->config();
    auto makeStyle = [](const QColor &c) {
        return QString("color:rgba(%1,%2,%3,%4);font-weight:bold;font-size:14px;")
            .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
    };
    int cur = model->currentPlayer();
    m_p1Label->setStyleSheet(cur == 0 ? makeStyle(cfg.player1Color) : QString("color:#555;font-size:14px;"));
    m_p2Label->setStyleSheet(cur == 1 ? makeStyle(cfg.player2Color) : QString("color:#555;font-size:14px;"));
}

void MainWindow::updateCoordLabels(const GameModel *model) {
    int cur = model->currentPlayer();
    int idx = model->selectedSquareIndex();
    auto sq = model->playerSquares(cur).value(idx);
    QString text = QString("(%1,%2)").arg(sq.rect.cx, 0, 'f', 1).arg(sq.rect.cy, 0, 'f', 1);
    if (model->config().showCoordinates && !sq.destroyed) {
        if (cur == 0) { m_coordLabel->setText(text); m_coordLabel->show(); m_coordLabel2->hide(); }
        else          { m_coordLabel2->setText(text); m_coordLabel2->show(); m_coordLabel->hide(); }
    } else {
        m_coordLabel->hide(); m_coordLabel2->hide();
    }
}

void MainWindow::startNewGame() {
    if (m_newGameCmd) m_newGameCmd();
    updateTopBarColors();
    if (m_vm) updateCoordLabels(m_vm->get_model());
    showPage(PageGame);
}

void MainWindow::goToConfig() {
    if (m_vm && m_configPage) m_configPage->refresh(m_vm->get_model()->config());
    showPage(PageConfig);
}

void MainWindow::goToSaveManager() { showPage(PageSaveMgr); }

void MainWindow::goToPause() {
    if (m_pauseCmd) m_pauseCmd();
    showPage(PagePause);
}

void MainWindow::resumeFromPause() {
    if (m_resumeCmd) m_resumeCmd();
    showPage(PageGame);
}

void MainWindow::backToStart() {
    showPage(PageStart);
    if (m_resumeCmd) m_resumeCmd();
}

void MainWindow::onGameLoaded() { showPage(PageGame); }

void MainWindow::onGameOver(const QString &winnerInfo) {
    auto *dialog = new QDialog(this);
    dialog->setWindowTitle("Game Over");
    dialog->setStyleSheet("background:#141420;color:#ddd;");
    dialog->setMinimumWidth(420);

    auto *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    auto *title = new QLabel(winnerInfo, dialog);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color:#4af;font-size:28px;font-weight:bold;");

    auto *hint = new QLabel("Do you want to play again?", dialog);
    hint->setAlignment(Qt::AlignCenter);
    hint->setStyleSheet("color:#aaa;font-size:16px;");

    auto *playAgainBtn = new QPushButton("PLAY AGAIN", dialog);
    playAgainBtn->setCursor(Qt::PointingHandCursor);
    playAgainBtn->setStyleSheet(
        "QPushButton { background:#2a7; color:white; font-size:18px; font-weight:bold;"
        "padding:12px 40px; border-radius:8px; }"
        "QPushButton:hover { background:#3c9; }");

    auto *backToStartBtn = new QPushButton("BACK TO START PAGE", dialog);
    backToStartBtn->setCursor(Qt::PointingHandCursor);
    backToStartBtn->setStyleSheet(
        "QPushButton { background:#384; color:white; font-size:16px;"
        "padding:10px 40px; border-radius:8px; }"
        "QPushButton:hover { background:#4a5; }");

    layout->addWidget(title);
    layout->addWidget(hint);
    layout->addWidget(playAgainBtn, 0, Qt::AlignCenter);
    layout->addWidget(backToStartBtn, 0, Qt::AlignCenter);

    connect(playAgainBtn, &QPushButton::clicked, dialog, [=]() {
        dialog->accept();
        if (m_newGameCmd) m_newGameCmd();
        showPage(PageGame);
    });
    connect(backToStartBtn, &QPushButton::clicked, dialog, [=]() {
        dialog->accept();
        showPage(PageStart);
    });

    dialog->exec();
    dialog->deleteLater();
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
