#include "C_SoundEmitter.h"


C_SoundEmitter::C_SoundEmitter() : C_Base(Component::SoundEmitter), m_soundId(-1) {}

const std::string& C_SoundEmitter::GetSound(const EntitySound& i_sound){
	static std::string empty = "";
	return((int)i_sound < Max_EntitySounds ? m_params[(int)i_sound].m_sound : empty);
}

bool C_SoundEmitter::IsSoundFrame(const EntitySound& i_sound, int i_frame) {
	if ((int)i_sound >= Max_EntitySounds) { return false; }
	if ((int)i_sound == -1) { return false; }
	for (int i = 0; i < SoundParameters::Max_SoundFrames; ++i) {
		if (m_params[(int)i_sound].m_frames[i] == -1) { return false; }
		if (m_params[(int)i_sound].m_frames[i] == i_frame) { return true; }
	}
	return false;
}

SoundId C_SoundEmitter::GetSoundId() { return m_soundId; }
void C_SoundEmitter::SetSoundId(const SoundId& i_sound) { m_soundId = i_sound; }
SoundParameters* C_SoundEmitter::GetSoundParamters() { return &m_params[0]; }

void C_SoundEmitter::ReadIn(std::stringstream& i_stream) {
	for (auto& parameters : m_params) {
		parameters = SoundParameters{};
	}

	std::string chunk;

	for (int i = 0; i < Max_EntitySounds && (i_stream >> chunk); ++i) {
		const size_t separator = chunk.find(':');
		if (separator == std::string::npos) {
			continue;
		}

		const std::string binding = chunk.substr(0, separator);
		const std::string frames = chunk.substr(separator + 1);

		int soundIndex = i;
		std::string sound = binding;

		const size_t equals = binding.find('=');

		if (equals != std::string::npos) {
			const std::string action = binding.substr(0, equals);
			sound = binding.substr(equals + 1);

			if (action == "Door_Open") {
				soundIndex = static_cast<int>(EntitySound::Door_Open);
			}
			else if (action == "Door_Close") {
				soundIndex = static_cast<int>(EntitySound::Door_Close);
			}
			else {
				continue; // Unknown action.
			}
		}

		if (soundIndex < 0 || soundIndex >= Max_EntitySounds ||
			sound.empty()) {
			continue;
			}

		SoundParameters& parameters = m_params[soundIndex];
		parameters = SoundParameters{};
		parameters.m_sound = sound;

		// Keep your existing frame-parsing loop below this.
	}
}