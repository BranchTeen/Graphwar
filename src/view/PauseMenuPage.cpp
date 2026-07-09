#include "PauseMenuPage.h"
#include "viewmodel/SaveManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

PauseMenuPage::PauseMenuPage(GameViewModel *vm, QWidget *parent)
    : QWidget(parent), m_vm(vm) {

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(40, 30, 40, 30);
    root->setSpacing(16);

    auto *title = new QLabel("Paused", this);
    title->setStyleSheet("color:#4af;font-size:28px;font-weight:bold;");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    auto *resumeBtn = new QPushButton("Resume (Esc)", this);
    resumeBtn->setStyleSheet(
        "QPushButton{background:#2a7;color:white;padding:12px 30px;border-radius:6px;font-size:16px;}"
        "QPushButton:hover{background:#3c9;}");
    connect(resumeBtn, &QPushButton::clicked, this, &PauseMenuPage::resumeGame);
    root->addWidget(resumeBtn);

    auto *hint = new QLabel("Save current game into a slot (will overwrite if not empty):", this);
    hint->setStyleSheet("color:#aaa;font-size:13px;");
    root->addWidget(hint);

    m_slotsContainer = new QWidget(this);
    m_slotsContainer->setStyleSheet("background:#1a1a2a;border-radius:8px;");
    root->addWidget(m_slotsContainer, 1);

    auto *backBtn = new QPushButton("Back to title", this);
    backBtn->setStyleSheet(
        "QPushButton{background:#533;color:white;padding:10px 24px;border-radius:6px;font-size:14px;}"
        "QPushButton:hover{background:#744;}");
    connect(backBtn, &QPushButton::clicked, this, &PauseMenuPage::backToStart);
    root->addWidget(backBtn, 0, Qt::AlignCenter);

    rebuild();
}

void PauseMenuPage::refresh() {
    rebuild();
}

void PauseMenuPage::rebuild() {
    if (m_slotsContainer->layout()) {
        QLayoutItem *item;
        while ((item = m_slotsContainer->layout()->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete m_slotsContainer->layout();
    }

    auto *layout = new QVBoxLayout(m_slotsContainer);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(10);

    for (int slot = 0; slot < SaveManager::kSlotCount; ++slot) {
        SaveInfo info = SaveManager::slotInfo(slot);

        auto *row = new QWidget(m_slotsContainer);
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

        auto *saveBtn = new QPushButton(QString("Save %1").arg(info.exists ? "(overwrite)" : ""), row);
        saveBtn->setStyleSheet(
            "QPushButton{background:#2a7;color:white;padding:8px 22px;border-radius:4px;font-size:13px;}"
            "QPushButton:hover{background:#3c9;}");
        connect(saveBtn, &QPushButton::clicked, this, [this, slot](){ saveTo(slot); });
        rowLayout->addWidget(saveBtn);

        layout->addWidget(row);
    }
}

void PauseMenuPage::saveTo(int slot) {
    if (!m_vm) return;
    SaveInfo info = SaveManager::slotInfo(slot);
    if (info.exists) {
        auto ret = QMessageBox::question(this, "Overwrite?",
            QString("Slot %1 already exists. Overwrite?").arg(slot + 1),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret != QMessageBox::Yes) return;
    }
    bool ok = m_vm->saveToSlot(slot);
    if (!ok) {
        QMessageBox::warning(this, "Save failed", QString("Could not save to slot %1.").arg(slot + 1));
        return;
    }
    rebuild();
    QMessageBox::information(this, "Saved", QString("Saved to slot %1.\n%2")
        .arg(slot + 1).arg(SaveManager::slotPath(slot)));
}
