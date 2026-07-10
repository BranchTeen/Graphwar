#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QKeyEvent>
#include <QPushButton>
#include <QDialog>
#include <QFont>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Graphwar");
    resize(900, 700);
    setStyleSheet("background:#0f0f1a;color:#ddd;");

    m_vm = new GameViewModel(this);

    // Game page
    m_canvas = new GameCanvas(m_vm, this);
    m_input  = new FunctionInput(m_vm, this);

    auto *gamePage = new QWidget(this);
    auto *gameLayout = new QVBoxLayout(gamePage);
    gameLayout->setContentsMargins(0, 0, 0, 0);
    gameLayout->setSpacing(0);

    // Top bar
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

    connect(m_vm, &GameViewModel::turnChanged, this, [this](int player) {
        updateTopBarColors();
    });
    connect(m_vm, &GameViewModel::roundChanged, this, [=](int r) {
        roundLabel->setText(QString("Round %1").arg(r));
    });
    connect(m_vm, &GameViewModel::pointsChanged, this, [=](int pts) {
        pointsLabel->setText(QString("Points: %1").arg(pts));
    });
    connect(m_vm, &GameViewModel::trajectoryUpdated, m_canvas, QOverload<>::of(&QWidget::update));
    connect(m_vm, &GameViewModel::phaseChanged,    m_canvas, QOverload<>::of(&QWidget::update));
    connect(m_vm, &GameViewModel::turnChanged,     m_canvas, QOverload<>::of(&QWidget::update));
    connect(m_vm, &GameViewModel::gameOver,        m_canvas, QOverload<>::of(&QWidget::update));

    // Start page
    auto *startPage = new QWidget(this);
    startPage->setStyleSheet("background:#141420;");
    auto *startLayout = new QVBoxLayout(startPage);
    startLayout->setAlignment(Qt::AlignCenter);
    startLayout->setSpacing(16);

    auto *titleLabel = new QLabel("GRAPH WAR", this);
    titleLabel->setStyleSheet("color:#4af;font-size:64px;font-weight:bold;padding:20px;");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto *subtitleLabel = new QLabel("Two players, one curve. Fire functions at each other!", this);
    subtitleLabel->setStyleSheet("color:#aaa;font-size:16px;padding-bottom:20px;");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    auto *newGameBtn = new QPushButton("NEW GAME", this);
    newGameBtn->setStyleSheet(
        "QPushButton { background: #2a7; color: white; font-size: 22px; font-weight: bold;"
        "padding: 15px 60px; border-radius: 8px; }"
        "QPushButton:hover { background: #3c9; }");
    newGameBtn->setCursor(Qt::PointingHandCursor);

    auto *loadGameBtn = new QPushButton("Load / Manage Saves", this);
    loadGameBtn->setStyleSheet(
        "QPushButton { background: #384; color: white; font-size: 18px;"
        "padding: 12px 50px; border-radius: 8px; }"
        "QPushButton:hover { background: #4a5; }");
    loadGameBtn->setCursor(Qt::PointingHandCursor);

    startLayout->addStretch();
    startLayout->addWidget(titleLabel);
    startLayout->addWidget(subtitleLabel);
    startLayout->addWidget(newGameBtn, 0, Qt::AlignCenter);
    startLayout->addWidget(loadGameBtn, 0, Qt::AlignCenter);
    startLayout->addStretch();

    // NEW GAME 不再直接进入游戏，而是先进入 Config 页
    // 用户在 Config 页点击"START GAME"后再实际开始游戏
    connect(newGameBtn, &QPushButton::clicked, this, &MainWindow::goToConfig);
    connect(loadGameBtn, &QPushButton::clicked, this, &MainWindow::goToSaveManager);

    // Save manager page
    m_savePage = new SaveManagerPage(m_vm, this);
    connect(m_savePage, &SaveManagerPage::backToStart, this, &MainWindow::backToStart);
    connect(m_savePage, &SaveManagerPage::gameLoaded, this, &MainWindow::onGameLoaded);

    // Config page
    m_configPage = new ConfigPage(this);
    connect(m_configPage, &ConfigPage::backToStart, this, &MainWindow::backToStart);
    // "Save" 按钮现在的语义是"进入游戏"：先把配置应用到 VM，再启动新游戏
    connect(m_configPage, &ConfigPage::configSaved, this, [this](const GameConfig &cfg) {
        m_vm->setConfig(cfg);
        startNewGame();
    });

    // Pause page
    m_pausePage = new PauseMenuPage(m_vm, this);
    connect(m_pausePage, &PauseMenuPage::resumeGame, this, &MainWindow::resumeFromPause);
    connect(m_pausePage, &PauseMenuPage::backToStart, this, &MainWindow::backToStart);

    
    connect(m_input, &FunctionInput::pauseClicked, this, &MainWindow::goToPause);

    // 游戏结束 → 弹出"重新开始"对话框（这条 connect 必须在 MainWindow 构造时绑定，否则 gameOver 信号无响应）
    connect(m_vm, &GameViewModel::gameOver, this, &MainWindow::onGameOver);

    // Stack
    m_stack = new QStackedWidget(this);
    m_stack->addWidget(startPage);     // 0: StartPage
    m_stack->addWidget(gamePage);      // 1: GamePage
    m_stack->addWidget(m_savePage);    // 2: SaveManagerPage
    m_stack->addWidget(m_pausePage);   // 3: PausePage
    m_stack->addWidget(m_configPage);  // 4: ConfigPage
    m_stack->setCurrentIndex(PageStart);
    setCentralWidget(m_stack);
}

void MainWindow::showPage(PageIndex p) {
    m_stack->setCurrentIndex(p);
}

void MainWindow::updateTopBarColors() {
    const auto &cfg = m_vm->config();
    auto makeStyle = [](const QColor &c) {
        return QString("color:rgba(%1,%2,%3,%4);font-weight:bold;font-size:14px;")
            .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
    };
    auto dimStyle = QString("color:#555;font-size:14px;");
    int cur = m_vm->currentPlayer();
    m_p1Label->setStyleSheet(cur == 0 ? makeStyle(cfg.player1Color) : dimStyle);
    m_p2Label->setStyleSheet(cur == 1 ? makeStyle(cfg.player2Color) : dimStyle);
}

void MainWindow::startNewGame() {
    m_vm->newGame();
    updateTopBarColors();
    showPage(PageGame);
}

void MainWindow::goToConfig() {
    // 每次进入 Config 页面时，以当前 vm 中的 GameConfig 回显表单
    // —— 这样用户可以在存档加载后看到保存时的设置（含 Show grid lines 等）
    if (m_vm && m_configPage) m_configPage->refresh(m_vm->config());
    showPage(PageConfig);
}

void MainWindow::goToSaveManager() {
    m_savePage->refresh();
    showPage(PageSaveMgr);
}

void MainWindow::goToPause() {
    m_vm->pause();
    m_pausePage->refresh();
    showPage(PagePause);
}

void MainWindow::resumeFromPause() {
    m_vm->resume();
    showPage(PageGame);
}

void MainWindow::backToStart() {
    m_vm->resume();   
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
        m_vm->newGame();
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
