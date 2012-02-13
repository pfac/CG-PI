#pragma once

#include "pattern.h"
#include "player.h"

namespace cg
{
	namespace pi
	{
		namespace ar
		{
			// Initializes AR Toolkit
			void init(Player *player);

			// Starts marker recognition
			void run();
		}
	}
}