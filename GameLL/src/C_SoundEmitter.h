#pragma once
#include "C_Base.h"
#include "SoundProps.h"
class C_SoundEmitter: public C_Base
{
public:
	static const int Max_EntitySounds = 4;
	C_SoundEmitter();
	const std::string& GetSound(const EntitySound& i_sound);
	bool IsSoundFrame(const EntitySound& i_sound, int i_frame);
	SoundId GetSoundId();
	void SetSoundId(const SoundId& i_sound);
	SoundParameters* GetSoundParamters();
	void ReadIn(std::stringstream& i_stream) override;
private:
	SoundId m_soundId;
	std::array<SoundParameters, Max_EntitySounds> m_params;
};

