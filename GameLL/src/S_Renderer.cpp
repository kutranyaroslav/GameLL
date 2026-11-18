#include "S_Renderer.h"
#include "C_Interactable.h"
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
// Existing gameplay calls continue working.
void S_Renderer::Render(Window* window, unsigned int layer) {
	if (!window) {
		return;
	}

	sf::RenderTexture* scene = window->GetSceneTexture();
	Render(*scene, scene->getView(), layer);
}

void S_Renderer::Render(
	sf::RenderTarget& target,
	const sf::View& view,
	unsigned int layer
) {
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	if (!entities) {
		return;
	}

	const sf::Vector2f center = view.getCenter();
	const sf::Vector2f size = view.getSize();

	const sf::FloatRect visible(
		center.x - size.x * 0.5f,
		center.y - size.y * 0.5f,
		size.x,
		size.y
	);

	const sf::View previousView = target.getView();
	target.setView(view);

	for (EntityId entity : m_entities) {
		auto* position = entities->GetComponent<C_Position>(
			entity, Component::Position);

		auto* component = entities->GetComponent<C_SpriteSheet>(
			entity, Component::SpriteSheet);

		if (!position || !component) {
			continue;
		}

		SpriteSheet* sheet = component->GetSpriteSheet();
		if (!sheet) {
			continue;
		}

		// Actual bounds include the sprite's origin, scale and rotation.
		if (!visible.intersects(sheet->GetSprite()->getGlobalBounds())) {
			continue;
		}

		bool useParts = true;

		auto* interaction = entities->GetComponent<C_Interactable>(
			entity, Component::Interactable);

		if (interaction &&
			interaction->GetType() == InteractionType::Door) {
			useParts = !interaction->GetDoorOpened();
			}
		sheet->DrawOnLayer(
			target,
			position->getElevation(),
			layer,
			useParts
		);
	}

	target.setView(previousView);
}

void S_Renderer::LoadMaterial(const std::string& i_materialName)
{
}
void S_Renderer::LoadMaterials()
{
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