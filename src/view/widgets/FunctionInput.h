#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "common/frame.h"

class FunctionInput : public QWidget {
    Q_OBJECT
public:
    explicit FunctionInput(QWidget *parent = nullptr);
    FunctionInput(const FunctionInput&) = delete;
    ~FunctionInput() noexcept;
    FunctionInput& operator=(const FunctionInput&) = delete;

    void set_launch_command(std::function<void(const QString&)>&& cmd) noexcept { m_launchCmd = std::move(cmd); }
    void set_update_cost_preview_command(std::function<void(const QString&)>&& cmd) noexcept { m_costPreviewCmd = std::move(cmd); }
    void set_pause_command(std::function<void()>&& cmd) noexcept { m_pauseCmd = std::move(cmd); }

    void setInputEnabled(bool enabled);
    void setMessage(const QString &msg);
    void setCostPreview(int cost);
    void clear() { m_input->clear(); }
    void setFocus() { m_input->setFocus(); }

private slots:
    void onTextChanged(const QString &text);
    void onLaunch();

private:
    QLineEdit *m_input;
    QPushButton *m_fireBtn;
    QLabel *m_messageLabel;
    QLabel *m_costLabel;

    std::function<void()> m_pauseCmd;
    std::function<void(const QString&)> m_launchCmd;
    std::function<void(const QString&)> m_costPreviewCmd;
};
