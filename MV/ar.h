#pragma once

#include "pattern.h"

namespace cg
{
	namespace pi
	{
		namespace ar
		{
			// Initializes AR Toolkit
			void init();
			void init(float *glTransformationMatrix);

			// Starts marker recognition
			void run();

			void lockOutput();
			void unlockOutput();
		}
	}
}