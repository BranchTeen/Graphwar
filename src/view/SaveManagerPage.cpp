#include "SaveManagerPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QFont>

SaveManagerPage::SaveManagerPage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(40, 30, 40, 30);
    root->setSpacing(20);

    auto *title = new QLabel("Load / Manage Saves", this);
    title->setStyleSheet("color:#4af;font-size:28px;font-weight:bold;");
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    buildSlots();

    auto *backBtn = new QPushButton("← Back to Start", this);
    backBtn->setStyleSheet(
        "QPushButton{background:#334;color:#ddd;padding:10px 24px;border-radius:6px;font-size:14px;}"
        "QPushButton:hover{background:#446;}");
    connect(backBtn, &QPushButton::clicked, this, &SaveManagerPage::onBackClicked);
    root->addWidget(backBtn, 0, Qt::AlignCenter);

    auto &bus = EventBus::instance();
    connect(&bus, &EventBus::evtSaveResult, this, &SaveManagerPage::onSaveResult);
}

void SaveManagerPage::buildSlots() {
    auto *slotsContainer = new QWidget(this);
    slotsContainer->setStyleSheet("background:#1a1a2a;border-radius:8px;");
    auto *layout = new QVBoxLayout(slotsContainer);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    auto &bus = EventBus::instance();
    int total = bus.slotCount();
    auto infos = bus.slotInfos();

    for (int slot = 0; slot < total; ++slot) {
        SaveInfo info;
        if (slot < infos.size()) info = infos[slot];

        auto *row = new QWidget(slotsContainer);
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
        connect(loadBtn, &QPushButton::clicked, this, [this, slot](){ onLoadClicked(slot); });
        rowLayout->addWidget(loadBtn);

        auto *delBtn = new QPushButton("Delete", row);
        delBtn->setEnabled(info.exists);
        delBtn->setStyleSheet(
            "QPushButton{background:#733;color:white;padding:8px 18px;border-radius:4px;font-size:13px;}"
            "QPushButton:hover:!disabled{background:#a44;}"
            "QPushButton:disabled{background:#333;color:#666;}");
        connect(delBtn, &QPushButton::clicked, this, [this, slot](){ onDeleteClicked(slot); });
        rowLayout->addWidget(delBtn);

        layout->addWidget(row);
    }

    static_cast<QVBoxLayout*>(this->layout())->insertWidget(1, slotsContainer, 1);
}

void SaveManagerPage::onLoadClicked(int slot) {
    emit EventBus::instance().cmdLoadFromSlot(slot);
}

void SaveManagerPage::onDeleteClicked(int slot) {
    auto ret = QMessageBox::question(this, "Confirm delete",
        QString("Delete slot %1?").arg(slot + 1),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) return;
    emit EventBus::instance().cmdDeleteSlot(slot);
}

void SaveManagerPage::onBackClicked() {
    emit backRequested();
}

void SaveManagerPage::onSaveResult(int slot, bool ok, const QString &info) {
    if (!ok) {
        QMessageBox::warning(this, "Load failed",
            QString("Could not load slot %1.\nFile path:\n%2")
                .arg(slot + 1).arg(EventBus::instance().slotPath(slot)));
        return;
    }
}
