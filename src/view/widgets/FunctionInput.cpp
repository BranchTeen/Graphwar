#include "FunctionInput.h"
#include <QHBoxLayout>
#include <QFont>

FunctionInput::FunctionInput(QWidget *parent) : QWidget(parent) {
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);

    auto *pauseBtn = new QPushButton("⏸ Pause", this);
    pauseBtn->setFont(QFont("Consolas", 11));
    pauseBtn->setStyleSheet(
        "QPushButton{background:#556;color:#fff;padding:6px 12px;border-radius:4px;}"
        "QPushButton:hover{background:#779;}");
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

    m_fireBtn = new QPushButton("Fire!", this);
    m_fireBtn->setFont(QFont("Consolas", 12, QFont::Bold));
    m_fireBtn->setDefault(true);
    m_fireBtn->setStyleSheet(
        "QPushButton { background: #4a7; color: white; padding: 6px 20px; border-radius: 4px; }"
        "QPushButton:disabled { background: #555; color: #888; }");
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
