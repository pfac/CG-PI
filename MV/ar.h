#pragma once

#include "pattern.h"

namespace cg
{
	namespace pi
	{
		namespace ar
		{
			//!	Initialize the ARToolKit module
			/*!
				Initializes the ARToolKit library and prepares the capture system.

				Also sets the transformation matrix to be modified when the pattern is detected.

				/param	glTransformationMatrix	Pointer to the shared transformation matrix.
			*/
			void init(float *glTransformationMatrix);

			//!	Starts the application main loop
			/*!
				Calls the main loop of the GLUT library. The ARToolKit capture and recognize loop runs in a GLUT window.

				Every time a pattern is recognized, the shared transformation matrix is set to reflect the pattern positioning, ignoring any translation.

				When no pattern is found, the matrix is set to be the identity.
			*/
			void run();

			//!	Ask for access to the output data.
			/*!
				Asks for access to the shared transformation matrix, to avoid race conditions.
			*/
			void lockOutput();

			//!	Dismiss the access to the output data.
			/*!
				Releases the shared transformation matrix, avoiding deadlocks.
			*/
			void unlockOutput();
		}
	}
}