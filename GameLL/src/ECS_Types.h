#pragma once
using ComponentType = unsigned int; 
#define N_COMPONENT_TYPES 11

enum class Component {
	Position = 0, SpriteSheet, State, Movable, Controller,Collidable, SoundEmitter, SoundListener,
	LightSource, Particles, Interactable
};
enum class System {
	Renderer = 0, Movement,Collision, Control, State, SheetAnimation,Sound, Lighting, Particles
};