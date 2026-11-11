#pragma once
using ComponentType = unsigned int; 
#define N_COMPONENT_TYPES 13

enum class Component {
	Position = 0, SpriteSheet, State, Movable, Controller,Collidable, SoundEmitter, SoundListener,
	LightSource, Particles,/*made only for actors for now >*/ Interactable,Inventory,Type
};
enum class System {
	Renderer = 0, Movement,Collision, Control, State, SheetAnimation,Sound, Lighting, Particles,
	Interaction,
};