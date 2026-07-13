#pragma once
#include <QWidget>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QVector>
#include "model/GameConfig.h"
#include "common/frame.h"

class ConfigPage : public QWidget {
    Q_OBJECT
public:
    explicit ConfigPage(QWidget *parent = nullptr);
    ConfigPage(const ConfigPage&) = delete;
    ~ConfigPage() noexcept;
    ConfigPage& operator=(const ConfigPage&) = delete;

    void refresh(const GameConfig &cfg);

signals:
    void backToStart();
    void configSaved(const GameConfig &cfg);

private slots:
    void onColorClicked(int player, int index);
    void onStartClicked();
    void onBackClicked();

private:
    QWidget* createColorPalette(int player);
    void updateColorBorders();

    QSpinBox *m_squareCount;
    QSpinBox *m_obstacleCount;
    QDoubleSpinBox *m_obstacleSize;
    QCheckBox *m_showCoords;
    QCheckBox *m_showGrid;
    QVector<QLabel*> m_p1Swatches;
    QVector<QLabel*> m_p2Swatches;
    int m_p1ColorIndex = 0;
    int m_p2ColorIndex = 1;

    static const QVector<QColor> kPresetColors;
};
