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
			/**
			 * pattern id, and source file name
			 * both should only be changed on constructor
			 */
			const string _filename;

			int _id;
			// holds the center position in reference to pattern coordinates
			// (0, 0) by default
			double _center[2];
			double _width;

		public:
			Pattern(const string filename);

			// Getters & Setters
			int id() const;
			string filename() const;
			double centerX() const;
			double centerY() const;
			double width() const;
			bool match(ARMarkerInfo marker_info) const;
		};
	}
}

#endif//___PATTERN_H___