#pragma once
#include "GUI_Element.h"
class GUI_Viewport :
    public GUI_Element
{
public:
    GUI_Viewport(const std::string& i_name, GUI_Interface* i_owner);
    ~GUI_Viewport();
    void ReadIn(std::stringstream& i_stream)override;
    void OnClick(const sf::Vector2f& i_mousePos)override;
    void OnRelease()override;
    void OnHover(const sf::Vector2f& i_mousePos)override;
    void OnLeave()override;
    void Update(float i_dT)override;
    void Draw(sf::RenderTarget* i_target)override;
    void DrawOverlay(sf::RenderTarget* i_target) override;
    void ApplyBgStyle()override;
    void CallbackSetup()override;

    void UpdateCamera(sf::RenderWindow* i_wind);
    // Keeps m_view's viewport rect in step with the element's on-screen rect.
    void UpdateViewportRect();
    void OnResize(const sf::Vector2f& i_scale) override;
    void ClearBrush();
    sf::View& GetView();
    void SetViewSize(const sf::Vector2f& i_size);
    void SetViewCenter(const sf::Vector2f& i_pos);
	void SetBrush(std::string& i_texture, int i_tileId);
    void SetCameraSpeed(float i_speed);
    void SetHoverTilesPos(const sf::Vector2i& i_pos);
	void SetClickedTileInfo(TileInfo*i_info);
    
	float& GetCameraSpeed();
    // World units per on-screen pixel inside this element.
    sf::Vector2f GetWorldPerPixel();
    void React(EventDetails* i_details);
private:
	//Change the map based on the clicked tile position and the current brush
    void ChangeMap();
    sf::Sprite m_hoverTile;
    sf::Vector2i m_hoverTilePos;
    sf::Vector2i m_clickedTilePos;
	TileInfo* m_clickedTileInfo;
    float m_cameraSpeed;
};

