#pragma once
#include "view/MainWindow.h"
#include "viewmodel/GameViewModel.h"

class GraphwarApp {
public:
    GraphwarApp();
    GraphwarApp(const GraphwarApp&) = delete;
    ~GraphwarApp() noexcept;

    GraphwarApp& operator=(const GraphwarApp&) = delete;

    void show_main_window() { m_main_wnd.show(); }

private:
    GameViewModel m_view_model;
    MainWindow m_main_wnd;
};
