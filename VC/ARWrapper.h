#pragma once

#ifdef _WIN32
#include <windows.h>
#endif
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

#ifndef __APPLE__
#include<gl/GL.h>
#include<gl/GLUT.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include <string.h>
#include <iostream>
#include <vector>
using namespace std;

#include "Pattern.h"

namespace CGLibs {
	class ARWrapper {
	public:
	#pragma region PUBLIC
		// Constant Declaration
		static const int MARKER_SAMPLE = 0;
		static const int MARKER_HIRO = 1;

		// Initializes AR Toolkit
		static void init(int *argc, char **argv);
		// Starts marker recognition
		static void run();
	#pragma endregion

	private:
	#pragma region PRIVATE_VARS
		static string cam_data;
		static string file_hiro, file_sample;
		static string v_conf;
		static ARParam cparam;
		static char *tmp_str_ptr;

		static int patt_hiro, patt_sample;
		static int markers_c[3];
		static vector<Pattern> patterns;
		static vector<int> markers;
		static double patt_width;
		static double hiro_center[2];
		static double sample_center[2];
		static double patt_trans[3][4];
	
		static int hiro_mark;
		static int sample_mark;

		static int patt_thresh;
	
		static int v_width, v_height;
		static int frame_count;
	#pragma endregion

	#pragma region PRIVATE_METHODS
		// Initializes AR Toolkit vars
		static void initVars();
	
		static void loopFunc();
		static void cleanup();
		static void markerDetected(Pattern pattern, ARMarkerInfo marker);
		static void renderOnPattern(int pattern_id, double *gl_mat);
		static void renderManually(int pattern_index, double x, double y, double z, double a, double b, double c);
		static void extractTransModelView(double gl_mat[16], double *x, double *y, double *z, double *a, double *b, double *c);

		// Drawing Function
		static void drawOnMarker(int marker);
		//static void drawCube();
		//static void drawTeapot();
	
		// Mouse Function
		static void mouseFunc(int button, int state, int x, int y);
		// Keyboard Function
		static void keyFunc(unsigned char key, int x, int y);

		static char * tmp_str(string cpp_str);
	#pragma endregion
	};
}