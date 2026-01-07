#pragma once 
#include <sstream>
#include "Anim_Base.h"


class Anim_Directional:public Anim_Base
{
protected:
	void FrameStep();
	void CropSprite();
	void ReadIn(std::stringstream& i_stream);
};

