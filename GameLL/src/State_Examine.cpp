#include "State_Examine.h"
#include "GUI_Manager.h"
#include "GUI_ItemPreview.h"
#include "ExamineText.h"
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iterator>

namespace {
const char* Panel = "ExaminePanel";
const sf::Color Background(9, 9, 11), Text(226, 220, 208);
const sf::Color Muted(147, 139, 137), Red(207, 54, 61), Line(77, 37, 42);
const char* Category(Items::ItemCategory category) {
    switch (category) {
    case Items::Consumable: return "CONSUMABLE";
    case Items::Ammo: return "AMMUNITION";
    case Items::Weapon: return "WEAPON";
    case Items::KeyItem: return "KEY ITEM";
    default: return "ITEM";
    }
}
}
State_Examine::State_Examine(StateManager* manager) : BaseState(manager) {}
GUI_Interface* State_Examine::Interface() const {
    return m_stateManager->GetSharedContext()->m_guiManager->GetInterface(m_state, Panel);
}
GUI_ItemPreview* State_Examine::Preview() const {
    auto* panel = Interface();
    return panel ? dynamic_cast<GUI_ItemPreview*>(panel->GetElement("Preview")) : nullptr;
}
void State_Examine::onCreate() {
    m_state = StateType::Examine;
    SetTransparent(false);
    SetTranscendent(false);
    auto* context = m_stateManager->GetSharedContext();
    m_view = context->m_wind->GetUIView();
    m_fontLoaded = context->m_fontManager->RequireResource("Main");
    if (m_fontLoaded) { m_font = context->m_fontManager->GetResource("Main"); }
    auto* gui = context->m_guiManager;
    gui->AddInterface(m_state, Panel);
    auto* panel = Interface();
    if (!panel) { return; }
    for (const char* name : {"Header", "Mode", "TopRule", "BottomRule", "Title",
         "Category", "Body", "Page", "Back", "Previous", "Next", "RotateLeft",
         "RotateRight", "ZoomIn", "ZoomOut", "Reset", "Hint", "ImageStatus"}) {
        panel->AddElement(GUI_ElementType::Label, name);
    }
    panel->AddElement(GUI_ElementType::ItemPreview, "Preview");
    auto* events = context->m_eventManager;
    events->AddCallback(m_state, "Key_Escape", &State_Examine::Unpause, this);
    const std::pair<const char*, sf::Keyboard::Key> keys[] = {
        {"Previous", sf::Keyboard::Left}, {"Next", sf::Keyboard::Right},
        {"ZoomIn", sf::Keyboard::Up}, {"ZoomOut", sf::Keyboard::Down},
        {"RotateLeft", sf::Keyboard::Q}, {"RotateRight", sf::Keyboard::E},
        {"Reset", sf::Keyboard::R}, {"Next", sf::Keyboard::Return}
    };
    for (std::size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        const std::string name = "Examine_Key_" + std::to_string(i);
        auto* binding = new Binding(name);
        binding->BindEvent(EventType::KeyDown, EventInfo(keys[i].second));
        if (!events->AddBinding(binding)) { delete binding; }
        events->AddCallback(m_state, name, &State_Examine::HandleControl, this);
        m_dynamicCallbacks.push_back(name);
    }
    for (const char* action : {"Back", "Previous", "Next", "RotateLeft", "RotateRight",
                               "ZoomIn", "ZoomOut", "Reset"}) {
        const std::string name = std::string("Examine_Click_") + action;
        events->AddDynamicBinding(name, EventType::GUI_Click, Panel, action);
        events->AddCallback(m_state, name, &State_Examine::HandleControl, this);
        m_dynamicCallbacks.push_back(name);
    }
    Layout();
}
void State_Examine::onDestroy() {
    auto* context = m_stateManager->GetSharedContext();
    context->m_eventManager->RemoveCallback(m_state, "Key_Escape");
    for (const auto& name : m_dynamicCallbacks) {
        context->m_eventManager->RemoveCallback(m_state, name);
        context->m_eventManager->RemoveBinding(name);
    }
    context->m_guiManager->RemoveInterface(m_state, Panel);
    if (m_fontLoaded) { context->m_fontManager->ReleaseResource("Main"); }
    m_font = nullptr;
    m_fontLoaded = false;
}
void State_Examine::Activate() { m_closeRequested = false; Layout(); }
void State_Examine::Deactivate() {}
void State_Examine::Update(const sf::Time&) {}
void State_Examine::Draw() {}
void State_Examine::DrawOverlay() {
    auto* wind = m_stateManager->GetSharedContext()->m_wind;
    wind->GetRenderWindow()->setView(*wind->GetUIView());
}
void State_Examine::OnResize(const sf::Vector2u&) { Layout(); }
void State_Examine::Unpause(EventDetails*) {
    if (m_closeRequested) { return; }
    m_closeRequested = true;
    m_stateManager->CloseExamine(m_returnState);
}
void State_Examine::ShowItem(Items::ItemId item, StateType returnState) {
    auto* manager = m_stateManager->GetSharedContext()->m_itemManager;
    const auto* definition = manager ? manager->GetItemDef(item) : nullptr;
    m_returnState = returnState;
    m_noteMode = false;
    m_page = 0;
    m_title = definition ? definition->m_name : "UNKNOWN ITEM";
    m_text = definition && !definition->m_description.empty()
        ? definition->m_description : "No further information is available.";
    m_category = definition ? Category(definition->m_category) : "ITEM";
    if (auto* preview = Preview()) { preview->SetImage(definition ? definition->m_icon : ""); }
    Layout();
}
void State_Examine::ShowText(const std::string& title, const std::string& text, StateType returnState) {
    m_noteMode = true;
    m_returnState = returnState;
    m_title = title;
    m_text = text;
    m_page = 0;
    if (auto* preview = Preview()) { preview->SetImage(""); preview->SetActive(false); }
    Layout();
}
void State_Examine::ShowNote(const std::string& textId, StateType returnState) {
    // Notes are UTF-8: the first line is the title, all remaining lines are body text.
    const std::filesystem::path id(textId);
    if (textId.empty() || id.has_parent_path() || id.is_absolute() || textId == "." || textId == "..") {
        ShowText("DOCUMENT", "This document is unavailable.", returnState);
        return;
    }
    const std::string filename = id.has_extension() ? textId : textId + ".txt";
    const auto path = std::filesystem::path(Utils::GetWorkingDirectory()) / "Assets" / "Notes" / filename;
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Could not open note: " << path.string() << '\n';
        ShowText("DOCUMENT", "This document is unavailable.", returnState);
        return;
    }
    std::string title;
    std::getline(file, title);
    if (title.compare(0, 3, "\xEF\xBB\xBF") == 0) { title.erase(0, 3); }
    if (!title.empty() && title.back() == '\r') { title.pop_back(); }
    const std::string body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    if (file.bad()) { ShowText("DOCUMENT", "This document is unavailable.", returnState); return; }
    if (body.find_first_not_of(" \t\r\n") == std::string::npos) {
        ShowText("Note", title, returnState);
    } else {
        ShowText(title.empty() ? "Untitled document" : title, body, returnState);
    }
    auto& notes = m_stateManager->GetSharedContext()->m_readNotes;
    if (std::find(notes.begin(), notes.end(), textId) == notes.end()) { notes.push_back(textId); }
}
void State_Examine::HandleControl(EventDetails* details) {
    if (!details || m_closeRequested) { return; }
    std::string action = details->m_guiElement;
    if (action.empty()) {
        // Existing EventManager callbacks reliably carry the binding name;
        // its key-code field is not populated for every key event.
        const char* actions[] = {"Previous", "Next", "ZoomIn", "ZoomOut",
                                 "RotateLeft", "RotateRight", "Reset", "Next"};
        for (unsigned i = 0; i < 8; ++i) {
            if (details->m_name == "Examine_Key_" + std::to_string(i)) {
                action = actions[i];
                break;
            }
        }
    }
    if (action == "Back") { Unpause(details); return; }
    if (action == "Previous" && m_page > 0) { --m_page; UpdatePage(); }
    if (action == "Next" && m_page + 1 < m_pages.size()) { ++m_page; UpdatePage(); }
    if (!m_noteMode) {
        if (auto* preview = Preview(); preview && preview->HasImage()) {
            if (action == "RotateLeft") { preview->Rotate(-15.f); }
            if (action == "RotateRight") { preview->Rotate(15.f); }
            if (action == "ZoomIn") { preview->Zoom(.15f); }
            if (action == "ZoomOut") { preview->Zoom(-.15f); }
            if (action == "Reset") { preview->ResetView(); }
            // Input is processed after GUI::Update; refresh the cached content now.
            Interface()->RedrawContent();
        }
    }
}
void State_Examine::Layout() {
    auto* panel = Interface();
    if (!panel) { return; }
    const auto window = m_stateManager->GetSharedContext()->m_wind->GetUIView()->getSize();
    const float scale = std::max(.01f, std::min(window.x / 1200.f, window.y / 720.f));
    const sf::Vector2f offset((window.x - 1200.f * scale) / 2.f, (window.y - 720.f * scale) / 2.f);
    panel->SetScale(1.f);
    panel->SetPosition({0.f, 0.f});
    GUI_Style base;
    base.m_size = {std::max(1.f, window.x), std::max(1.f, window.y)};
    base.m_backgroundColor = Background;
    for (auto state : {GUI_ElementState::Neutral, GUI_ElementState::Focused, GUI_ElementState::Clicked}) {
        panel->UpdateStyle(state, base);
    }
    auto label = [&](const char* name, const sf::String& text, float x, float y,
                     float w, float h, unsigned fontSize, sf::Color color, bool button = false) {
        auto* element = panel->GetElement(name);
        element->SetScale(1.f);
        element->SetActive(true);
        element->SetPosition(offset + sf::Vector2f(x * scale, y * scale));
        element->SetUnicodeText(text);
        GUI_Style style;
        style.m_size = {w * scale, h * scale};
        style.m_textFont = m_font ? "Main" : "";
        style.m_textSize = std::max(1u, static_cast<unsigned>(std::lround(fontSize * scale)));
        style.m_textColor = color;
        style.m_textPadding = button ? sf::Vector2f(12.f * scale, 9.f * scale) : sf::Vector2f();
        style.m_backgroundColor = button ? sf::Color(36, 23, 27) : sf::Color::Transparent;
        element->UpdateStyle(GUI_ElementState::Neutral, style);
        if (button) { style.m_backgroundColor = sf::Color(92, 30, 37); style.m_textColor = Text; }
        element->UpdateStyle(GUI_ElementState::Focused, style);
        element->UpdateStyle(GUI_ElementState::Clicked, style);
    };
    label("Header", m_noteMode ? "DOCUMENT" : "EXAMINE", 40, 28, 500, 40, 27, Red);
    label("Mode", m_noteMode ? "ARCHIVE / READING" : "PERSONAL EFFECTS / INSPECTION", 720, 37, 440, 30, 14, Muted);
    for (const auto& rule : {std::make_pair("TopRule", 80.f), std::make_pair("BottomRule", 630.f)}) {
        label(rule.first, "", 40, rule.second, 1120, 2, 1, Red);
        auto* element = panel->GetElement(rule.first);
        GUI_Style style; style.m_size = {1120.f * scale, 2.f * scale}; style.m_backgroundColor = Line;
        for (auto state : {GUI_ElementState::Neutral, GUI_ElementState::Focused, GUI_ElementState::Clicked}) {
            element->UpdateStyle(state, style);
        }
    }
    if (m_noteMode) {
        constexpr float left = 60.f, width = 1080.f;
        constexpr unsigned titleSize = 42, bodySize = 36;
        sf::String heading = ExamineText::Utf8(m_title);
        sf::String body = ExamineText::Utf8(m_text);
        const auto pixels = [scale](unsigned size) {
            return std::max(1u, static_cast<unsigned>(std::lround(size * scale)));
        };
        if (m_font) {
            sf::Text measure(heading, *m_font, pixels(titleSize));
            if (heading.find("\n") != sf::String::InvalidPos ||
                ExamineText::Extent(measure).x > width * scale ||
                ExamineText::Extent(measure).y > 60.f * scale) {
                // Keep the complete heading readable in the paginated text.
                body = heading + "\n\n" + body;
                heading = "Note";
            }
            m_pages = ExamineText::DocumentPages(body, *m_font, pixels(bodySize),
                width * scale, 410.f * scale);
        } else { m_pages = {body}; }
        label("Title", heading, left, 100, width, 60, titleSize, Text);
        panel->GetElement("Category")->SetActive(false);
        label("Body", "", left, 170, width, 410, bodySize, Text);
        label("Page", "", left, 590, width, 32, 20, Muted);
    } else {
        const float textX = 740.f;
        const float textWidth = 420.f;
        unsigned titleSize = 30;
        if (m_font) {
            sf::Text measure(ExamineText::Utf8(m_title), *m_font, 30);
            while (titleSize > 14 && measure.getLocalBounds().width > textWidth) { measure.setCharacterSize(--titleSize); }
        }
        label("Title", ExamineText::Utf8(m_title), textX, 120, textWidth, 68, titleSize, Text);
        label("Category", m_category, textX, 193, textWidth, 26, 14, Red);
        panel->GetElement("Category")->SetActive(!m_noteMode);
        const float bodyY = 250.f;
        const float bodyHeight = 300.f;
        label("Body", "", textX, bodyY, textWidth, bodyHeight, 21, Text);
        const unsigned bodySize = std::max(1u, static_cast<unsigned>(std::lround(21.f * scale)));
        if (m_font) {
            const unsigned lines = std::max(1u, static_cast<unsigned>((bodyHeight * scale - bodySize) / m_font->getLineSpacing(bodySize)));
            m_pages = ExamineText::Paginate(ExamineText::Utf8(m_text), *m_font, bodySize, textWidth * scale, lines);
        } else { m_pages = {ExamineText::Utf8(m_text)}; }
        label("Page", "", 740, 586, 420, 28, 14, Muted);
    }
    label("Back", "ESC  RETURN", 40, 655, 170, 40, 15, Text, true);
    label("Previous", "<  PREVIOUS", 830, 655, 160, 40, 15, Muted, true);
    label("Next", "NEXT  >", 1000, 655, 160, 40, 15, Text, true);
    label("Hint", m_noteMode ? "LEFT / RIGHT  TURN PAGE" : "Q / E  ROTATE     UP / DOWN  ZOOM", 245, 668, 555, 24, 13, Muted);
    auto* preview = Preview();
    if (preview) {
        preview->SetScale(1.f);
        preview->SetPosition(offset + sf::Vector2f(40.f * scale, 120.f * scale));
        GUI_Style style; style.m_size = {650.f * scale, 400.f * scale};
        for (auto state : {GUI_ElementState::Neutral, GUI_ElementState::Focused, GUI_ElementState::Clicked}) {
            preview->UpdateStyle(state, style);
        }
        preview->SetActive(!m_noteMode);
    }
    const bool imageAvailable = preview && preview->HasImage();
    label("ImageStatus", imageAvailable ? "" : "NO IMAGE AVAILABLE", 40, 530, 650, 24, 13, Muted);
    panel->GetElement("ImageStatus")->SetActive(!m_noteMode && !imageAvailable);
    const char* names[] = {"RotateLeft", "RotateRight", "ZoomOut", "ZoomIn", "Reset"};
    const char* captions[] = {"Q  ROTATE <", "E  ROTATE >", "-  ZOOM", "+  ZOOM", "R  RESET"};
    for (unsigned i = 0; i < 5; ++i) {
        label(names[i], captions[i], 40.f + i * 132.f, 565, 122, 40, 13, Muted, true);
        panel->GetElement(names[i])->SetActive(!m_noteMode && imageAvailable);
    }
    panel->AdjustContentSize();
    UpdatePage();
    panel->Redraw();
    panel->RedrawControls();
}
void State_Examine::UpdatePage() {
    auto* panel = Interface();
    if (!panel) { return; }
    if (m_pages.empty()) { m_pages.emplace_back(); }
    m_page = std::min(m_page, m_pages.size() - 1);
    panel->GetElement("Body")->SetUnicodeText(m_pages[m_page]);
    panel->GetElement("Page")->SetText("PAGE " + std::to_string(m_page + 1) + " / " + std::to_string(m_pages.size()));
    panel->GetElement("Previous")->SetActive(m_page > 0);
    panel->GetElement("Next")->SetActive(m_page + 1 < m_pages.size());
    panel->RedrawContent();
}
