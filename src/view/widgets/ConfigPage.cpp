#include "ConfigPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFont>
#include <QMessageBox>

const QVector<QColor> ConfigPage::kPresetColors = {
    QColor(60, 120, 220), QColor(220, 60, 60), QColor(60, 180, 80),
    QColor(220, 140, 40), QColor(160, 80, 200), QColor(40, 180, 200),
    QColor(200, 190, 50), QColor(220, 100, 160)
};

ConfigPage::ConfigPage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(40, 30, 40, 30);
    root->setSpacing(16);

    auto *title = new QLabel("Game Settings", this);
    title->setStyleSheet("color:#4af;font-size:28px;font-weight:bold;");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    auto *form = new QFormLayout();
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight);

    m_squareCount = new QSpinBox(this);
    m_squareCount->setRange(1, 10);
    m_squareCount->setValue(5);
    m_squareCount->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    form->addRow("Squares per player (1-10):", m_squareCount);

    m_obstacleCount = new QSpinBox(this);
    m_obstacleCount->setRange(0, 30);
    m_obstacleCount->setValue(10);
    m_obstacleCount->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    form->addRow("Obstacles (0-30):", m_obstacleCount);

    m_obstacleSize = new QDoubleSpinBox(this);
    m_obstacleSize->setRange(0.5, 5.0);
    m_obstacleSize->setSingleStep(0.1);
    m_obstacleSize->setValue(1.8);
    m_obstacleSize->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    form->addRow("Obstacle size (0.5-5.0):", m_obstacleSize);

    m_showCoords = new QCheckBox("Show square coordinates", this);
    m_showCoords->setChecked(true);
    form->addRow(m_showCoords);

    m_showGrid = new QCheckBox("Show grid lines", this);
    m_showGrid->setChecked(false);
    form->addRow(m_showGrid);

    root->addLayout(form);

    root->addWidget(new QLabel("P1 Color:", this));
    root->addWidget(createColorPalette(0));
    root->addWidget(new QLabel("P2 Color:", this));
    root->addWidget(createColorPalette(1));

    auto *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    auto *backBtn = new QPushButton("← Back", this);
    backBtn->setStyleSheet(
        "QPushButton{background:#556;color:#ddd;padding:10px 24px;border-radius:6px;font-size:14px;}"
        "QPushButton:hover{background:#779;}");
    connect(backBtn, &QPushButton::clicked, this, &ConfigPage::onBackClicked);
    btnLayout->addWidget(backBtn);

    auto *startBtn = new QPushButton("START GAME", this);
    startBtn->setStyleSheet(
        "QPushButton{background:#2a7;color:white;padding:12px 30px;border-radius:6px;font-size:16px;font-weight:bold;}"
        "QPushButton:hover{background:#3c9;}");
    connect(startBtn, &QPushButton::clicked, this, &ConfigPage::onStartClicked);
    btnLayout->addWidget(startBtn);

    root->addLayout(btnLayout);
}

ConfigPage::~ConfigPage() noexcept {}

QWidget* ConfigPage::createColorPalette(int player) {
    auto *w = new QWidget(this);
    auto *layout = new QHBoxLayout(w);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    auto &swatches = (player == 0) ? m_p1Swatches : m_p2Swatches;
    for (int i = 0; i < kPresetColors.size(); ++i) {
        auto *lbl = new QLabel(w);
        lbl->setFixedSize(30, 24);
        QColor c = kPresetColors[i];
        lbl->setStyleSheet(QString("background:rgba(%1,%2,%3,200);border-radius:4px;")
            .arg(c.red()).arg(c.green()).arg(c.blue()));
        lbl->setCursor(Qt::PointingHandCursor);
        swatches.append(lbl);
        connect(lbl, &QLabel::linkActivated, this, [this, player, i](){});
        lbl->installEventFilter(this);
        // Use mouse press via event filter
        lbl->setProperty("player", player);
        lbl->setProperty("index", i);
        lbl->setAlignment(Qt::AlignCenter);
        // Make clickable via a transparent button overlay or mouse tracking
        // Simpler: use a QPushButton styled to look like a label
        auto *btn = new QPushButton(w);
        btn->setFixedSize(30, 24);
        btn->setStyleSheet(QString(
            "QPushButton{background:rgba(%1,%2,%3,200);border-radius:4px;border:2px solid transparent;}"
            "QPushButton:hover{border:2px solid white;}")
            .arg(c.red()).arg(c.green()).arg(c.blue()));
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [this, player, i]() { onColorClicked(player, i); });
        layout->addWidget(btn);
    }
    return w;
}

void ConfigPage::refresh(const GameConfig &cfg) {
    m_squareCount->setValue(cfg.squaresPerPlayer);
    m_obstacleCount->setValue(cfg.obstacleCount);
    m_obstacleSize->setValue(cfg.obstacleSize);
    m_showCoords->setChecked(cfg.showCoordinates);
    m_showGrid->setChecked(cfg.showGridLines);
    updateColorBorders();
}

void ConfigPage::onColorClicked(int player, int index) {
    if (player == 0) m_p1ColorIndex = index;
    else m_p2ColorIndex = index;
    updateColorBorders();
}

void ConfigPage::updateColorBorders() {
    for (int i = 0; i < kPresetColors.size(); ++i) {
        QColor c = kPresetColors[i];
        QString base = QString("background:rgba(%1,%2,%3,200);border-radius:4px;").arg(c.red()).arg(c.green()).arg(c.blue());
        m_p1Swatches[i]->setStyleSheet(base + (i == m_p1ColorIndex ? "border:2px solid white;" : "border:2px solid transparent;"));
        m_p2Swatches[i]->setStyleSheet(base + (i == m_p2ColorIndex ? "border:2px solid white;" : "border:2px solid transparent;"));
    }
}

void ConfigPage::onStartClicked() {
    if (m_p1ColorIndex == m_p2ColorIndex) {
        QMessageBox::warning(this, "Same color", "Player 1 and Player 2 cannot choose the same color!");
        return;
    }
    GameConfig cfg;
    cfg.squaresPerPlayer = m_squareCount->value();
    cfg.obstacleCount = m_obstacleCount->value();
    cfg.obstacleSize = m_obstacleSize->value();
    cfg.showCoordinates = m_showCoords->isChecked();
    cfg.showGridLines = m_showGrid->isChecked();
    cfg.player1Color = kPresetColors[m_p1ColorIndex];
    cfg.player2Color = kPresetColors[m_p2ColorIndex];
    emit configSaved(cfg);
}

void ConfigPage::onBackClicked() { emit backToStart(); }
