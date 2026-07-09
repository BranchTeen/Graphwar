#include "FunctionInput.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTimer>

FunctionInput::FunctionInput(GameViewModel *vm, QWidget *parent)
    : QWidget(parent) {
    setViewModel(vm);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 5, 10, 5);

    m_input = new QLineEdit(this);
    m_input->setPlaceholderText("f(x) = ...");
    m_input->setFont(QFont("Consolas", 14));
    layout->addWidget(m_input, 1);

    m_costLabel = new QLabel("Cost: 0", this);
    m_costLabel->setFont(QFont("Consolas", 12));
    layout->addWidget(m_costLabel);

    m_launchBtn = new QPushButton("Fire!", this);
    m_launchBtn->setFont(QFont("Consolas", 12, QFont::Bold));
    m_launchBtn->setStyleSheet(
        "QPushButton { background: #4a7; color: white; padding: 6px 20px; border-radius: 4px; }"
        "QPushButton:disabled { background: #555; color: #888; }");
    layout->addWidget(m_launchBtn);

    m_msgLabel = new QLabel(this);
    m_msgLabel->setFont(QFont("Consolas", 11));
    layout->addWidget(m_msgLabel, 1);

    connect(m_input, &QLineEdit::textChanged, this, &FunctionInput::onTextChanged);
    connect(m_input, &QLineEdit::returnPressed, this, &FunctionInput::onLaunch);
    connect(m_launchBtn, &QPushButton::clicked, this, &FunctionInput::onLaunch);
}

void FunctionInput::setViewModel(GameViewModel *vm) {
    m_vm = vm;
    if (m_vm) {
        connect(m_vm, &GameViewModel::phaseChanged, this, &FunctionInput::onPhaseChanged);
        connect(m_vm, &GameViewModel::turnChanged, this, [this](int) {
            m_input->clear();
        });
        connect(m_vm, &GameViewModel::messageChanged, this, [this](const QString &msg) {
            m_msgLabel->setText(msg);
        });
        connect(m_vm, &GameViewModel::costPreviewChanged, this, [this](int cost) {
            m_costLabel->setText(QString("Cost: %1").arg(cost));
        });
    }
}

void FunctionInput::onTextChanged(const QString &text) {
    if (m_vm) m_vm->updateCostPreview(text);
}

void FunctionInput::onLaunch() {
    if (m_vm) m_vm->launch(m_input->text());
}

void FunctionInput::onPhaseChanged(GamePhase phase) {
    bool enabled = (phase == GamePhase::WaitingInput);
    m_input->setEnabled(enabled);
    m_launchBtn->setEnabled(enabled);
    if (phase == GamePhase::RoundEnd || phase == GamePhase::GameOver) {
        if (m_vm && phase == GamePhase::RoundEnd) {
            // Auto advance after short delay
            QTimer::singleShot(1500, m_vm, &GameViewModel::nextTurn);
        }
    }
}
