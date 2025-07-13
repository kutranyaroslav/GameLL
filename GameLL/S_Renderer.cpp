#include "S_Renderer.h"
S_Renderer::S_Renderer(SystemManager* i_systemMgr) :
	S_Base(System::Renderer, i_systemMgr) {
	Bitmask req; 
	req.turnOnBit((unsigned int)Component::Position);
	req.turnOnBit((unsigned int)Component::SpriteSheet);
	m_requiredComponents.push_back(req);
	req.Clear();
	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::Direction_Changed,this);
}
S_Renderer::~S_Renderer(){}

void S_Renderer::Update(float i_dT) {
	EntityManagerNew* entities  = m_systemMgr->GetEntityManager();
	for (auto& entity : m_entities) {
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);

		C_Drawable* drawable = nullptr;
		if (entities->HasComponent(entity, Component::SpriteSheet)) {
			drawable = entities->GetComponent<C_Drawable>(entity, Component::SpriteSheet);
		}
		else { continue; }
		drawable->UpdatePosition(position->GetPosition());
	}
}
void S_Renderer::HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) {
	if (i_event == EntityEvent::Moving_Left || i_event == EntityEvent::Moving_Right
		|| i_event == EntityEvent::Moving_Up || i_event == EntityEvent::Moving_Down ||
		i_event == EntityEvent::Elevation_Change || i_event == EntityEvent::Spawned
		)
	{
		SortDrawables();
	}
}

void S_Renderer::Notify(const Message& i_message) {
	if (HasEntity(i_message.m_receiver)) {
		EntityMessage m = (EntityMessage)i_message.m_type;
		switch (m) {
		case EntityMessage::Direction_Changed:
			SetSheetDirection(i_message.m_receiver, (Direction)i_message.m_int);
			break;
		}
	}
}
void S_Renderer::Render(Window* i_wind, unsigned int i_layer) {
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	for (auto& entity : m_entities) {
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);
		if (position->getElevation() < i_layer) { continue;}
		if (position->getElevation() > i_layer) { break; }
		C_Drawable* drawable = nullptr;
		if (!entities->HasComponent(entity, Component::SpriteSheet)) {
			continue;
		}
		drawable = entities->GetComponent<C_Drawable>(entity, Component::SpriteSheet);
		sf::FloatRect drawableBounds;
		drawableBounds.left = position->GetPosition().x - (drawable->GetSize().x / 2);
		drawableBounds.top = position->GetPosition().y - drawable->GetSize().y;
		drawableBounds.width = drawable->GetSize().x;
		drawableBounds.height = drawable->GetSize().y;
		if (!i_wind->GetViewSpace().intersects(drawableBounds)) {
			continue;
		}
		drawable->Draw(i_wind->GetRenderWindow());
	}
}
void S_Renderer::SetSheetDirection(const EntityId& i_entity, const Direction& i_dir ) {
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	if (!entities->HasComponent(i_entity, Component::SpriteSheet)) {
		return;
	}
	C_SpriteSheet* sheet = entities->GetComponent<C_SpriteSheet>(i_entity, Component::SpriteSheet);
	sheet->GetSpriteSheet()->SetSpriteDir(i_dir);
}

void S_Renderer::SortDrawables() {
	EntityManagerNew* entityMgr = m_systemMgr->GetEntityManager();
	std::sort(m_entities.begin(), m_entities.end(), [entityMgr](unsigned int i_1, unsigned int i_2) {
		auto pos1 = entityMgr->GetComponent<C_Position>(i_1, Component::Position);
		auto pos2 = entityMgr->GetComponent<C_Position>(i_2, Component::Position);
		if (pos1->getElevation() == pos2->getElevation()) {
			return pos1->GetPosition().y < pos2->GetPosition().y;
		}
		return pos1->getElevation() < pos2->getElevation();
		});
}