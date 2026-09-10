
enum class EntityEvent {
	Spawned, Despawned, Colliding_X, Colliding_Y,
	Moving_Left, Moving_Right, Moving_Up, Moving_Down,
	Elevation_Change, Became_Idle, Began_Moving, Door_Close,
	Door_Open, Healed, Died, Attacked, Interacted, Uninteracted,
	// one per InteractionType added below Door/AidKit (Healed already covers AidKit)
	Item_Picked, Note_Read, Lever_Toggled, Examined, Game_Saved, Terminal_Used, Map_Changed
};