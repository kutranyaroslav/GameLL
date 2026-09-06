#pragma once
#include "SFML/Graphics.hpp"
#include "ShaderManager.h"

class PostProcessor
{
public:
    void LoadShaders(ShaderManager* i_shaderMgr);
    void OnResize(unsigned int i_w, unsigned int i_h);
    void ApplyBloom(sf::RenderTexture& i_sceneRT, float i_threshold, float i_intensity);
private:
    sf::RenderTexture m_brightPass, m_blurH, m_blurV;
    sf::Shader* m_brightPassShader = nullptr;
    sf::Shader* m_blurShader = nullptr;
};