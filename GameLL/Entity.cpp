#include "Entity.h"
#include "Player.h"
#include "Enemy.h"

EntityBase::EntityBase(EntityManager* i_entityMgr) :
	m_entityManager(i_entityMgr), m_name("BaseEntity"),
	m_type(EntityType::Base), m_referenceTile(nullptr),
	m_state(EntityState::Idle),m_id(0),m_friction(0.f, 0.8f), m_collidingOnX(false), m_collidingOnY(false)
{

}
EntityBase::~EntityBase(){}
void EntityBase::SetAcceleration(float i_x, float i_y){
	m_acceleration = sf::Vector2f(i_x, i_y);
}
void EntityBase::SetPosition(const float& i_x, const float& i_y){
	m_position = sf::Vector2f(i_x, i_y);
	UpdateAABB();
}
//void EntityBase::SetPosition(const sf::Vector2f& i_pos) {
//	m_position = i_pos;
//	UpdateAABB();
//}

void EntityBase::SetSize(const float& i_x, const float& i_y) {
	m_size = sf::Vector2f(i_x, i_y);
	UpdateAABB();
}
void EntityBase::SetState(const EntityState& i_state) {
	if (m_state == EntityState::Dying) { return; }
	m_state = i_state;
}

void EntityBase::Move(float i_x, float i_y) {
	m_positionOld = m_position;
	if(!m_collidingOnY){
		m_position += sf::Vector2f(i_x, i_y);
	}
	sf::Vector2u mapSize = m_entityManager->GetContext()->m_gameMap->GetMapSize();
	if (m_position.x < 0) {
		m_position.x = 0;
	}
	else if (m_position.x > (mapSize.x + 1) * Sheet::Tile_Size) {
		m_position.x = (mapSize.x + 1) * Sheet::Tile_Size;
	}
	if (m_position.y< 0) {
		m_position.y = 0;
	}
	else if (m_position.y > (mapSize.y + 1) * Sheet::Tile_Size) {
		m_position.y = (mapSize.y + 1) * Sheet::Tile_Size;
	//	SetState(EntityState::Dying);
	}
	UpdateAABB();
}

void EntityBase::AddVelocity(float i_x, float i_y) {
	m_velocity += sf::Vector2f(i_x, i_y);
	if (abs(m_velocity.x) > m_velocityMax.x) {
		if (m_velocity.x < 0) { m_velocity.x = -m_velocityMax.x; }
		else { m_velocity.x = m_velocityMax.x; }
	}
	if (abs(m_velocity.y) > m_velocityMax.y) {
		if (m_velocity.y < 0) { m_velocity.y = -m_velocityMax.y; }
		else { m_velocity.y = m_velocityMax.y; }
	}
}

void EntityBase::Accelerate(float i_x, float i_y) {
	m_acceleration += sf::Vector2f(i_x, i_y);
}

void EntityBase::ApplyFriction(float i_x, float i_y) {
	if (m_velocity.x != 0) {
		if (abs(m_velocity.x) - abs(i_x) < 0) { m_velocity.x = 0; }
		else {
			if (m_velocity.x < 0) { m_velocity.x += i_x; }
			else { m_velocity.x -= i_x; }
		}
	}
	
	if (m_velocity.y != 0) {
		if (abs(m_velocity.y) - abs(i_y) < 0) { m_velocity.y = 0; }
		else {
			if (m_velocity.y < 0) { m_velocity.y += i_y; }
			else { m_velocity.y -= i_y; }
		}
	}
}

void EntityBase::Update(float i_dT) {
	Map* map = m_entityManager->GetContext()->m_gameMap;
	float gravity = map->GetGravity();
	if (!m_collidingOnY) {
		Accelerate(0, gravity);
	}
	else {
		Accelerate(0, 0);
	}
	AddVelocity(m_acceleration.x * i_dT, m_acceleration.y * i_dT);
	SetAcceleration(0.f, 0.f);
	sf::Vector2f frictionValue;
	if (m_referenceTile) {
		frictionValue = m_referenceTile->m_friction;
		if (m_referenceTile->m_deadly) { SetState(EntityState::Dying); }
	}
	else if (map->GetDefaultTile()) {
		frictionValue = map->GetDefaultTile()->m_friction;
	}
	else {
		frictionValue = m_friction;
	}
	float friction_x = (m_speed.x * frictionValue.x) * i_dT;
	float friction_y = (m_speed.y * frictionValue.y) * i_dT;
	ApplyFriction(friction_x, friction_y);
	sf::Vector2f deltaPos = m_velocity * i_dT;
 	Move(deltaPos.x, deltaPos.y);
	m_collidingOnX = false;
	m_collidingOnY = false;
	CheckCollisions();
	ResolveCollisions();
}

void EntityBase::UpdateAABB() {
	m_AABB = sf::FloatRect(m_position.x - (m_size.x / 2), m_position.y - m_size.y, m_size.x, m_size.y);
}

bool EntityBase::SortCollisions(const CollisionElement& i_1, const CollisionElement& i_2) {
	return i_1.m_area > i_2.m_area;
}

void EntityBase::CheckCollisions() {
	Map* gameMap = m_entityManager->GetContext()->m_gameMap;
	unsigned int tileSize = gameMap->GetTileSize();
	int fromX = floor(m_AABB.left / tileSize);
	int toX = floor(m_AABB.left + m_AABB.width / tileSize);
	int fromY = floor(m_AABB.top / tileSize);
	int toY = floor(m_AABB.top + m_AABB.height / tileSize);

	for (int x = fromX; x <= toX; ++x) {
		for (int y = fromY; y <= toY; ++y) {
			Tile* tile = gameMap->GetTile(x, y);
			if (!tile) { continue; }
			sf::FloatRect tileBounds(x * tileSize, y * tileSize, tileSize, tileSize);
			sf::FloatRect intersection;
			m_AABB.intersects(tileBounds, intersection);
			if (intersection.width != 0.f || intersection.height != 0) {
				std::cout << "hello" << std::endl;
			}
			float area = intersection.height * intersection.width;
			CollisionElement e(area, tile->m_properties, tileBounds);
			m_collisions.emplace_back(e);
			if (tile->m_warp && m_type == EntityType::Player) {
				gameMap->LoadNext();
			}
		}
	}
}


void EntityBase::ResolveCollisions() {
	if (!m_collisions.empty()) {
		auto cmp = [](CollisionElement a, CollisionElement b) { return a.m_area > b.m_area;};
		std::sort(m_collisions.begin(), m_collisions.end(),cmp);
		Map* gameMap = m_entityManager->GetContext()->m_gameMap;
		unsigned int tileSize = gameMap->GetTileSize();
		for (auto& itr : m_collisions) {
			if (!m_AABB.intersects(itr.m_bounds)) { continue; }
			float xDiff = (m_AABB.left + (m_AABB.width / 2)) - (itr.m_bounds.left + (itr.m_bounds.width / 2));
			float yDiff = (m_AABB.top + (m_AABB.height / 2)) - (itr.m_bounds.top + (itr.m_bounds.height / 2));
			float resolve = 0;
			if (abs(xDiff) > abs(yDiff)) {
				if (xDiff > 0) {
					resolve = (itr.m_bounds.left + tileSize) - m_AABB.left;
				}
				else {
					resolve = -((m_AABB.left + m_AABB.width) - itr.m_bounds.left);
				}
				Move(resolve, 0);
				m_velocity.x = 0;
				m_collidingOnX = true;
			}
			else {
				if (yDiff > 0) {
					resolve = (itr.m_bounds.top + tileSize) - m_AABB.top;
				}
				else {
					resolve = -((m_AABB.top + m_AABB.height) - itr.m_bounds.top);
				}
				Move(0, resolve);
				m_velocity.y = 0;
				if (m_collidingOnY) { continue; }
				m_referenceTile = itr.m_tile;
				m_collidingOnY = true;
				
			}
		}
		m_collisions.clear();
	}
	if (!m_collidingOnY) { m_referenceTile = nullptr; }
}
std::string EntityBase::GetName() { return m_name; }
int EntityBase::GetId() { return m_id; }
EntityType EntityBase::GetType() { return m_type; }
EntityState EntityBase::GetState() { return m_state; }
sf::Vector2f EntityBase::GetPosition() { return m_position; }

EntityManager::EntityManager(SharedContext* i_context, unsigned int i_maxEntities) :
	m_context(i_context), m_maxEntities(i_maxEntities), m_idCounter(0) {
	LoadEnemyTypes("Entitylist.list");
	RegisterEntity<Player>(EntityType::Player);
	RegisterEntity<Enemy>(EntityType::Enemy);
}
EntityManager::~EntityManager() { Purge(); }

int EntityManager::Add(const EntityType& i_type, const std::string& i_name){
	auto itr = m_entityFactory.find(i_type);
	if (itr == m_entityFactory.end()) { return -1; }
	EntityBase* entity = itr->second();
	entity->m_id = m_idCounter;
	if (i_name != "") { entity->m_name = i_name; }
	m_entities.emplace(m_idCounter, entity);
	if (i_type == EntityType::Enemy) {
		auto itr = m_enemyTypes.find(i_name);
		if (itr != m_enemyTypes.end()) {
			Enemy* enemy = (Enemy*)entity;
			enemy->Load(itr->second);
		}
	}
	++m_idCounter;
	return m_idCounter - 1;
}

EntityBase* EntityManager::Find(const std::string& i_name) {
	for (auto& itr : m_entities) {
		if (itr.second->GetName() == i_name) {
			return itr.second;
		}
	}
	return nullptr;
}
EntityBase* EntityManager::Find(unsigned int i_id) {
	auto itr = m_entities.find(i_id);
	if (itr != m_entities.end()) { return itr->second; }
	return nullptr;
}

void EntityManager::Remove(unsigned int i_id) {
	m_entitiesToRemove.emplace_back(i_id);
}

void EntityManager::Update(float i_dT) {
	for (auto& itr : m_entities) {
		itr.second->Update(i_dT);
	}
	EntityCollisionCheck();
	ProcessRemovals();
}

void EntityManager::Draw() {
	sf::RenderWindow* wnd = m_context->m_wind->GetRenderWindow();
	sf::FloatRect viewSpace = m_context->m_wind->GetViewSpace();

	for (auto& itr : m_entities) {
		if (!viewSpace.intersects(itr.second->m_AABB)) { continue; }
		itr.second->Draw(wnd);	
	}
}

void EntityManager::Purge() {
	for (auto& itr : m_entities) {
		delete itr.second;
	}
	m_entities.clear();
	m_idCounter = 0;
}

void EntityManager::ProcessRemovals() {
	while (m_entitiesToRemove.begin() != m_entitiesToRemove.end()) {
		unsigned int id = m_entitiesToRemove.back();
		auto itr = m_entities.find(id);
		if (itr != m_entities.end()) {
			std::cout << "Discarding entity: " << itr->second->GetId() << std::endl;
			delete itr->second;
			m_entities.erase(itr);
		}
		m_entitiesToRemove.pop_back();
	}
}

void EntityManager::EntityCollisionCheck() {
	if (m_entities.empty()) { return;}
	for (auto itr = m_entities.begin();std::next(itr) != m_entities.end(); ++itr) {
		for (auto itr2 = std::next(itr); itr2 != m_entities.end(); ++itr) {
			if (itr->first == itr2->first) { continue; }
			if (itr->second->m_AABB.intersects(itr2->second->m_AABB)) {
				itr->second->OnEntityCollision(itr2->second,false);
				itr2->second->OnEntityCollision(itr->second,false);
			}
			EntityType t1 = itr->second->GetType();
			EntityType t2 = itr2->second->GetType();

			if (t1 == EntityType::Enemy || t1 == EntityType::Player) {
				Character* c1 = (Character*)itr->second;
				if (c1->m_attackAABB.intersects(itr2->second->m_AABB)) {
					c1->OnEntityCollision(itr2->second, true);
				}
			}
			if (t2 == EntityType::Enemy || t2 == EntityType::Player) {
				Character* c2 = (Character*)itr2->second;
				if (c2->m_attackAABB.intersects(itr->second->m_AABB)) {
					c2->OnEntityCollision(itr->second, true);
				}
			}
		}
	}
}

void EntityManager::LoadEnemyTypes(const std::string& i_name) {
	std::ifstream file; 
	file.open(Utils::GetWorkingDirectory() + i_name);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			if (line[0] == '|') { continue; }
			std::stringstream keystream(line);
			std::string name; 
			std::string charfile;
			keystream >> name >> charfile;
			m_enemyTypes.emplace(name, charfile);
		}
		file.close();
	}
}

SharedContext* EntityManager::GetContext() { return m_context; }