#pragma once
using ComponentType = unsigned int; 
#define N_COMPONENT_TYPES 9

enum class Component {
	Position = 0, SpriteSheet, State, Movable, Controller,Collidable, SoundEmitter, SoundListener,
	LightSource
};
enum class System {
	Renderer = 0, Movement,Collision, Control, State, SheetAnimation,Sound, Lighting
};