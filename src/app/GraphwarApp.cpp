#include "GraphwarApp.h"
#include "view/widgets/GameCanvas.h"
#include "view/widgets/FunctionInput.h"
#include "view/widgets/SaveManagerPage.h"
#include "view/widgets/PauseMenuPage.h"

GraphwarApp::GraphwarApp()
    : m_main_wnd()
{
    const GameState *state = m_view_model.get_state();

    m_main_wnd.set_state(state);
    m_main_wnd.set_cost_preview_ptr(&m_view_model.costPreview());
    m_main_wnd.get_canvas()->set_state(state);

    // === Wire commands ===
    m_main_wnd.set_new_game_command(m_view_model.get_new_game_command());
    m_main_wnd.set_pause_command(m_view_model.get_pause_command());
    m_main_wnd.set_resume_command(m_view_model.get_resume_command());
    m_main_wnd.set_set_config_command(m_view_model.get_set_config_command());
    m_main_wnd.set_load_slot_command(m_view_model.get_load_slot_command());
    m_main_wnd.set_delete_slot_command(m_view_model.get_delete_slot_command());
    m_main_wnd.set_save_slot_command(m_view_model.get_save_slot_command());
    m_main_wnd.set_next_turn_command(m_view_model.get_next_turn_command());

    m_main_wnd.get_input()->set_launch_command(m_view_model.get_launch_command());
    m_main_wnd.get_input()->set_update_cost_preview_command(m_view_model.get_update_cost_preview_command());
    m_main_wnd.get_input()->set_pause_command(m_view_model.get_pause_command());

    m_main_wnd.get_save_page()->set_load_slot_command(m_view_model.get_load_slot_command());
    m_main_wnd.get_save_page()->set_delete_slot_command(m_view_model.get_delete_slot_command());

    m_main_wnd.get_pause_page()->set_resume_command(m_view_model.get_resume_command());
    m_main_wnd.get_pause_page()->set_save_slot_command(m_view_model.get_save_slot_command());

    // === Wire notification chain ===
    m_view_model.add_notification(m_main_wnd.get_notification());
}

GraphwarApp::~GraphwarApp() noexcept {}
