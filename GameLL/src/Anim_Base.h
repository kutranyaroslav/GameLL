#pragma once
#include <sstream>
#include "SFML/Graphics.hpp"
class SpriteSheet;

using Frame = unsigned int;

class Anim_Base
{
	friend class SpriteSheet;
public:
	Anim_Base();
	virtual ~Anim_Base();
	bool IsInAction();
	bool IsPlaying();
	bool CheckMoved();
	void Play();
	void Pause();
	void Reset();
	void Stop();
	virtual void Update(const sf::Time& i_dT);
	friend std::stringstream& operator >>(std::stringstream& i_stream,Anim_Base& a){
		a.ReadIn(i_stream);
		return i_stream;
	}

	void SetLooping(const bool& i_loop);
	void SetSpriteSheet(SpriteSheet* i_spriteSheet);
	bool SetFrame(const unsigned int& i_frame, const unsigned int& i_endFrame = 0);
	void SetName(const std::string& i_name);
	std::string GetName();
	Frame GetCurrentFrame();
protected:
	virtual void FrameStep() = 0;
	virtual void CropSprite() = 0;
	virtual void ReadIn(std::stringstream& i_stream) = 0;
	Frame m_currentFrame;
	Frame m_startFrame;
	Frame m_endFrameEast;
	Frame m_endFrameWest;
	Frame m_endFrameNorth;
	Frame m_endFrameSouth;
	Frame m_rowFrame;
	int m_frameActionStart;
	int m_frameActionEnd;
	float m_frameTime;
	float m_elapsedTime;
	bool m_loop;
	bool m_playing;
	bool m_hasMoved;
	std::string m_name;
	SpriteSheet* m_spriteSheet;
};

