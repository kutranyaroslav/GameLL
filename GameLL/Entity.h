#pragma once
#include <vector>
#include <algorithm>
#include "Map.h"

enum class EntityType {Base, Enemy, Player};
enum class EntityState{Idle, Walking, Jumping, Attacking, Hurt, Dying};

struct CollisionElement {
	CollisionElement(float i_area, TileInfo* i_info,const sf::FloatRect& i_bounds):
		m_area(i_area), m_tile(i_info), m_bounds(i_bounds)
	{}
	float m_area;
	TileInfo* m_tile;
	sf::FloatRect m_bounds;

};
using Collisions = std::vector<CollisionElement>;
class EntityManager;

class EntityBase
{
	friend class EntityManager;
public:
	EntityBase(EntityManager* i_entityMgr);
	virtual ~EntityBase();
	void Move(float i_x, float i_y);
	void AddVelocity(float i_x, float i_y);
	void Accelerate(float i_x, float i_y);
	void SetAcceleration(float i_x, float i_y);
	void ApplyFriction(float i_x, float i_y);
	virtual void Update(float i_dT);
	virtual void Draw(sf::RenderWindow* i_wind) = 0;
	// Setters and getters to be added later on
	void SetPosition(const float& i_x, const float& i_y);
	/*void SetPosition(const sf::Vector2f& i_pos);*/
	void SetSize(const float& i_x, const float& i_y);
	void SetState(const EntityState& i_state);
	std::string GetName();
	int GetId();
	EntityType GetType();
	EntityState GetState();
	sf::Vector2f GetPosition();
protected:
	void UpdateAABB();
	void CheckCollisions();
	void ResolveCollisions();
	bool SortCollisions(const CollisionElement& i_1, const CollisionElement& i_2);
	virtual void OnEntityCollision(EntityBase* i_collider, bool i_attack) = 0;

	//data members 
	std::string m_name;
	EntityType m_type;
	unsigned int m_id;
	sf::Vector2f m_position;
	sf::Vector2f m_positionOld;
	sf::Vector2f m_velocity;
	sf::Vector2f m_velocityMax;
	sf::Vector2f m_speed;
	sf::Vector2f m_acceleration;
	sf::Vector2f m_friction;
	TileInfo* m_referenceTile;
	sf::Vector2f m_size;
	sf::FloatRect m_AABB;
	EntityState m_state;
	bool m_collidingOnX;
	bool m_collidingOnY;
	Collisions m_collisions;
	EntityManager* m_entityManager;
};

using EntityContainer = std::unordered_map<unsigned int, EntityBase*>;
using EntityFactory = std::unordered_map < EntityType, std::function<EntityBase*(void)>>;
using EnemyTypes = std::unordered_map < std::string, std::string>;

class EntityManager {
public:
	EntityManager(SharedContext* i_context, unsigned int i_maxEntities);
	~EntityManager();
	int Add(const EntityType& i_type, const std::string& i_name = "");
	EntityBase* Find(unsigned int i_id);
	EntityBase* Find(const std::string& i_name);
	void Remove(unsigned int i_id);
	void Update(float i_dT);
	void Draw();

	void Purge();
	SharedContext* GetContext();
private:
	template<class T>
	void RegisterEntity(const EntityType& i_type) {
		m_entityFactory[i_type] = [this]()->EntityBase* {
			return new T(this);
		};
	}

	void ProcessRemovals();
	void LoadEnemyTypes(const std::string& i_name);
	void EntityCollisionCheck();

	EntityContainer m_entities;
	EnemyTypes m_enemyTypes;
	EntityFactory m_entityFactory;
	SharedContext* m_context;
	unsigned int  m_idCounter;
	unsigned int m_maxEntities;
	std::vector<unsigned int> m_entitiesToRemove;

};