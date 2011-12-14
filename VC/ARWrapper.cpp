#include "ARWrapper.h"

#define PI	3.1415926536

#define RAD2DEG(x)	\
	( (x) * 180 / PI )

namespace CGLibs {
#pragma region VARIABLES
	char * ARWrapper::tmp_str_ptr;
	string ARWrapper::cam_data;
	string ARWrapper::file_hiro;
	string ARWrapper::file_sample1;
	string ARWrapper::file_sample2;
	string ARWrapper::file_kanji;
	ARParam ARWrapper::cparam;
	string ARWrapper::v_conf;

	int ARWrapper::patt_hiro;
	int ARWrapper::patt_sample;
	//int ARWrapper::markers[3];
	vector<Pattern> ARWrapper::patterns;
	vector<int> ARWrapper::markers;
	int ARWrapper::hiro_mark;
	int ARWrapper::sample_mark;
	double ARWrapper::patt_width;
	double ARWrapper::hiro_center[2];
	double ARWrapper::sample_center[2];
	double ARWrapper::patt_trans[3][4];

	int ARWrapper::patt_thresh;
	int ARWrapper::v_width, ARWrapper::v_height;
	int ARWrapper::frame_count;
#pragma endregion

#pragma region METHODS
	void ARWrapper::initVars() {
		tmp_str_ptr = NULL;
		cam_data = "../Data/camera_para.dat";
		file_hiro = "../Data/hiro.patt";
		file_sample1 = "../Data/sample1.patt";
		file_sample2 = "../Data/sample2.patt";
		file_kanji = "../Data/kanji.patt";
		#ifdef _WIN32
		v_conf = "../Data/WDM_camera_flipV.xml";
		#else
		vconf = "";
		#endif

		patt_thresh = 100;
		patt_width = 80.0;
	}
	void ARWrapper::init(int *argc, char **argv) {

		initVars();

		ARParam wparam;

		glutInit(argc, argv);
	
		/* open the video path */
		if(arVideoOpen(tmp_str(v_conf)) < 0) {
			printf("Error opening camera !!\n");
			exit(0);
		}

		/* find the size of the window */
		if(arVideoInqSize(&v_width, &v_height)< 0) {
			printf("Error getting video size !!\n");
			exit(0);
		}

		printf("Image size (x,y) = (%d,%d)\n", v_width, v_height);

		/* set the initial camera parameters */
		if( arParamLoad(tmp_str(cam_data), 1, &wparam) < 0 ) {
			printf("Camera parameter load error !!\n");
			exit(0);
		}

		arParamChangeSize( &wparam, v_width, v_height, &cparam );
		arInitCparam(&cparam);
		printf("*** Camera Parameters ***\n");
		arParamDisp(&cparam);

		/* open the graphics window */
		argInit( &cparam, 1.0, 0, 0, 0, 0 );

		// Loads patterns
		patterns.push_back(Pattern(file_hiro));
		patterns.push_back(Pattern(file_sample1));
		patterns.push_back(Pattern(file_sample2));
		patterns.push_back(Pattern(file_kanji));
	}
	void ARWrapper::run() {
		arVideoCapStart();
		getchar();
		argMainLoop(mouseFunc, keyFunc, loopFunc);
	}
	void ARWrapper::mouseFunc(int button, int state, int x, int y) {
		return;
	}
	void ARWrapper::keyFunc(unsigned char key, int x, int y) {
		/* quit if the ESC key is pressed */
		if( key == 0x1b ) {
			printf("*** %f (frame/sec)\n", (double)frame_count/arUtilTimer());
			cleanup();
			exit(0);
		}
	}
	void ARWrapper::loopFunc() {
		ARUint8         *dataPtr;
		ARMarkerInfo    *marker_info;
		int             marker_num;
		int             i, num_detected;

		/* grab a vide frame */
		if((dataPtr = (ARUint8 *) arVideoGetImage()) == NULL) {
			arUtilSleep(2); /* regular sleep */
			return;
		}

		if(frame_count == 0 )
			arUtilTimerReset(); /* Que fique claro, eu nao gosto disto */
		frame_count++;

		argDrawMode2D();
		argDispImage(dataPtr, 0, 0);

		/* detect the markers in the video frame */
		if(arDetectMarker(dataPtr, patt_thresh, &marker_info, &marker_num) < 0) {
			cleanup();
			exit(0);
		}

		arVideoCapNext();

		num_detected = 0;
		// for each marker detected, check for visibility
		for(i = 0; i < marker_num; ++i ) {
			// compare current marker with patterns
			for(vector<Pattern>::iterator it = patterns.begin(); it != patterns.end(); ++it) {
				if (it->match(marker_info[i])) {
					++num_detected;
					markerDetected(*it, marker_info[i]);
				}
			}
		}

		argSwapBuffers();
	}

	void ARWrapper::markerDetected(Pattern pattern, ARMarkerInfo marker) {
		double gl_mat[16];

		//compute transformation matrix from pattern
		arGetTransMat(&marker, pattern.getCenter(), pattern.getWidth(), patt_trans);

		//get OpenGL matrix
		argConvGlpara(patt_trans, gl_mat);

		/*cout << "pattern " << pattern.getId() << " detected" << endl;
		cout << gl_mat[0] << " " << gl_mat[4] << " " << gl_mat[8] << " " << gl_mat[12] << endl;
		cout << gl_mat[1] << " " << gl_mat[5] << " " << gl_mat[9] << " " << gl_mat[13] << endl;
		cout << gl_mat[2] << " " << gl_mat[6] << " " << gl_mat[10] << " " << gl_mat[14] << endl;
		cout << gl_mat[3] << " " << gl_mat[7] << " " << gl_mat[11] << " " << gl_mat[15] << endl;*/

		/**
		 * TODO CALCULAR AQUI AS COORDS DO OBJECTO A PARTIR DA MATRIZ GL
		 */
		if (gl_mat[12] != 0) {
			cout << "X = (" << (int) gl_mat[12] << ", " << (int) gl_mat[13] << ", " << (int) gl_mat[14] << ")" << endl;
		}

		double x, y, z;/*	coordenadas do objecto	*/
		double a, b, c;/*	ângulos de rotação nos eixos Ox, Oy e Oz, respectivamente	*/
		double sa, ca;/*	sine and cosine of a	*/
		double sb, cb;/*	sine and cosine of b	*/
		double sc, cc;/*	sine and cosine of c	*/
		
		/* Posição */
		x = gl_mat[12];		y = gl_mat[13];		z = gl_mat[14];
		
		/* Rotação, assumindo Rx, Ry, Rz	*/
		sb = gl_mat[8];
		cb = sqrt(1 - sb*sb);
		sa = - ( gl_mat[9] / cb );
		//ca = gl_mat[10] / cb;
		sc = gl_mat[4] / cb;
		//cc = gl_mat[0] / cb;

		a = RAD2DEG( asin(sa) );
		b = RAD2DEG( asin(sb) );
		c = RAD2DEG( asin(sc) );

		renderOnPattern(pattern.getId(), gl_mat);
		//renderManually(pattern.getId(), x, y, z, a, b, c);
	}

	void ARWrapper::renderOnPattern(int pattern_index, double *gl_mat) {
		GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
		GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
		GLfloat   mat_flash_shiny[] = {50.0};
		GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
		GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
		GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

		argDrawMode3D();
		argDraw3dCamera(0, 0);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
    
		// load the camera transformation matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(gl_mat);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		//glMatrixMode(GL_MODELVIEW);
		glTranslatef(0.0, 0.0, 25.0);
    
		switch(pattern_index) {
		case MARKER_HIRO:
			glColor3f(1.0, 0.0, 0.0);
			glutSolidCube(50.0);
			break;

		case MARKER_SAMPLE1:
			glColor3f(0.0, 1.0, 0.0);
			glutSolidTeapot(50.0);
			break;

		case MARKER_SAMPLE2:
			glColor3f(0.0, 0.0, 1.0);
			glutSolidCube(50.0);
			break;

		case MARKER_KANJI:
			glColor3f(0.5, 0.5, 0.5);
			glutSolidCube(50.0);
			break;
		}

		glDisable(GL_LIGHTING);
		glDisable( GL_DEPTH_TEST );
	}

	void ARWrapper::renderManually(int pattern_index, double x, double y, double z, double a, double b, double c) {
		GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
		GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
		GLfloat   mat_flash_shiny[] = {50.0};
		GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
		GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
		GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

		argDrawMode3D();
		argDraw3dCamera(0, 0);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
    
		// load the camera transformation matrix
		//glMatrixMode(GL_MODELVIEW);
		//glLoadMatrixd(gl_mat);

		glTranslatef(x,y,z);
		glRotatef(a, 1, 0, 0);
		glRotatef(b, 0, 1, 0);
		glRotatef(c, 0, 0, 1);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0.0, 0.0, 25.0);
    
		switch(pattern_index) {
		case MARKER_HIRO:
			glColor3f(1.0, 0.0, 0.0);
			glutSolidCube(50.0);
			break;

		case MARKER_SAMPLE1:
			glColor3f(0.0, 1.0, 0.0);
			glutSolidCube(50.0);
			break;

		case MARKER_SAMPLE2:
			glColor3f(0.0, 0.0, 1.0);
			glutSolidCube(50.0);
			break;

		case MARKER_KANJI:
			glColor3f(0.5, 0.5, 0.5);
			glutSolidCube(50.0);
			break;
		}

		glDisable(GL_LIGHTING);
		glDisable( GL_DEPTH_TEST );
	}

	void ARWrapper::cleanup() {
		arVideoCapStop();
		arVideoClose();
		argCleanup();

		if (tmp_str_ptr != NULL) free(tmp_str_ptr);
	}
	void ARWrapper::drawCube() {
		double    gl_para[16];
		GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
		GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
		GLfloat   mat_flash_shiny[] = {50.0};
		GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
		GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
		GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
    
		argDrawMode3D();
		argDraw3dCamera(0, 0);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
    
		/* load the camera transformation matrix */
		argConvGlpara(patt_trans, gl_para);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(gl_para);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMatrixMode(GL_MODELVIEW);
		glTranslatef( 0.0, 0.0, 25.0 );
		glutSolidCube(50.0);
		glDisable( GL_LIGHTING );

		glDisable( GL_DEPTH_TEST );
	}
	void ARWrapper::drawTeapot() {
		double    gl_para[16];
		GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
		GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
		GLfloat   mat_flash_shiny[] = {50.0};
		GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
		GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
		GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};
    
		argDrawMode3D();
		argDraw3dCamera(0, 0);
		glClearDepth(1.0);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
    
		/* load the camera transformation matrix */
		argConvGlpara(patt_trans, gl_para);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixd(gl_para);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
		glMatrixMode(GL_MODELVIEW);
		glTranslatef(0.0, 0.0, 25.0);
		//glRotatef(90, 1, 0, 0);
		glutSolidTeapot(50.0);
		glDisable(GL_LIGHTING);

		glDisable( GL_DEPTH_TEST );
	}
	char * ARWrapper::tmp_str(string cpp_str) {
		if (tmp_str_ptr != NULL) free(tmp_str_ptr);
		tmp_str_ptr = _strdup(cpp_str.c_str());
		return tmp_str_ptr;
	}
#pragma endregion
}