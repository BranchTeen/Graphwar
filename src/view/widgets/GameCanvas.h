#pragma once
#include <QWidget>
#include <QPointF>
#include "common/frame.h"

class GameViewModel;

class GameCanvas : public QWidget {
    Q_OBJECT
public:
    explicit GameCanvas(QWidget *parent = nullptr);
    GameCanvas(const GameCanvas&) = delete;
    ~GameCanvas() noexcept;
    GameCanvas& operator=(const GameCanvas&) = delete;

    void set_view_model(const GameViewModel* vm) noexcept { m_vm = vm; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPointF worldToScreen(double wx, double wy) const;

    const GameViewModel *m_vm = nullptr;
    double m_scale = 40.0;
    double m_ox = 0, m_oy = 0;
};
