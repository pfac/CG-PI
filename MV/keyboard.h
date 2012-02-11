#pragma once
#ifndef ___KEYBOARD_H___
#define ___KEYBOARD_H___

#include "player.h"

namespace cg
{
	namespace keyboard
	{
		void init();
		void init(Player *p);

		void keyDown(unsigned char key, int xx, int yy);
		void keyUp(unsigned char key, int xx, int yy);
	}
}

#endif//___KEYBOARD_H___