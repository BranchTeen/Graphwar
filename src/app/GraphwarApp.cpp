#include "GraphwarApp.h"

GraphwarApp::GraphwarApp() : m_main_wnd(&m_view_model) {
}

GraphwarApp::~GraphwarApp() noexcept {
}

void GraphwarApp::show_main_window() {
    m_main_wnd.show();
}
