#include "ConfigPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QFont>
#include <QMessageBox>

const QVector<QColor> ConfigPage::s_colors{
    {60, 120, 220, 200},
    {220, 60, 60, 200},
    {60, 200, 100, 200},
    {240, 150, 40, 200},
    {180, 80, 220, 200},
    {40, 200, 220, 200},
    {220, 200, 40, 200},
    {240, 100, 160, 200},
};

ConfigPage::ConfigPage(QWidget *parent) : QWidget(parent) {
    setStyleSheet("background:#141420;");
    build();
}

int ConfigPage::matchColorIndex(const QColor &c) {
    int bestIdx = 0;
    int bestDist = INT_MAX;
    for (int i = 0; i < s_colors.size(); ++i) {
        int dr = s_colors[i].red()   - c.red();
        int dg = s_colors[i].green() - c.green();
        int db = s_colors[i].blue()  - c.blue();
        int dist = dr*dr + dg*dg + db*db;
        if (dist < bestDist) { bestDist = dist; bestIdx = i; }
    }
    return bestIdx;
}

void ConfigPage::refresh(const GameConfig &cfg) {
    if (m_squaresSpin)      m_squaresSpin->setValue(cfg.squaresPerPlayer);
    if (m_obstacleCountSpin) m_obstacleCountSpin->setValue(cfg.obstacleCount);
    if (m_obstacleSizeSpin) m_obstacleSizeSpin->setValue(cfg.obstacleSize);
    if (m_coordCheck)       m_coordCheck->setChecked(cfg.showCoordinates);
    if (m_gridCheck)        m_gridCheck->setChecked(cfg.showGridLines);

    m_p1Index = matchColorIndex(cfg.player1Color);
    m_p2Index = matchColorIndex(cfg.player2Color);
    if (m_p1Index == m_p2Index) {
        m_p2Index = (m_p1Index + 1) % s_colors.size();
    }

    for (int i = 0; i < m_p1Swatches.size(); ++i) {
        const QColor &c = s_colors[i];
        QString border = (i == m_p1Index)
            ? "border:2px solid white;border-radius:4px;"
            : "border:1px solid #666;border-radius:4px;";
        m_p1Swatches[i]->setStyleSheet(
            QString("background:rgba(%1,%2,%3,%4);%5")
                .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha()).arg(border));
    }
    for (int i = 0; i < m_p2Swatches.size(); ++i) {
        const QColor &c = s_colors[i];
        QString border = (i == m_p2Index)
            ? "border:2px solid white;border-radius:4px;"
            : "border:1px solid #666;border-radius:4px;";
        m_p2Swatches[i]->setStyleSheet(
            QString("background:rgba(%1,%2,%3,%4);%5")
                .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha()).arg(border));
    }
}

void ConfigPage::build() {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(60, 40, 60, 40);
    root->setSpacing(20);

    auto *title = new QLabel("Game Settings", this);
    title->setStyleSheet("color:#4af;font-size:32px;font-weight:bold;");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    auto *form = new QWidget(this);
    form->setStyleSheet("background:#1a1a2e;border-radius:8px;padding:20px;");
    auto *formLayout = new QFormLayout(form);
    formLayout->setSpacing(16);
    formLayout->setContentsMargins(30, 20, 30, 20);
    formLayout->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto spinStyle = "QSpinBox{background:#0f0f1a;color:#fff;font-size:14px;padding:4px 8px;border:1px solid #335;border-radius:4px;}";
    auto dspinStyle = "QDoubleSpinBox{background:#0f0f1a;color:#fff;font-size:14px;padding:4px 8px;border:1px solid #335;border-radius:4px;}";
    auto sepStyle = "QCheckBox{color:#ddd;font-size:14px;spacing:8px;}"
                    "QCheckBox::indicator{width:18px;height:18px;border-radius:3px;border:1px solid #558;background:#0f0f1a;}"
                    "QCheckBox::indicator:checked{background:#2a7;border:1px solid #3c9;}";

    auto addField = [&](const QString &label, QWidget *field) {
        int r = formLayout->rowCount();
        formLayout->addRow(label, field);
        if (auto *item = formLayout->itemAt(r, QFormLayout::FieldRole))
            item->setAlignment(Qt::AlignVCenter);
    };

    m_squaresSpin = new QSpinBox(this);
    m_squaresSpin->setRange(1, 10);
    m_squaresSpin->setValue(5);
    m_squaresSpin->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    m_squaresSpin->setStyleSheet(spinStyle);
    addField("Squares per player (1-10):", m_squaresSpin);

    m_obstacleCountSpin = new QSpinBox(this);
    m_obstacleCountSpin->setRange(0, 30);
    m_obstacleCountSpin->setValue(10);
    m_obstacleCountSpin->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    m_obstacleCountSpin->setStyleSheet(spinStyle);
    addField("Obstacles (0-30):", m_obstacleCountSpin);

    m_obstacleSizeSpin = new QDoubleSpinBox(this);
    m_obstacleSizeSpin->setRange(0.5, 5.0);
    m_obstacleSizeSpin->setSingleStep(0.1);
    m_obstacleSizeSpin->setValue(1.8);
    m_obstacleSizeSpin->setButtonSymbols(QAbstractSpinBox::PlusMinus);
    m_obstacleSizeSpin->setStyleSheet(dspinStyle);
    addField("Obstacle size (0.5-5.0):", m_obstacleSizeSpin);

    m_coordCheck = new QCheckBox("Show square coordinates", this);
    m_coordCheck->setChecked(true);
    m_coordCheck->setStyleSheet(sepStyle);
    formLayout->addRow("", m_coordCheck);

    m_gridCheck = new QCheckBox("Show grid lines", this);
    m_gridCheck->setChecked(false);
    m_gridCheck->setStyleSheet(sepStyle);
    formLayout->addRow("", m_gridCheck);

    formLayout->addRow("P1 Color:", createSwatchRow(0));
    formLayout->addRow("P2 Color:", createSwatchRow(1));

    root->addWidget(form);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(20);

    auto *backBtn = new QPushButton("← Back", this);
    backBtn->setStyleSheet(
        "QPushButton{background:#334;color:#ddd;padding:12px 40px;border-radius:6px;font-size:16px;}"
        "QPushButton:hover{background:#556;}");
    connect(backBtn, &QPushButton::clicked, this, &ConfigPage::backToStart);
    btnLayout->addWidget(backBtn);

    auto *startBtn = new QPushButton("START GAME", this);
    startBtn->setStyleSheet(
        "QPushButton{background:#2a7;color:white;font-size:22px;font-weight:bold;padding:14px 50px;border-radius:8px;}"
        "QPushButton:hover{background:#3c9;}");
    connect(startBtn, &QPushButton::clicked, this, [this]() {
        if (m_p1Index == m_p2Index) {
            QMessageBox::warning(this, "Same Color", "Players cannot share the same color. Please pick different colors.");
            return;
        }
        GameConfig cfg;
        cfg.squaresPerPlayer = m_squaresSpin->value();
        cfg.obstacleCount = m_obstacleCountSpin->value();
        cfg.obstacleSize = m_obstacleSizeSpin->value();
        cfg.player1Color = s_colors[m_p1Index];
        cfg.player2Color = s_colors[m_p2Index];
        cfg.showCoordinates = m_coordCheck->isChecked();
        cfg.showGridLines = m_gridCheck->isChecked();
        emit configSaved(cfg);
    });
    btnLayout->addWidget(startBtn);

    root->addLayout(btnLayout);
}

QWidget *ConfigPage::createSwatchRow(int player) {
    auto *row = new QWidget(this);
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    auto &swatches = (player == 0) ? m_p1Swatches : m_p2Swatches;

    for (int i = 0; i < s_colors.size(); ++i) {
        const QColor &c = s_colors[i];
        auto *btn = new QPushButton(row);
        btn->setFixedSize(32, 32);
        QString bg = QString("background:rgba(%1,%2,%3,%4);")
            .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha());
        QString border = (player == 0 && i == m_p1Index) || (player == 1 && i == m_p2Index)
            ? "border:2px solid white;border-radius:4px;"
            : "border:1px solid #666;border-radius:4px;";
        btn->setStyleSheet(bg + border);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [this, player, i]() {
            selectSwatch(player, i);
        });
        layout->addWidget(btn);
        swatches.append(btn);
    }
    layout->addStretch();
    return row;
}

void ConfigPage::selectSwatch(int player, int index) {
    if (player == 0 && index == m_p1Index) return;
    if (player == 1 && index == m_p2Index) return;

    int otherIdx = (player == 0) ? m_p2Index : m_p1Index;
    if (index == otherIdx) {
        QMessageBox::information(this, "Color Taken",
            QString("Player %1 already has that color. Choose a different one.").arg(player == 0 ? 2 : 1));
        return;
    }

    auto &swatches = (player == 0) ? m_p1Swatches : m_p2Swatches;
    int &curIdx = (player == 0) ? m_p1Index : m_p2Index;

    if (curIdx >= 0 && curIdx < swatches.size()) {
        const QColor &c = s_colors[curIdx];
        swatches[curIdx]->setStyleSheet(
            QString("background:rgba(%1,%2,%3,%4);border:1px solid #666;border-radius:4px;")
            .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha()));
    }

    curIdx = index;

    const QColor &c = s_colors[index];
    swatches[index]->setStyleSheet(
        QString("background:rgba(%1,%2,%3,%4);border:2px solid white;border-radius:4px;")
        .arg(c.red()).arg(c.green()).arg(c.blue()).arg(c.alpha()));
}
