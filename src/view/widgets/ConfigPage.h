#pragma once
#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QVector>
#include <QColor>
#include "common/GameConfig.h"

class ConfigPage : public QWidget {
    Q_OBJECT
public:
    explicit ConfigPage(QWidget *parent = nullptr);

    void refresh(const GameConfig &cfg);

signals:
    void configSaved(const GameConfig &config);
    void backToStart();

private:
    static int matchColorIndex(const QColor &c);

    void build();
    QWidget *createSwatchRow(int player);
    void selectSwatch(int player, int index);

    QSpinBox *m_squaresSpin;
    QSpinBox *m_obstacleCountSpin;
    QDoubleSpinBox *m_obstacleSizeSpin;
    QCheckBox *m_coordCheck;
    QCheckBox *m_gridCheck;
    QVector<QPushButton*> m_p1Swatches;
    QVector<QPushButton*> m_p2Swatches;
    int m_p1Index = 0;
    int m_p2Index = 1;

    static const QVector<QColor> s_colors;
};
