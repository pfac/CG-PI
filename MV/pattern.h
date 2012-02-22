#pragma once
#ifndef ___PATTERN_H___
#define ___PATTERN_H___

#include <AR/ar.h>
#include <iostream>
using namespace std;


namespace cg
{
	namespace pi
	{
		class Pattern
		{
		private:
			const string _filename;	//!	Name of the pattern's source file.

			int _id;				//!	Pattern identification. Used to distinguish between multiple patterns.
			double _center[2];		//!	Coordinates of the pattern's center.
			double _width;			//!	Real width of the pattern.

		public:
			//!	Default constructor.
			/*!
				Loads the pattern to be recognized. If the pattern can not be loaded, aborts the program execution.
				
				/param	filename	Name of the pattern's source file.
			*/
			Pattern(const string filename);

			// Getters & Setters
			//!	Get the pattern identification.
			/*!
				/return	The pattern identification number.
			*/
			int id() const;

			//!	Get the pattern filename
			/*!
				/return	The name of the pattern's source file.
			*/
			string filename() const;

			//!	Get the first coordinate of the pattern's center.
			/*!
				/return	The x component of the pattern's center.
			*/
			double centerX() const;

			//!	Get the second coordinate of the pattern's center.
			/*!
				/return	The y component of the pattern's center.
			*/
			double centerY() const;

			//!	Get the real width.
			/*!
				/return	The real width of the pattern.
			*/
			double width() const;
		};
	}
}

#endif//___PATTERN_H___