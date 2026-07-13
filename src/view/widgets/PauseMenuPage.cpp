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

    auto *title = new QLabel("Paused", this);
    title->setStyleSheet("color:#4af;font-size:28px;font-weight:bold;");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    m_continueBtn = new QPushButton("Resume (Esc)", this);
    m_continueBtn->setStyleSheet(
        "QPushButton{background:#2a7;color:white;padding:12px 30px;border-radius:6px;font-size:16px;}"
        "QPushButton:hover{background:#3c9;}");
    connect(m_continueBtn, &QPushButton::clicked, this, &PauseMenuPage::onContinueClicked);
    root->addWidget(m_continueBtn);

    buildSaveSlots();

    m_backBtn = new QPushButton("Back to title", this);
    m_backBtn->setStyleSheet(
        "QPushButton{background:#533;color:white;padding:10px 24px;border-radius:6px;font-size:14px;}"
        "QPushButton:hover{background:#744;}");
    connect(m_backBtn, &QPushButton::clicked, this, &PauseMenuPage::onBackClicked);
    root->addWidget(m_backBtn, 0, Qt::AlignCenter);
}

PauseMenuPage::~PauseMenuPage() noexcept {}

void PauseMenuPage::buildSaveSlots() {
    m_slotsWidget = new QWidget(this);
    auto *slotsLayout = new QVBoxLayout(m_slotsWidget);
    slotsLayout->setContentsMargins(20, 20, 20, 20);
    slotsLayout->setSpacing(10);
    m_slotsWidget->setStyleSheet("background:#1a1a2a;border-radius:8px;");

    int total = m_state ? m_state->slotCount : 3;
    auto infos = m_state ? m_state->slotInfos : QVector<SaveInfo>();

    for (int slot = 0; slot < total; ++slot) {
        SaveInfo info;
        if (slot < infos.size()) info = infos[slot];

        auto *row = new QWidget(m_slotsWidget);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(12, 6, 12, 6);
        rowLayout->setSpacing(16);

        auto *slotLabel = new QLabel(QString("Slot %1").arg(slot + 1), row);
        slotLabel->setStyleSheet("color:#ffcc00;font-size:15px;font-weight:bold;");
        slotLabel->setMinimumWidth(80);
        rowLayout->addWidget(slotLabel);

        auto *infoLabel = new QLabel(row);
        if (info.exists) {
            infoLabel->setText(QString("[Existing] Round %1  |  Player %2  |  %3")
                .arg(info.roundNumber).arg(info.currentPlayer + 1).arg(info.displayTime()));
            infoLabel->setStyleSheet("color:#f88;font-size:12px;");
        } else {
            infoLabel->setText("(Empty)");
            infoLabel->setStyleSheet("color:#888;font-size:12px;font-style:italic;");
        }
        rowLayout->addWidget(infoLabel, 1);

        auto *saveBtn = new QPushButton(info.exists ? "Save (overwrite)" : "Save", row);
        saveBtn->setStyleSheet(
            "QPushButton{background:#2a7;color:white;padding:8px 22px;border-radius:4px;font-size:13px;}"
            "QPushButton:hover{background:#3c9;}");
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