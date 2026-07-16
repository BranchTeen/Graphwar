#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class GuidePage : public QWidget {
    Q_OBJECT
public:
    explicit GuidePage(QWidget *parent = nullptr);
    GuidePage(const GuidePage&) = delete;
    ~GuidePage() noexcept;
    GuidePage& operator=(const GuidePage&) = delete;

signals:
    void backRequested();

private slots:
    void onBackClicked();
};