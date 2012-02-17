#pragma comment (lib,"libAR")
#pragma comment (lib,"libARgsub")
#pragma comment (lib,"libARvideo")

#include "artoolkit.h"

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
	/// <summary>Initializes the system configurations, namely camera data, pattern files and properties, and camera configuration files.</summary>
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

	/// <summary>Initializes the application components. This function calls <see cref="initVars" />, glutInit, sets up the video capture mechanism and the window, and loads the patterns.</summary>
	void ARWrapper::init(int *argc, char **argv) {
		ARParam wparam;

		initVars();
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

	/// <summary>Starts the video capture and the application main loop.</summary>
	void ARWrapper::run() {
		arVideoCapStart();
		argMainLoop(mouseFunc, keyFunc, loopFunc);
	}

	/// <summary>Mouse button handler. Currently implements no reaction to mouse button clicks.</summary>
	void ARWrapper::mouseFunc(int button, int state, int x, int y) {
		return;
	}

	///	<summary>Keyboard button handler.
	///		<list type="table">
	///			<listheader>
	///				<term>Button Key</term>
	///				<description>Reaction</description>
	///			</listheader>
	///			<item>
	///				<term>ESC</term>
	///				<description>Prints framerate and quits.</description>
	///			</item>
	///		</list>
	///	</summary>
	void ARWrapper::keyFunc(unsigned char key, int x, int y) {
		/* quit if the ESC key is pressed */
		if( key == 0x1b ) {
			printf("*** %f (frame/sec)\n", (double)frame_count/arUtilTimer());
			cleanup();
			exit(0);
		}
	}

	///	<summary>Main loop function: captures a video frame and checks it for markers. For every detected marker, compares it with the patterns set.<seealso cref="initVars" /> When a detected marker is associated with a loaded pattern, calls <see cref="markerDetected" />.</summary>
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
			arUtilTimerReset();
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

	///	<summary>Handles the matched markers, extracting position and orientation information. Tests the information extraction process by overlapping a manually rendered object (based on the translation and rotation data collected) with an automatically rendered object (loading the transformation matrix directly into OpenGL).</summary>
	///	<param name="pattern">The matched pattern structure</param>
	///	<param name="marker">The detected marker structure</param>
	void ARWrapper::markerDetected(Pattern pattern, ARMarkerInfo marker) {
		double gl_mat[16];

		//compute transformation matrix from pattern
		arGetTransMat(&marker, pattern.getCenter(), pattern.getWidth(), patt_trans);

		//get OpenGL matrix
		argConvGlpara(patt_trans, gl_mat);
		double rot[3][3];
		arGetInitRot(&marker, patt_trans, rot);

		render(pattern.getId(), gl_mat);
	}

	///	<summary>Extracts transformations from the ModelView matrix (usually obtained automatically).</summary>
	///	<param name="gl_mat">OpenGL ModelView transformation homogeneous 4x4 matrix, in a column-major array, as conventioned in OpenGL</param>
	///	<param name="x">Reference to return the first component of the position.</param>
	///	<param name="y">Reference to return the second component of the position.</param>
	///	<param name="z">Reference to return the third component of the position.</param>
	///	<param name="a">Reference to return the counter-clockwise rotation angle around the x axis.</param>
	/// <param name="b">Reference to return the counter-clockwise rotation angle around the y axis.</param>
	///	<param name="c">Reference to return the counter-clockwise rotation angle around the z axis.</param>
	void ARWrapper::extractTransModelView(double gl_mat[16], double *x, double *y, double *z, double *a, double *b, double *c)
	{
		double sa, ca, ta;
		double sb, cb;
		double sc, cc, tc;

		/*	RzRyRx rotation matrix	*/
		sb = - gl_mat[2];

		tc = gl_mat[1] / gl_mat[0];
		ta = gl_mat[6] / gl_mat[10];

		/*	angles	*/
		*a = RAD2DEG( atan(ta) );
		*b = RAD2DEG( asin(sb) );
		*c = RAD2DEG( atan(tc) );

		/*	positions	*/
		*x = gl_mat[12];
		*y = gl_mat[13];
		*z = gl_mat[14];
	}

	///	<summary>Main rendering function. Calls <see cref="renderAuto" /> for the automatically set object and <see cref="renderManual" /> for the manually positioned and oriented object. Information is extracted through <see cref="extractTransModelView" />.</summary>
	///	<param name="pattern_index">Index of the matched pattern, on which to render</param>
	///	<param name="gl_mat">OpenGL ModelView transformation homogeneous 4x4 matrix, in a column-major array, as conventioned in OpenGL</param>
	void ARWrapper::render (int pattern_index, double *gl_mat)
	{
		double x, y, z;/*	Pattern position	*/
		double a, b, c;/*	Pattern Euler angles	*/

		/*	information extraction	*/
		extractTransModelView(gl_mat, &x, &y, &z, &a, &b, &c);

		/*	render object	*/
		renderAuto(pattern_index, gl_mat);
		renderManual(pattern_index, x, y, z, a, b, c);
	}

	///	<summary>Renders an automatically positioned and oriented object, by loading a given OpenGL ModelView matrix.</summary>
	///	<param name="pattern_index">Index of the matched pattern, on which to render</param>
	///	<param name="gl_mat">OpenGL ModelView transformation homogeneous 4x4 matrix, in a column-major array, as conventioned in OpenGL</param>
	void ARWrapper::renderAuto(int pattern_index, double *gl_mat) {
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

		switch(pattern_index) {
		case MARKER_HIRO:
			glColor3f(1.0, 0.0, 0.0);
			glutSolidTeapot(50.0);
			break;
		case MARKER_SAMPLE2:
			glColor3f(0.0, 0.0, 1.0);
			glutSolidCube(50.0);
			break;
		}

		glDisable(GL_LIGHTING);
		glDisable( GL_DEPTH_TEST );
	}

	///	<summary>Renders a manually positioned and oriented object, using explicitly given information. The information required for this can be obtained from <see cref="extractTransModelView" />.</summary>
	///	<param name="pattern_index">Index of the matched pattern, on which to render</param>
	///	<param name="x">Value of the first component of the position.</param>
	///	<param name="y">Value of the second component of the position.</param>
	///	<param name="z">Value of the third component of the position.</param>
	///	<param name="a">Value of the counter-clockwise rotation angle around the x axis.</param>
	/// <param name="b">Value of the counter-clockwise rotation angle around the y axis.</param>
	///	<param name="c">Value of the counter-clockwise rotation angle around the z axis.</param>
	void ARWrapper::renderManual(int pattern_index, double x, double y, double z, double a, double b, double c) {
		GLfloat   mat_ambient[]     = {0.0, 1.0, 0.0, 1.0};
		GLfloat   mat_flash[]       = {0.0, 1.0, 0.0, 1.0};
		GLfloat   mat_flash_shiny[] = {50.0};
		GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
		GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
		GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

		// load the camera transformation matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(x,y,z);
		glRotatef(a, 1, 0, 0);/* flip it vertically	*/
		glRotatef(b, 0, 1, 0);
		glRotatef(c, 0, 0, 1);
		//glRotatef(180,1,0,0);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);
		glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
		glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);	
		glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

		switch(pattern_index) {
		case MARKER_HIRO:
			glColor3f(1.0, 0.0, 0.0);
			glutSolidTeapot(50.0);
			break;
		case MARKER_SAMPLE2:
			glColor3f(0.0, 0.0, 1.0);
			glutSolidCube(50.0);
			break;
		}

		glDisable(GL_LIGHTING);
		glDisable( GL_DEPTH_TEST );
	}

	///	<summary>Begins the process of termination by stopping the video capture and closing it, and calling the appropriate functions to release any other allocated resources.</summary>
	void ARWrapper::cleanup() {
		arVideoCapStop();
		arVideoClose();
		argCleanup();

		if (tmp_str_ptr != NULL) free(tmp_str_ptr);
	}

	///	<summary>Auxiliary function to convert from C++ strings to ansi-C strings.</summary>
	char * ARWrapper::tmp_str(string cpp_str) {
		if (tmp_str_ptr != NULL) free(tmp_str_ptr);
		tmp_str_ptr = _strdup(cpp_str.c_str());
		return tmp_str_ptr;
	}
#pragma endregion
}