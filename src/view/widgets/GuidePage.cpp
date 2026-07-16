#include "GuidePage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QFont>

GuidePage::GuidePage(QWidget *parent) : QWidget(parent) {
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(40, 30, 40, 30);
    root->setSpacing(20);

    auto *title = new QLabel("GAME GUIDE", this);
    title->setStyleSheet(
        "color: white; font-size: 36px; font-weight: bold;"
        "padding: 16px 32px; letter-spacing: 2px;"
        "background: rgba(0, 0, 0, 140); border-radius: 16px;"
    );
    title->setAlignment(Qt::AlignCenter);
    root->addWidget(title);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet(
        "QScrollArea { background: rgba(0, 0, 0, 120); border-radius: 12px; }"
        "QScrollArea QWidget { background: transparent; }"
        "QScrollBar:vertical { width: 8px; background: rgba(50, 50, 80, 100); border-radius: 4px; }"
        "QScrollBar::handle:vertical { background: rgba(100, 100, 150, 150); border-radius: 4px; }"
        "QScrollBar::handle:vertical:hover { background: rgba(150, 150, 200, 180); }"
    );

    auto *contentWidget = new QWidget(this);
    auto *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(16);

    auto addRuleSection = [&](const QString &header, const QString &text) {
        auto *headerLabel = new QLabel(header, contentWidget);
        headerLabel->setStyleSheet(
            "color: #ffcc00; font-size: 16px; font-weight: bold;"
            "padding: 8px 12px; background: rgba(0, 0, 0, 100); border-radius: 8px;"
        );
        contentLayout->addWidget(headerLabel);

        auto *textLabel = new QLabel(text, contentWidget);
        textLabel->setStyleSheet("color: #e8e8ff; font-size: 14px;");
        textLabel->setWordWrap(true);
        textLabel->setAlignment(Qt::AlignTop);
        contentLayout->addWidget(textLabel);
    };

    addRuleSection("OBJECTIVE",
        "Destroy all of your opponent's squares by firing mathematical functions at them!");

    addRuleSection("HOW TO PLAY",
        "Each turn, a random square from your side is selected as the launch point. "
        "Enter a function f(x) in the input box - the game will automatically adjust "
        "it to pass through your launch point. The function curve will travel across "
        "the battlefield towards your opponent's squares.");

    addRuleSection("FUNCTION INPUT",
        "Supported operations: +, -, *, /, ^ (power)\n"
        "Supported functions: sin, cos, tan, asin, acos, atan, sqrt, abs, log, ln, exp\n"
        "Example: x^2 + sin(x) - 3");

    addRuleSection("COST SYSTEM",
        "Each mathematical operation costs points. You start with 5 points and gain 2 "
        "more points each round. Complex functions cost more - plan your strategy carefully!");

    addRuleSection("OBSTACLES",
        "Random obstacles are scattered across the battlefield. Hitting an obstacle "
        "destroys it but ends your turn immediately. Destroyed obstacles no longer "
        "block projectiles.");

    addRuleSection("WINNING",
        "Be the first to destroy all of your opponent's squares to win the game!");

    addRuleSection("TIPS",
        "• Simple linear functions (like x or -x) are cheap and effective\n"
        "• Use quadratic and trigonometric functions to curve around obstacles\n"
        "• Watch your opponent's trajectories to anticipate their strategy\n"
        "• Press ESC to pause the game at any time");

    scrollArea->setWidget(contentWidget);
    root->addWidget(scrollArea, 1);

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
    connect(backBtn, &QPushButton::clicked, this, &GuidePage::onBackClicked);
    root->addWidget(backBtn, 0, Qt::AlignCenter);
}

GuidePage::~GuidePage() noexcept {}

void GuidePage::onBackClicked() { emit backRequested(); }