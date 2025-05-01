#include "S_Collision.h"
S_Collision::S_Collision(SystemManager* i_systemMgr) :S_Base(System::Collision, i_systemMgr) {
	Bitmask req; 
	req.turnOnBit((unsigned int)Component::Collidable);
	req.turnOnBit((unsigned int)Component::Position);
	m_requiredComponents.push_back(req);
	req.Clear();
	m_gameMap = nullptr;
}

Map* S_Collision::GetMap() { return m_gameMap; }
void S_Collision::SetMap(Map* i_map) { m_gameMap = i_map; }

void S_Collision::Update(float i_dT){
	if (!m_gameMap) { return; }
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	for (auto& entity : m_entities) {
		C_Position* pos = entities->GetComponent<C_Position>(entity, Component::Position);
		C_Collidable* collidable = entities->GetComponent<C_Collidable>(entity, Component::Collidable);
		collidable->SetPosition(pos->GetPosition());
		collidable->ResetCollisionFlags();
		CheckOutOfBounds(pos, collidable);
		MapCollisions(entity, pos, collidable);
	}
	EntityCollisions();
}

void S_Collision::CheckOutOfBounds(C_Position* i_pos, C_Collidable* i_collidable) {
	unsigned int TileSize = m_gameMap->GetTileSize();
	if (i_pos->GetPosition().x < 0) {
		i_pos->SetPosition(sf::Vector2f(0.f, i_pos->GetPosition().y));
		i_collidable->SetPosition(i_pos->GetPosition());
	}
	else if (i_pos->GetPosition().x > m_gameMap->GetMapSize().x * TileSize) {
		i_pos->SetPosition(sf::Vector2f(m_gameMap->GetMapSize().x * TileSize, i_pos->GetPosition().y));
		i_collidable->SetPosition(i_pos->GetPosition());
	}

	if (i_pos->GetPosition().y < 0) {
		i_pos->SetPosition(sf::Vector2f(i_pos->GetPosition().x, 0.f));
		i_collidable->SetPosition(i_pos->GetPosition());
	}
	else if (i_pos->GetPosition().y > m_gameMap->GetMapSize().y * TileSize) {
		i_pos->SetPosition(sf::Vector2f(i_pos->GetPosition().x, m_gameMap->GetMapSize().y * TileSize));
		i_collidable->SetPosition(i_pos->GetPosition());
	}
}

void S_Collision::MapCollisions(const EntityId& i_entity, C_Position* i_pos, C_Collidable* i_col) {
	unsigned int TileSize = m_gameMap->GetTileSize();
	Collisions c; 
	sf::FloatRect EntityAABB = i_col->GetCollidable();
	int fromX = floor(EntityAABB.left / TileSize);
	int fromY = floor(EntityAABB.top / TileSize);
	int toX = ceil((EntityAABB.left + EntityAABB.width) / TileSize);
	int toY = ceil((EntityAABB.top + EntityAABB.height) / TileSize);

	for (int x = fromX; x <= toX; x++) {
		for (int y = fromY; y <= toY; y++) {
			for (int l = 0; l < Sheet::Num_Layers; l++) {
				Tile* t = m_gameMap->GetTile(x, y, l);
				if (!t) { continue; }
				if (!t->m_solid) { continue; }
				sf::FloatRect TileAABB(x * TileSize, y * TileSize, TileSize, TileSize);
				sf::FloatRect intersection;
				EntityAABB.intersects(TileAABB,intersection);
				float S = intersection.width * intersection.height;
				c.emplace_back(S, t->m_properties, TileAABB);
				break;
			}
		}

	}
	if (c.empty()) { return; }
	std::sort(c.begin(), c.end(), [](CollisionElement& i_1, CollisionElement& i_2) {
		return i_1.m_area > i_2.m_area;
		});

	for (auto& col : c) {
		EntityAABB = i_col->GetCollidable();
		if (!EntityAABB.intersects(col.m_tileBounds)) { continue; }
		float xDiff = ((EntityAABB.left + (EntityAABB.width / 2)) - (col.m_tileBounds.left + (col.m_tileBounds.width / 2)));
		float yDiff = ((EntityAABB.top + (EntityAABB.height / 2)) - (col.m_tileBounds.top + (col.m_tileBounds.height / 2)));
		float resolve = 0;
		if (std::abs(xDiff) > std::abs(yDiff)) {
			if (xDiff > 0) {
				resolve = (col.m_tileBounds.left + TileSize) - EntityAABB.left;
			}
			else {
				resolve = (EntityAABB.left + EntityAABB.width) - col.m_tileBounds.left;
			}
			i_pos->MoveBy(resolve, 0);
			i_col->SetPosition(i_pos->GetPosition());
			m_systemMgr->AddEvent(i_entity, (EventId)EntityEvent::Colliding_X);
			i_col->CollideOnX();
		}
		else {
			if (yDiff > 0) {
				resolve = (col.m_tileBounds.top + TileSize) - EntityAABB.top;
			}
			else {
				resolve = (EntityAABB.top + EntityAABB.height) - col.m_tileBounds.top;
				
			}
			i_pos->MoveBy(0, resolve);
			i_col->SetPosition(i_pos->GetPosition());
			m_systemMgr->AddEvent(i_entity, (EventId)EntityEvent::Colliding_Y);
			i_col->CollideOnY();
		}
	}
}

void S_Collision::EntityCollisions() {
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	for (auto itr = m_entities.begin(); itr != m_entities.end(); ++itr) {
		for (auto itr2 = std::next(itr); itr2 != m_entities.end(); ++itr2) {
			C_Collidable* collidable1 = entities->GetComponent<C_Collidable>(*itr, Component::Collidable);
			C_Collidable* collidable2 = entities->GetComponent<C_Collidable>(*itr2, Component::Collidable);
			if (collidable1->GetCollidable().intersects(collidable2->GetCollidable())) {
				//collision of entities
			}
		}
	}
}
void S_Collision::Notify(const Message& i_message) {
	return;
}
void S_Collision::HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) {
	return;
}