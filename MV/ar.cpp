#include "ar.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef __APPLE__
#include<gl/GL.h>
#include<gl/GLUT.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include <iostream>
#include <string>
#include <iostream>

#include <AR/gsub.h>

#include <AR/video.h>

//#include <AR/param.h>
//#include <AR/ar.h>

#ifndef _DEBUG
#pragma comment (lib,"libAR")
#pragma comment (lib,"libARgsub")
#pragma comment (lib,"libARvideo")
#else
#pragma comment (lib,"libARd")
#pragma comment (lib,"libARgsubd")
#pragma comment (lib,"libARvideod")
#endif

#include "blaf.h"

namespace cg
{
	namespace pi
	{
		namespace ar
		{
			//	Camera
			const string	camera_data_filename	=	"../conf/camera.dat";
			ARParam			camera_param;

			//	Pattern
			const int		pattern_threshold		=	100;
			const double	pattern_width			=	80.0;
			const string	pattern_filename		=	"../patterns/hiro.patt";
			Pattern			pattern(pattern_filename);
			double			pattern_transformation[3][4];

			//	Video
#ifdef _WIN32
			const string	video_conf_filename		=	"../conf/video.xml";
#else
			const string	video_conf_filename		=	"";
#endif
			int				video_height;
			int				video_width;
			unsigned		video_frame_count;

			//	Player
			Player *		player;






			//////////////////////////////////////////////////////////////////////////
			//	Prototypes
			void markerDetected( Pattern pattern , ARMarkerInfo marker );
			void render( int pattern_index , double gl_mat[16] );
			void extractTransModelView(double gl_mat[16], double *x, double *y, double *z, double *a, double *b, double *c);
			void renderAuto(int pattern_index, double *gl_mat);
			void renderManual(int pattern_index, GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c);
			void cleanup();






			//////////////////////////////////////////////////////////////////////////
			/// <summary>Initializes the application components. This function calls <see cref="initVars" />, glutInit, sets up the video capture mechanism and the window, and loads the patterns.</summary>
			void init(cg::Player *player) {
				ARParam param;

				/* open the video path */
				char *filename = _strdup( video_conf_filename.c_str() );
				if ( arVideoOpen( filename ) < 0 )
				{
					cerr
						<<	"Error opening camera !!"
						<<	endl;
					getchar();
					exit(1);
				}
				free( filename );

				/* find the size of the window */
				if ( arVideoInqSize( &video_width , &video_height ) < 0 )
				{
					cerr
						<<	"Error getting video size !!"
						<<	endl;
					getchar();
					exit(1);
				}

				printf( "Image size (x,y) = (%d,%d)\n", video_width, video_height );

				/* set the initial camera parameters */
				if ( arParamLoad( camera_data_filename.c_str() , 1 , &param ) < 0 )
				{
					cerr
						<<	"Camera parameter load error !!"
						<<	endl;
					getchar();
					exit(1);
				}

				arParamChangeSize( &param, video_width, video_height, &camera_param );
				arInitCparam( &camera_param );
				cout
					<<	"*** Camera Parameters ***"
					<<	endl;
				arParamDisp( &camera_param );

				/* open the graphics window */
				argInit( &camera_param , 1.0 , 0 , 0, 0, 0 );

				video_frame_count = 0;
				ar::player = player;
			}

			/// <summary>Mouse button handler. Currently implements no reaction to mouse button clicks.</summary>
			void mouseFunc(int button, int state, int x, int y) {
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
			void keyFunc(unsigned char key, int x, int y) {
				/* quit if the ESC key is pressed */
				if( key == 0x1b ) {
					printf("*** %f (frame/sec)\n", (double)video_frame_count/arUtilTimer());
					cleanup();
					exit(0);
				}
			}





			//////////////////////////////////////////////////////////////////////////
			///	<summary>Main loop function: captures a video frame and checks it for markers. For every detected marker, compares it with the patterns set.<seealso cref="initVars" /> When a detected marker is associated with a loaded pattern, calls <see cref="markerDetected" />.</summary>
			void loopFunc()
			{
				ARUint8         *dataPtr;
				ARMarkerInfo    *marker_info;
				int             marker_num;
				int             num_detected;

				/* grab a video frame */
				dataPtr = (ARUint8*) arVideoGetImage();
				if ( ! dataPtr ) {
					arUtilSleep(2);
					return;
				}

				if ( ! video_frame_count++ )
					arUtilTimerReset();

				argDrawMode2D();
				argDispImage( dataPtr , 0 , 0 );

				/* detect the markers in the video frame */
				if( arDetectMarker( dataPtr , pattern_threshold , &marker_info , &marker_num ) < 0 )
				{
					cleanup();
					exit(1);
				}

				arVideoCapNext();

				num_detected = 0;
				// for each marker detected, check for visibility
				for ( int i = 0 ; i < marker_num ; ++i )
				{
					// compare
					if ( pattern.id() == marker_info[i].id )
					{
						markerDetected( pattern , marker_info[i] );
					}
				}

				argSwapBuffers();
			}






			//////////////////////////////////////////////////////////////////////////
			///	<summary>Handles the matched markers, extracting position and orientation information. Tests the information extraction process by overlapping a manually rendered object (based on the translation and rotation data collected) with an automatically rendered object (loading the transformation matrix directly into OpenGL).</summary>
			///	<param name="pattern">The matched pattern structure</param>
			///	<param name="marker">The detected marker structure</param>
			void markerDetected ( Pattern pattern , ARMarkerInfo marker )
			{
				double gl_mat[16];
				double center[2];

				//compute transformation matrix from pattern
				center[0] = pattern.centerX();
				center[1] = pattern.centerY();
				arGetTransMat( &marker , center , pattern.width() , pattern_transformation );

				//get OpenGL matrix
				argConvGlpara( pattern_transformation , gl_mat );
				//double rot[3][3];
				//arGetInitRot(&marker, patt_trans, rot);

				render( pattern.id() , gl_mat );
			}






			//////////////////////////////////////////////////////////////////////////
			///	<summary>Main rendering function. Calls <see cref="renderAuto" /> for the automatically set object and <see cref="renderManual" /> for the manually positioned and oriented object. Information is extracted through <see cref="extractTransModelView" />.</summary>
			///	<param name="pattern_index">Index of the matched pattern, on which to render</param>
			///	<param name="gl_mat">OpenGL ModelView transformation homogeneous 4x4 matrix, in a column-major array, as conventioned in OpenGL</param>
			void render ( int pattern_index , double gl_mat[16] )
			{
				double x, y, z;//	Pattern position
				double a, b, c;//	Pattern Euler angles

				/*	information extraction	*/
				extractTransModelView( gl_mat , &x , &y , &z , &a , &b , &c );

				cout
					<<	"b: "
					<<	-b
					<<	endl;
				//if ( b > 0 )
				//	player->left();
				//else if ( b < 0 )
				//	player->right();
				//else
				//	player->straight();
				player->turn( blaf::deg2rad(b) );

				/*	render object	*/
				renderAuto( pattern_index , gl_mat );
				renderManual(
					pattern_index,
					(GLfloat) x,
					(GLfloat) y,
					(GLfloat) z,
					(GLfloat) a,
					(GLfloat) b,
					(GLfloat) c)
					;
			}






			//////////////////////////////////////////////////////////////////////////
			///	<summary>Extracts transformations from the ModelView matrix (usually obtained automatically).</summary>
			///	<param name="gl_mat">OpenGL ModelView transformation homogeneous 4x4 matrix, in a column-major array, as conventioned in OpenGL</param>
			///	<param name="x">Reference to return the first component of the position.</param>
			///	<param name="y">Reference to return the second component of the position.</param>
			///	<param name="z">Reference to return the third component of the position.</param>
			///	<param name="a">Reference to return the counter-clockwise rotation angle around the x axis.</param>
			/// <param name="b">Reference to return the counter-clockwise rotation angle around the y axis.</param>
			///	<param name="c">Reference to return the counter-clockwise rotation angle around the z axis.</param>
			void extractTransModelView(double gl_mat[16], double *x, double *y, double *z, double *a, double *b, double *c)
			{
				double ta;
				double sb;
				double tc;

				/*	RzRyRx rotation matrix	*/
				sb = - gl_mat[2];

				tc = gl_mat[1] / gl_mat[0];
				ta = gl_mat[6] / gl_mat[10];

				/*	angles	*/
				*a = blaf::rad2deg( atan(ta) );
				*b = blaf::rad2deg( asin(sb) );
				*c = blaf::rad2deg( atan(tc) );

				/*	positions	*/
				*x = gl_mat[12];
				*y = gl_mat[13];
				*z = gl_mat[14];
			}






			//////////////////////////////////////////////////////////////////////////
			///	<summary>Renders an automatically positioned and oriented object, by loading a given OpenGL ModelView matrix.</summary>
			///	<param name="pattern_index">Index of the matched pattern, on which to render</param>
			///	<param name="gl_mat">OpenGL ModelView transformation homogeneous 4x4 matrix, in a column-major array, as conventioned in OpenGL</param>
			void renderAuto(int pattern_index, double *gl_mat)
			{
				GLfloat   mat_ambient[]     = { 0.0f , 0.0f , 1.0f , 1.0f };
				GLfloat   mat_flash[]       = { 0.0f , 0.0f , 1.0f , 1.0f };
				GLfloat   mat_flash_shiny[] = { 50.0f };
				GLfloat   light_position[]  = { 100.0f , -200.0f , 200.0f , 0.0f };
				GLfloat   ambi[]            = { 0.1f , 0.1f , 0.1f , 0.1f };
				GLfloat   lightZeroColor[]  = { 0.9f , 0.9f , 0.9f , 0.1f };

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

				glColor3f(1.0, 0.0, 0.0);
				glutSolidTeapot(50.0);

				glDisable(GL_LIGHTING);
				glDisable( GL_DEPTH_TEST );
			}






			//////////////////////////////////////////////////////////////////////////
			///	<summary>Renders a manually positioned and oriented object, using explicitly given information. The information required for this can be obtained from <see cref="extractTransModelView" />.</summary>
			///	<param name="pattern_index">Index of the matched pattern, on which to render</param>
			///	<param name="x">Value of the first component of the position.</param>
			///	<param name="y">Value of the second component of the position.</param>
			///	<param name="z">Value of the third component of the position.</param>
			///	<param name="a">Value of the counter-clockwise rotation angle around the x axis.</param>
			/// <param name="b">Value of the counter-clockwise rotation angle around the y axis.</param>
			///	<param name="c">Value of the counter-clockwise rotation angle around the z axis.</param>
			void renderManual(int pattern_index, GLfloat x, GLfloat y, GLfloat z, GLfloat a, GLfloat b, GLfloat c)
			{
				GLfloat   mat_ambient[]     = { 0.0f , 1.0f , 0.0f , 1.0f };
				GLfloat   mat_flash[]       = { 0.0f , 1.0f , 0.0f , 1.0f };
				GLfloat   mat_flash_shiny[] = { 50.0f };
				GLfloat   light_position[]  = { 100.0f , -200.0f , 200.0f , 0.0f };
				GLfloat   ambi[]            = { 0.1f , 0.1f , 0.1f , 0.1f };
				GLfloat   lightZeroColor[]  = { 0.9f , 0.9f , 0.9f , 0.1f };

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

				glColor3f(1.0, 0.0, 0.0);
				glutSolidTeapot(50.0);

				glDisable(GL_LIGHTING);
				glDisable( GL_DEPTH_TEST );
			}






			//////////////////////////////////////////////////////////////////////////
			/// <summary>Starts the video capture and the application main loop.</summary>
			void run()
			{
				arVideoCapStart();
				argMainLoop( mouseFunc , keyFunc , loopFunc );
			}






			//////////////////////////////////////////////////////////////////////////
			///	<summary>Begins the process of termination by stopping the video capture and closing it, and calling the appropriate functions to release any other allocated resources.</summary>
			void cleanup()
			{
				arVideoCapStop();
				arVideoClose();
				argCleanup();
			}



		}
	}
}