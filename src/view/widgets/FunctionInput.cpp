#include "FunctionInput.h"
#include <QHBoxLayout>
#include <QFont>

FunctionInput::FunctionInput(QWidget *parent) : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);

    auto *pauseBtn = new QPushButton("⏸ PAUSE", this);
    pauseBtn->setFont(QFont("Consolas", 11, QFont::Bold));
    pauseBtn->setCursor(Qt::PointingHandCursor);
    pauseBtn->setStyleSheet(
        "QPushButton {"
        "min-width: 80px; min-height: 40px; border-radius: 8px;"
        "border: 1px solid rgba(255, 255, 255, 150); font-weight: bold;"
        "color: white; font-size: 12px;"
        "background: rgba(40, 40, 60, 170);"
        "}"
        "QPushButton:hover {"
        "background: rgba(70, 130, 200, 200); border: 1px solid white;"
        "}"
        "QPushButton:pressed {"
        "background: rgba(50, 100, 180, 220);"
        "}"
    );
    connect(pauseBtn, &QPushButton::clicked, this, [this]() {
        if (m_pauseCmd) m_pauseCmd();
    });
    layout->addWidget(pauseBtn);

    m_input = new QLineEdit(this);
    m_input->setPlaceholderText("f(x) = ...   (Enter to fire)");
    m_input->setFont(QFont("Consolas", 14));
    layout->addWidget(m_input, 1);

    m_costLabel = new QLabel("Cost: 0", this);
    m_costLabel->setFont(QFont("Consolas", 12));
    layout->addWidget(m_costLabel);

    m_fireBtn = new QPushButton("FIRE!", this);
    m_fireBtn->setFont(QFont("Consolas", 12, QFont::Bold));
    m_fireBtn->setDefault(true);
    m_fireBtn->setCursor(Qt::PointingHandCursor);
    m_fireBtn->setStyleSheet(
        "QPushButton {"
        "min-width: 80px; min-height: 40px; border-radius: 8px;"
        "border: 2px solid rgba(255, 255, 255, 200); font-weight: bold;"
        "color: white; font-size: 14px;"
        "background: rgba(40, 100, 80, 170);"
        "}"
        "QPushButton:hover:!disabled {"
        "background: rgba(70, 160, 130, 200); border: 2px solid white;"
        "}"
        "QPushButton:pressed:!disabled {"
        "background: rgba(50, 130, 100, 220);"
        "}"
        "QPushButton:disabled {"
        "background: rgba(30, 30, 40, 100); border: 2px solid rgba(100, 100, 100, 100);"
        "color: #666;"
        "}"
    );
    layout->addWidget(m_fireBtn);

    m_messageLabel = new QLabel(this);
    m_messageLabel->setFont(QFont("Consolas", 11));
    layout->addWidget(m_messageLabel, 1);

    connect(m_input, &QLineEdit::textChanged, this, &FunctionInput::onTextChanged);
    connect(m_input, &QLineEdit::returnPressed, this, &FunctionInput::onLaunch);
    connect(m_fireBtn, &QPushButton::clicked, this, &FunctionInput::onLaunch);
}

FunctionInput::~FunctionInput() noexcept {}

void FunctionInput::setInputEnabled(bool enabled) {
    m_input->setEnabled(enabled);
    m_fireBtn->setEnabled(enabled);
}

void FunctionInput::setMessage(const QString &msg) {
    m_messageLabel->setText(msg);
}

void FunctionInput::setCostPreview(int cost) {
    m_costLabel->setText(QString("Cost: %1").arg(cost));
}

void FunctionInput::onTextChanged(const QString &text) {
    if (m_costPreviewCmd) m_costPreviewCmd(text);
}

void FunctionInput::onLaunch() {
    if (m_launchCmd) m_launchCmd(m_input->text());
}
