#pragma once
#include <QWidget>
#include <QPointF>
#include "common/frame.h"

struct GameState;

class GameCanvas : public QWidget {
    Q_OBJECT
public:
    explicit GameCanvas(QWidget *parent = nullptr);
    GameCanvas(const GameCanvas&) = delete;
    ~GameCanvas() noexcept;
    GameCanvas& operator=(const GameCanvas&) = delete;

    void set_state(const GameState* state) noexcept { m_state = state; }

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPointF worldToScreen(double wx, double wy) const;

    const GameState *m_state = nullptr;
    double m_scale = 40.0;
    double m_ox = 0, m_oy = 0;
};
