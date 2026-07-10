#pragma once
#include <QWidget>
#include <QPointF>
#include "viewmodel/GameViewModel.h"

// GameCanvas：MVVM 的 View 层组件
// - 只通过 GameViewModel 的只读接口获取数据（不直接访问 GameModel）
// - 订阅 GameViewModel 的 signals 来触发重绘
class GameCanvas : public QWidget {
    Q_OBJECT
public:
    explicit GameCanvas(GameViewModel *vm, QWidget *parent = nullptr);

public slots:
    void refresh();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPointF worldToScreen(double wx, double wy) const;

    GameViewModel *m_vm = nullptr;
    double m_scale = 40.0;
    double m_ox = 0, m_oy = 0;
};
