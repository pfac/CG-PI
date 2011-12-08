#pragma once

#include <AR/ar.h>
#include <iostream>
using namespace std;


namespace CGLibs {

	class Pattern {
	private:
		/**
		 * pattern id, and source file name
		 * both should only be changed on constructor
		 */
		int id;
		string src_file;

		// holds the center position in reference to pattern coordinates
		// (0, 0) by default
		double center[2];
		double width;

	public:
		Pattern(string src_file);

		// Getters & Setters
		int getId();
		string getName();
		double* getCenter();
		double getWidth();
		bool match(ARMarkerInfo marker_info);

	private:
	};

};