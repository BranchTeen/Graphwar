#include "SaveManagerPage.h"
#include "viewmodel/SaveManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

SaveManagerPage::SaveManagerPage(GameViewModel *vm, QWidget *parent)
    : QWidget(parent), m_vm(vm) {

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(40, 30, 40, 30);
    root->setSpacing(20);

    auto *title = new QLabel("Load / Manage Saves", this);
    title->setStyleSheet("color:#4af;font-size:28px;font-weight:bold;");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    m_slotsContainer = new QWidget(this);
    m_slotsContainer->setStyleSheet("background:#1a1a2a;border-radius:8px;");
    root->addWidget(m_slotsContainer, 1);

    auto *backBtn = new QPushButton("← Back to Start", this);
    backBtn->setStyleSheet(
        "QPushButton{background:#334;color:#ddd;padding:10px 24px;border-radius:6px;font-size:14px;}"
        "QPushButton:hover{background:#446;}");
    connect(backBtn, &QPushButton::clicked, this, &SaveManagerPage::backToStart);
    root->addWidget(backBtn, 0, Qt::AlignCenter);

    rebuild();
}

void SaveManagerPage::refresh() {
    rebuild();
}

void SaveManagerPage::rebuild() {

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
    layout->setSpacing(12);

    for (int slot = 0; slot < SaveManager::kSlotCount; ++slot) {
        SaveInfo info = SaveManager::slotInfo(slot);

        auto *row = new QWidget(m_slotsContainer);
        auto *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(12, 8, 12, 8);
        rowLayout->setSpacing(16);

        auto *slotLabel = new QLabel(QString("Slot %1").arg(slot + 1), row);
        slotLabel->setStyleSheet("color:#ffcc00;font-size:16px;font-weight:bold;");
        slotLabel->setMinimumWidth(80);
        rowLayout->addWidget(slotLabel);

        auto *infoLabel = new QLabel(row);
        infoLabel->setStyleSheet("color:#ddd;font-size:13px;");
        if (info.exists) {
            infoLabel->setText(QString("Round %1  |  Player %2's turn  |  Saved at %3")
                .arg(info.roundNumber).arg(info.currentPlayer + 1).arg(info.displayTime()));
        } else {
            infoLabel->setText("(Empty)");
            infoLabel->setStyleSheet("color:#666;font-size:13px;font-style:italic;");
        }
        rowLayout->addWidget(infoLabel, 1);

        auto *loadBtn = new QPushButton("Load", row);
        loadBtn->setEnabled(info.exists);
        loadBtn->setStyleSheet(
            "QPushButton{background:#2a7;color:white;padding:8px 18px;border-radius:4px;font-size:13px;}"
            "QPushButton:hover:!disabled{background:#3c9;}"
            "QPushButton:disabled{background:#333;color:#666;}");
        connect(loadBtn, &QPushButton::clicked, this, [this, slot](){ loadSlot(slot); });
        rowLayout->addWidget(loadBtn);

        auto *delBtn = new QPushButton("Delete", row);
        delBtn->setEnabled(info.exists);
        delBtn->setStyleSheet(
            "QPushButton{background:#733;color:white;padding:8px 18px;border-radius:4px;font-size:13px;}"
            "QPushButton:hover:!disabled{background:#a44;}"
            "QPushButton:disabled{background:#333;color:#666;}");
        connect(delBtn, &QPushButton::clicked, this, [this, slot](){ deleteSlot(slot); });
        rowLayout->addWidget(delBtn);

        layout->addWidget(row);
    }
}

void SaveManagerPage::loadSlot(int slot) {
    if (!m_vm) return;
    bool ok = m_vm->loadFromSlot(slot);
    if (!ok) {
        QMessageBox::warning(this, "Load failed",
            QString("Could not load slot %1.\nFile path:\n%2")
                .arg(slot + 1).arg(SaveManager::slotPath(slot)));
        return;
    }
    emit gameLoaded();
}

void SaveManagerPage::deleteSlot(int slot) {
    if (!m_vm) return;
    auto ret = QMessageBox::question(this, "Confirm delete",
        QString("Delete slot %1?").arg(slot + 1),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) return;
    m_vm->deleteSlot(slot);
    rebuild();
}
