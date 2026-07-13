#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QPushButton>
#include <QDialog>
#include <QFont>
#include "viewmodel/GameViewModel.h"

MainWindow::MainWindow(GameViewModel *vm, QWidget *parent) : QMainWindow(parent), m_vm(vm) {
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

    m_p1Label = new QLabel("Player 1", this);
    m_p1Label->setStyleSheet("color:#3c78dc;font-weight:bold;font-size:14px;");
    m_p2Label = new QLabel("Player 2", this);
    m_p2Label->setStyleSheet("color:#dc3c3c;font-weight:bold;font-size:14px;");
    auto *roundLabel = new QLabel(this);
    roundLabel->setStyleSheet("color:#ffcc00;font-size:14px;font-weight:bold;");
    auto *pointsLabel = new QLabel(this);
    pointsLabel->setStyleSheet("color:#ffcc00;font-size:14px;");

    topLayout->addWidget(m_p1Label);
    topLayout->addStretch();
    topLayout->addWidget(roundLabel);
    topLayout->addWidget(pointsLabel);
    topLayout->addStretch();
    topLayout->addWidget(m_p2Label);

    gameLayout->insertWidget(0, topBar);
    gameLayout->addWidget(m_canvas, 1);
    gameLayout->addWidget(m_input);

    auto &bus = EventBus::instance();
    connect(&bus, &EventBus::evtTurnChanged, this, [this](int) {
        updateTopBarColors();
    });
    connect(&bus, &EventBus::evtRoundChanged, this, [=](int r) {
        roundLabel->setText(QString("Round %1").arg(r));
    });
    connect(&bus, &EventBus::evtPointsChanged, this, [=](int pts) {
        pointsLabel->setText(QString("Points: %1").arg(pts));
    });
    connect(&bus, &EventBus::evtTrajectoryUpdated, m_canvas, QOverload<>::of(&QWidget::update));
    connect(&bus, &EventBus::evtPhaseChanged,      m_canvas, QOverload<>::of(&QWidget::update));
    connect(&bus, &EventBus::evtTurnChanged,       m_canvas, QOverload<>::of(&QWidget::update));
    connect(&bus, &EventBus::evtGameOver,          m_canvas, QOverload<>::of(&QWidget::update));

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
    connect(&bus, &EventBus::evtSaveResult, this, [this](int slot, bool ok, const QString &) {
        if (ok) onGameLoaded();
    });

    m_configPage = new ConfigPage(this);
    connect(m_configPage, &ConfigPage::backToStart, this, &MainWindow::backToStart);
    connect(m_configPage, &ConfigPage::configSaved, this, [this, &bus](const GameConfig &cfg) {
        emit bus.cmdSetConfig(cfg);
        startNewGame();
    });

    m_pausePage = new PauseMenuPage(this);
    connect(m_pausePage, &PauseMenuPage::backToTitle, this, &MainWindow::backToStart);

    connect(&bus, &EventBus::evtPausedChanged, this, [this](bool paused) {
        if (!paused && m_stack->currentIndex() == PagePause) {
            resumeFromPause();
        }
    });

    connect(&bus, &EventBus::evtGameOver, this, &MainWindow::onGameOver);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(startPage);
    m_stack->addWidget(gamePage);
    m_stack->addWidget(m_savePage);
    m_stack->addWidget(m_pausePage);
    m_stack->addWidget(m_configPage);
    m_stack->setCurrentIndex(PageStart);
    setCentralWidget(m_stack);
}

void MainWindow::showPage(PageIndex p) {
    m_stack->setCurrentIndex(p);
}

void MainWindow::updateTopBarColors() {
    auto &bus = EventBus::instance();
    const auto &cfg = bus.config();
    auto makeStyle = [](const QColor &c) {
        return QString("color:rgba(%1,%2,%3,%4);font-weight:bold;font-size:14px;")
            .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
    };
    auto dimStyle = QString("color:#555;font-size:14px;");
    int cur = bus.currentPlayer();
    m_p1Label->setStyleSheet(cur == 0 ? makeStyle(cfg.player1Color) : dimStyle);
    m_p2Label->setStyleSheet(cur == 1 ? makeStyle(cfg.player2Color) : dimStyle);
}

void MainWindow::startNewGame() {
    emit EventBus::instance().cmdNewGame();
    updateTopBarColors();
    showPage(PageGame);
}

void MainWindow::goToConfig() {
    if (m_configPage) m_configPage->refresh(EventBus::instance().config());
    showPage(PageConfig);
}

void MainWindow::goToSaveManager() {
    showPage(PageSaveMgr);
}

void MainWindow::goToPause() {
    emit EventBus::instance().cmdPause();
    showPage(PagePause);
}

void MainWindow::resumeFromPause() {
    emit EventBus::instance().cmdResume();
    showPage(PageGame);
}

void MainWindow::backToStart() {
    emit EventBus::instance().cmdResume();
    showPage(PageStart);
}

void MainWindow::onGameLoaded() {
    showPage(PageGame);
}

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
        emit EventBus::instance().cmdNewGame();
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
