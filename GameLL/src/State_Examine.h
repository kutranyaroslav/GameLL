#pragma once
#include "StateManager.h"
class GUI_Interface;
class GUI_ItemPreview;

class State_Examine : public BaseState {
public:
    explicit State_Examine(StateManager* manager);
    void onCreate() override;
    void onDestroy() override;
    void Activate() override;
    void Deactivate() override;
    void Update(const sf::Time&) override;
    void Draw() override;
    void DrawOverlay() override;
    void OnResize(const sf::Vector2u&) override;
    void ShowItem(Items::ItemId item, StateType returnState);
    void ShowNote(const std::string& textId, StateType returnState);
    void ShowText(const std::string& title, const std::string& text, StateType returnState);
    void Unpause(EventDetails* details);
private:
    void HandleControl(EventDetails* details);
    void Layout();
    void UpdatePage();
    GUI_Interface* Interface() const;
    GUI_ItemPreview* Preview() const;
    StateType m_returnState = StateType::Game;
    bool m_noteMode = true;
    bool m_closeRequested = false;
    bool m_fontLoaded = false;
    sf::Font* m_font = nullptr;
    std::string m_title = "EXAMINE";
    std::string m_text = "Nothing selected.";
    std::string m_category;
    std::vector<sf::String> m_pages;
    std::size_t m_page = 0;
};
