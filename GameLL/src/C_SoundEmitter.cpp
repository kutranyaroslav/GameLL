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
	std::string main_delimiter = ":";
	std::string frame_delimiter = ",";
	for (int i = 0; i < Max_EntitySounds; ++i) {
		std::string chunk;
		i_stream >> chunk;
		if (chunk == "") { break; }
		std::string sound;
		sound = chunk.substr(0, chunk.find(main_delimiter));
		std::string frames = chunk.substr(chunk.find(main_delimiter)+ main_delimiter.length());
		m_params[i].m_sound = sound;
		size_t pos = 0;
		unsigned int FrameNum = 0;
		while (FrameNum < SoundParameters::Max_SoundFrames) {
			pos = frames.find(frame_delimiter);
			int frame = -1;
			if (pos != std::string::npos) {
				frame = std::stoi(frames.substr(0, pos));
				frames.erase(0, pos + frame_delimiter.length());
			}
			else {
				frame = std::stoi(frames);
				m_params[i].m_frames[FrameNum] = frame;
				break;
			}
			m_params[i].m_frames[FrameNum] = frame;
			++FrameNum;
 }
	}
}