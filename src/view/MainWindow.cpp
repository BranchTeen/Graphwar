#include "MainWindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Graphwar");
    resize(900, 700);

    m_vm = new GameViewModel(this);
    m_canvas = new GameCanvas(m_vm, this);
    m_input = new FunctionInput(m_vm, this);

    // Game page
    auto *gamePage = new QWidget(this);
    auto *gameLayout = new QVBoxLayout(gamePage);
    gameLayout->setContentsMargins(0, 0, 0, 0);
    gameLayout->setSpacing(0);
    gameLayout->addWidget(m_canvas, 1);
    gameLayout->addWidget(m_input);

    // Top bar
    auto *topBar = new QWidget(this);
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(10, 5, 10, 5);

    auto *p1Label = new QLabel("Player 1", this);
    p1Label->setStyleSheet("color: #3c78dc; font-weight: bold; font-size: 14px;");
    auto *p2Label = new QLabel("Player 2", this);
    p2Label->setStyleSheet("color: #dc3c3c; font-weight: bold; font-size: 14px;");
    auto *roundLabel = new QLabel(this);
    roundLabel->setStyleSheet("color: #ffcc00; font-size: 14px; font-weight: bold;");
    auto *pointsLabel = new QLabel(this);
    pointsLabel->setStyleSheet("color: #ffcc00; font-size: 14px;");

    topLayout->addWidget(p1Label);
    topLayout->addStretch();
    topLayout->addWidget(roundLabel);
    topLayout->addWidget(pointsLabel);
    topLayout->addStretch();
    topLayout->addWidget(p2Label);

    gameLayout->insertWidget(0, topBar);

    connect(m_vm, &GameViewModel::turnChanged, this, [=](int player) {
        p1Label->setStyleSheet(player == 0 ? "color: #3c78dc; font-weight: bold; font-size: 14px;"
                                            : "color: #555; font-size: 14px;");
        p2Label->setStyleSheet(player == 1 ? "color: #dc3c3c; font-weight: bold; font-size: 14px;"
                                            : "color: #555; font-size: 14px;");
    });
    connect(m_vm, &GameViewModel::roundChanged, this, [=](int r) {
        roundLabel->setText(QString("Round %1").arg(r));
    });
    connect(m_vm, &GameViewModel::pointsChanged, this, [=](int pts) {
        pointsLabel->setText(QString("Points: %1").arg(pts));
    });

    // Start page
    auto *startPage = new QWidget(this);
    startPage->setStyleSheet("background: #141420;");
    auto *startLayout = new QVBoxLayout(startPage);
    startLayout->setAlignment(Qt::AlignCenter);

    auto *titleLabel = new QLabel("GRAPH WAR", this);
    titleLabel->setStyleSheet(
        "color: #4af; font-size: 64px; font-weight: bold;"
        "padding: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto *subtitleLabel = new QLabel("Two players, one curve. Fire functions at each other!", this);
    subtitleLabel->setStyleSheet("color: #aaa; font-size: 16px; padding-bottom: 40px;");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    auto *startBtn = new QPushButton("START GAME", this);
    startBtn->setStyleSheet(
        "QPushButton { background: #2a7; color: white; font-size: 22px; font-weight: bold;"
        "padding: 15px 60px; border-radius: 8px; }"
        "QPushButton:hover { background: #3a9; }");
    startBtn->setCursor(Qt::PointingHandCursor);

    startLayout->addStretch();
    startLayout->addWidget(titleLabel);
    startLayout->addWidget(subtitleLabel);
    startLayout->addWidget(startBtn, 0, Qt::AlignCenter);
    startLayout->addStretch();

    // Stack
    m_stack = new QStackedWidget(this);
    m_stack->addWidget(startPage);
    m_stack->addWidget(gamePage);
    m_stack->setCurrentIndex(0);

    setCentralWidget(m_stack);

    connect(startBtn, &QPushButton::clicked, this, &MainWindow::startGame);
    connect(m_vm, &GameViewModel::phaseChanged, this, [this](GamePhase phase) {
        if (phase == GamePhase::GameOver && !m_playAgainBtn) {
            m_playAgainBtn = new QPushButton("Play Again", this);
            m_playAgainBtn->setStyleSheet(
                "QPushButton { background: #2a7; color: white; font-size: 18px;"
                "padding: 12px 40px; border-radius: 6px; }");
            connect(m_playAgainBtn, &QPushButton::clicked, this, [this]() {
                m_playAgainBtn->parentWidget()->layout()->removeWidget(m_playAgainBtn);
                m_playAgainBtn->deleteLater();
                m_playAgainBtn = nullptr;
                m_vm->newGame();
            });
            m_input->layout()->addWidget(m_playAgainBtn);
        }
    });
}

void MainWindow::startGame() {
    m_vm->newGame();
    m_stack->setCurrentIndex(1);
}
