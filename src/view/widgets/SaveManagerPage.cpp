#include "SaveManagerPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QFont>
#include <QShowEvent>

SaveManagerPage::SaveManagerPage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(40, 30, 40, 30);
    root->setSpacing(20);

    auto *title = new QLabel("LOAD / MANAGE SAVES", this);
    title->setStyleSheet(
        "color: white; font-size: 36px; font-weight: bold;"
        "padding: 16px 32px; letter-spacing: 2px;"
        "background: rgba(0, 0, 0, 140); border-radius: 16px;"
    );
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    buildSlots();

    auto *backBtn = new QPushButton("BACK TO START", this);
    backBtn->setCursor(Qt::PointingHandCursor);
    backBtn->setStyleSheet(
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
    connect(backBtn, &QPushButton::clicked, this, &SaveManagerPage::onBackClicked);
    root->addWidget(backBtn, 0, Qt::AlignCenter);
}

SaveManagerPage::~SaveManagerPage() noexcept {}

void SaveManagerPage::buildSlots() {
    m_slotsContainer = new QWidget(this);
    m_slotsContainer->setStyleSheet("background: rgba(0, 0, 0, 120); border-radius: 12px;");
    auto *layout = new QVBoxLayout(m_slotsContainer);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(16);

    int total = m_state ? m_state->slotCount : 3;
    auto infos = m_state ? m_state->slotInfos : QVector<SaveInfo>();

    for (int slot = 0; slot < total; ++slot) {
        SaveInfo info;
        if (slot < infos.size()) info = infos[slot];

        auto *row = new QWidget(m_slotsContainer);
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
        infoLabel->setStyleSheet("color: #e8e8ff; font-size: 14px;");
        if (info.exists) {
            infoLabel->setText(QString("ROUND %1  |  PLAYER %2  |  SAVED AT %3")
                .arg(info.roundNumber).arg(info.currentPlayer + 1).arg(info.displayTime()));
        } else {
            infoLabel->setText("(EMPTY)");
            infoLabel->setStyleSheet("color: #666; font-size: 14px; font-style: italic;");
        }
        rowLayout->addWidget(infoLabel, 1);

        auto *loadBtn = new QPushButton("LOAD", row);
        loadBtn->setEnabled(info.exists);
        loadBtn->setCursor(Qt::PointingHandCursor);
        loadBtn->setStyleSheet(
            "QPushButton {"
            "min-width: 100px; min-height: 40px; border-radius: 8px;"
            "border: 2px solid rgba(255, 255, 255, 180); font-weight: bold;"
            "color: white; font-size: 14px;"
            "background: rgba(40, 80, 120, 150);"
            "}"
            "QPushButton:hover:!disabled {"
            "background: rgba(70, 130, 200, 200); border: 2px solid white;"
            "}"
            "QPushButton:pressed:!disabled {"
            "background: rgba(50, 100, 180, 220);"
            "}"
            "QPushButton:disabled {"
            "background: rgba(30, 30, 40, 100); border: 2px solid rgba(100, 100, 100, 100);"
            "color: #666;"
            "}"
        );
        connect(loadBtn, &QPushButton::clicked, this, [this, slot](){ onLoadClicked(slot); });
        rowLayout->addWidget(loadBtn);

        auto *delBtn = new QPushButton("DELETE", row);
        delBtn->setEnabled(info.exists);
        delBtn->setCursor(Qt::PointingHandCursor);
        delBtn->setStyleSheet(
            "QPushButton {"
            "min-width: 100px; min-height: 40px; border-radius: 8px;"
            "border: 2px solid rgba(255, 100, 100, 180); font-weight: bold;"
            "color: white; font-size: 14px;"
            "background: rgba(100, 40, 40, 150);"
            "}"
            "QPushButton:hover:!disabled {"
            "background: rgba(180, 60, 60, 200); border: 2px solid #ff6666;"
            "}"
            "QPushButton:pressed:!disabled {"
            "background: rgba(150, 50, 50, 220);"
            "}"
            "QPushButton:disabled {"
            "background: rgba(30, 30, 40, 100); border: 2px solid rgba(100, 100, 100, 100);"
            "color: #666;"
            "}"
        );
        connect(delBtn, &QPushButton::clicked, this, [this, slot](){ onDeleteClicked(slot); });
        rowLayout->addWidget(delBtn);

        layout->addWidget(row);
    }

    static_cast<QVBoxLayout*>(this->layout())->insertWidget(1, m_slotsContainer, 1);
}

void SaveManagerPage::refreshSlots() {
    if (m_slotsContainer) {
        static_cast<QVBoxLayout*>(this->layout())->removeWidget(m_slotsContainer);
        m_slotsContainer->deleteLater();
        m_slotsContainer = nullptr;
        buildSlots();
    }
}

void SaveManagerPage::onLoadClicked(int slot) {
    if (m_loadSlotCmd) m_loadSlotCmd(slot);
    emit loadRequested();
}

void SaveManagerPage::onDeleteClicked(int slot) {
    auto ret = QMessageBox::question(this, "Confirm delete",
        QString("Delete slot %1?").arg(slot + 1),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret != QMessageBox::Yes) return;
    if (m_deleteSlotCmd) m_deleteSlotCmd(slot);
}

void SaveManagerPage::onBackClicked() { emit backRequested(); }