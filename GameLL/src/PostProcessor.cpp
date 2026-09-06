#include "PostProcessor.h"

void PostProcessor::LoadShaders(ShaderManager* i_shaderMgr) {
    if (!i_shaderMgr) { return; }
    if (i_shaderMgr->RequireResource("brightpass")) {
        m_brightPassShader = i_shaderMgr->GetResource("brightpass");
    }
    if (i_shaderMgr->RequireResource("blur")) {
        m_blurShader = i_shaderMgr->GetResource("blur");
    }
}

void PostProcessor::OnResize(unsigned int i_w, unsigned int i_h) {
    unsigned int w = std::max(1u, i_w / 2), h = std::max(1u, i_h / 2); // половинное разрешение
    m_brightPass.create(w, h);
    m_blurH.create(w, h);
    m_blurV.create(w, h);
}

void PostProcessor::ApplyBloom(sf::RenderTexture& i_sceneRT, float i_threshold, float i_intensity) {
    if (!m_brightPassShader || !m_blurShader) { return; }

    sf::Sprite sceneSprite(i_sceneRT.getTexture());
    sceneSprite.setScale(
        (float)m_brightPass.getSize().x / i_sceneRT.getSize().x,
        (float)m_brightPass.getSize().y / i_sceneRT.getSize().y);
    m_brightPassShader->setUniform("texture", sf::Shader::CurrentTexture);
    m_brightPassShader->setUniform("threshold", i_threshold);
    m_brightPass.clear(sf::Color::Black);
    m_brightPass.draw(sceneSprite, m_brightPassShader);
    m_brightPass.display();

    m_blurShader->setUniform("texture", sf::Shader::CurrentTexture);
    m_blurShader->setUniform("direction", sf::Glsl::Vec2(1.f / m_blurH.getSize().x, 0.f));
    m_blurH.clear(sf::Color::Black);
    m_blurH.draw(sf::Sprite(m_brightPass.getTexture()), m_blurShader);
    m_blurH.display();

    m_blurShader->setUniform("texture", sf::Shader::CurrentTexture);
    m_blurShader->setUniform("direction", sf::Glsl::Vec2(0.f, 1.f / m_blurV.getSize().y));
    m_blurV.clear(sf::Color::Black);
    m_blurV.draw(sf::Sprite(m_blurH.getTexture()), m_blurShader);
    m_blurV.display();

    sf::Sprite bloomSprite(m_blurV.getTexture());
    bloomSprite.setScale(
        (float)i_sceneRT.getSize().x / m_blurV.getSize().x,
        (float)i_sceneRT.getSize().y / m_blurV.getSize().y);
    bloomSprite.setColor(sf::Color(255, 255, 255, (sf::Uint8)(255 * i_intensity)));
    i_sceneRT.draw(bloomSprite, sf::RenderStates(sf::BlendAdd));
    i_sceneRT.display();
}