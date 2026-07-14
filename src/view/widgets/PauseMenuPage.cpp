#include "PauseMenuPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QFont>

PauseMenuPage::PauseMenuPage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(40, 30, 40, 30);
    root->setSpacing(16);

    auto *title = new QLabel("PAUSED", this);
    title->setStyleSheet(
        "color: white; font-size: 36px; font-weight: bold;"
        "padding: 16px 32px; letter-spacing: 2px;"
        "background: rgba(0, 0, 0, 140); border-radius: 16px;"
    );
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    m_continueBtn = new QPushButton("RESUME (ESC)", this);
    m_continueBtn->setCursor(Qt::PointingHandCursor);
    m_continueBtn->setStyleSheet(
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
    connect(m_continueBtn, &QPushButton::clicked, this, &PauseMenuPage::onContinueClicked);
    root->addWidget(m_continueBtn);

    buildSaveSlots();

    m_backBtn = new QPushButton("BACK TO TITLE", this);
    m_backBtn->setCursor(Qt::PointingHandCursor);
    m_backBtn->setStyleSheet(
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
    connect(m_backBtn, &QPushButton::clicked, this, &PauseMenuPage::onBackClicked);
    root->addWidget(m_backBtn, 0, Qt::AlignCenter);
}

PauseMenuPage::~PauseMenuPage() noexcept {}

void PauseMenuPage::buildSaveSlots() {
    m_slotsWidget = new QWidget(this);
    auto *slotsLayout = new QVBoxLayout(m_slotsWidget);
    slotsLayout->setContentsMargins(20, 20, 20, 20);
    slotsLayout->setSpacing(16);
    m_slotsWidget->setStyleSheet("background: rgba(0, 0, 0, 120); border-radius: 12px;");

    int total = m_state ? m_state->slotCount : 3;
    auto infos = m_state ? m_state->slotInfos : QVector<SaveInfo>();

    for (int slot = 0; slot < total; ++slot) {
        SaveInfo info;
        if (slot < infos.size()) info = infos[slot];

        auto *row = new QWidget(m_slotsWidget);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(16, 12, 16, 12);
        rowLayout->setSpacing(20);
        row->setStyleSheet("background: rgba(40, 40, 60, 100); border-radius: 8px;");

        auto *slotLabel = new QLabel(QString("SLOT %1").arg(slot + 1), row);
        slotLabel->setStyleSheet(
            "color: white; font-size: 16px; font-weight: bold;"
            "background: rgba(0, 0, 0, 100); padding: 4px 12px; border-radius: 6px;"
        );
        slotLabel->setMinimumWidth(80);
        rowLayout->addWidget(slotLabel);

        auto *infoLabel = new QLabel(row);
        if (info.exists) {
            infoLabel->setText(QString("[EXISTING] ROUND %1  |  PLAYER %2  |  %3")
                .arg(info.roundNumber).arg(info.currentPlayer + 1).arg(info.displayTime()));
            infoLabel->setStyleSheet("color: #ff8888; font-size: 14px;");
        } else {
            infoLabel->setText("(EMPTY)");
            infoLabel->setStyleSheet("color: #666; font-size: 14px; font-style: italic;");
        }
        rowLayout->addWidget(infoLabel, 1);

        auto *saveBtn = new QPushButton(info.exists ? "SAVE (OVERWRITE)" : "SAVE", row);
        saveBtn->setCursor(Qt::PointingHandCursor);
        saveBtn->setStyleSheet(
            "QPushButton {"
            "min-width: 120px; min-height: 40px; border-radius: 8px;"
            "border: 2px solid rgba(255, 255, 255, 180); font-weight: bold;"
            "color: white; font-size: 14px;"
            "background: rgba(40, 80, 100, 150);"
            "}"
            "QPushButton:hover {"
            "background: rgba(70, 130, 160, 200); border: 2px solid white;"
            "}"
            "QPushButton:pressed {"
            "background: rgba(50, 100, 140, 220);"
            "}"
        );
        connect(saveBtn, &QPushButton::clicked, this, [this, slot](){ onSaveClicked(slot); });
        m_saveSlots.append(saveBtn);
        rowLayout->addWidget(saveBtn);

        slotsLayout->addWidget(row);
    }

    static_cast<QVBoxLayout*>(layout())->insertWidget(2, m_slotsWidget, 1);
}

void PauseMenuPage::refreshSlots() {
    m_saveSlots.clear();
    if (m_slotsWidget) {
        static_cast<QVBoxLayout*>(layout())->removeWidget(m_slotsWidget);
        m_slotsWidget->deleteLater();
        m_slotsWidget = nullptr;
        buildSaveSlots();
    }
}

void PauseMenuPage::onContinueClicked() {
    if (m_resumeCmd) m_resumeCmd();
}

void PauseMenuPage::onBackClicked() {
    emit backToTitle();
}

void PauseMenuPage::onSaveClicked(int slot) {
    SaveInfo info;
    auto infos = m_state ? m_state->slotInfos : QVector<SaveInfo>();
    if (slot >= 0 && slot < infos.size()) info = infos[slot];

    if (info.exists) {
        auto ret = QMessageBox::question(this, "Overwrite?",
            QString("Slot %1 already exists. Overwrite?").arg(slot + 1),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret != QMessageBox::Yes) return;
    }

    if (m_saveSlotCmd) m_saveSlotCmd(slot);
    QMessageBox::information(this, "Saved", QString("Saved to slot %1.").arg(slot + 1));
}