////
//// Lighthouse3D.com VS*L OpenGL Sample
////
//// Loading and displaying a Textured Model
////
//// Uses:
////  Assimp library for model loading
////		http://assimp.sourceforge.net/
////  Devil for image loading
////		http://openil.sourceforge.net/
////  GLEW for OpenGL post 1.1 functions
////		http://glew.sourceforge.net/
////	TinyXML for font definition parsing
////		http://sourceforge.net/projects/tinyxml/
////
//// This demo was built for learning purposes only.
//// Some code could be severely optimised, but I tried to
//// keep as simple and clear as possible.
////
//// The code comes with no warranties, use it at your own risk.
//// You may use it, or parts of it, wherever you want.
////
//// If you do use it I would love to hear about it. 
//// Just post a comment at Lighthouse3D.com
//
//// Have Fun :-)
//

#include <string>
using std::string;

// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

// Use Very Simple Libs
#include <VS/vsGLInfoLib.h>
#include <VS/vsMathLib.h>
#include <VS/vsResModelLib.h>
#include <VS/vsShaderLib.h>

//#include "camera.h"
#include "blaf.h"

#include "ar.h"



VSMathLib *vsml;		//!<	Very Simple Math Library instance, used for drawing the scene.


VSShaderLib shaders[8];	//!<	Collection of shaders to draw the teapots in the different positions of the cube.
						//!<	-	0	:	black
						//!<	-	1	:	blue
						//!<	-	2	:	green
						//!<	-	3	:	cyan
						//!<	-	4	:	red
						//!<	-	5	:	magenta
						//!<	-	6	:	yellow
						//!<	-	7	:	white

VSResModelLib teapot;	//!<	Model used to draw the teapots.



float camDirection[4] = { 0.0f , 0.0f , -1.0f , 0.0f };	//!<	Default camera looking direction.
float camPosition[4] = { 0.0f , 0.0f , 0.0f , 1.0f };	//!<	Camera position, set by default to the world coordinate system origin.
float camUp[4] = { 0.0f , 1.0f , 0.0f , 0.0f };			//!<	Default camera up vector (defines the vertical orientation of the camera).
float camZoom = 5.0f;									//!<	Distance units between the camera and the target.



int straightBeginMilis;				//!<	Time value in miliseconds returned by the GLUT library when beginning movement or refreshing the current position of the camera.
float straightFactor;				//!<	Factor which influences how the movement is performed.
									//!<	A positive value performs a forward movement, whereas a negative value will perform a backwards movement.
									//!<	Absolute values greater then 1 increase velocity. Less than 1 decreases it.
const float straightVelocity = 10;	//!<	Movement velocity.
bool keyboardLowKeysDown[26];		//!<	Array of boolean flags to store which keys are down.



float glTransformationMatrix[16];	//!<	Transformation matrix shared between the capture system and the virtual world.
									//!<	The values in this matrix are set according to the ones retrieved from the ARToolKit library, ignoring any translation data.
									//!<	Due to its shared nature, this matrix is controlled by a mutex primitive to avoid race conditions. Accesses must follow a call to cg::pi::ar::lockOutput() and be followed by a call to cg::pi::ar::unlockOutput()
									//!<	\sa cg::pi::ar::lockOutput(), cg::pi::ar::unlockOutput()



//!	Set the camera's perspective to fit the new window size.
/*!
	Changes the viewport to fit the entire window and changes the camera's perspective according to the new ratio of the horizontal and vertical dimensions.

	Uses the Very Simple Math Library retrieved in the initialization.

	\param	w	The new width of the window.
	\param	h	The new height of the window.
*/
void changeSize(int w, int h) {

	float ratio;
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	ratio = (1.0f * w) / h;
	vsml->loadIdentity(VSMathLib::PROJECTION);
	vsml->perspective(53.13f, ratio, 0.1f, 10000.0f);
}



//!	Draws the scene in the 3-D virtual world.
/*!
	Retrieves the transformation matrix set by the ARToolKit library and computes the camera's new direction, up vector and target accordingly, using the lookAt function to correctly set the perspective.

	Also draws the teapots in a cube shape around the origin of the world coordinate system, changing the shader used to set the teapot's color according to its position in the cube.

	\sa cg::pi::ar::lockOutput(), cg::pi::ar::unlockOutput()
*/
void renderScene(void) {

	glClear(
		GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT )
		;

	vsml->loadIdentity(VSMathLib::MODEL);
	vsml->loadIdentity(VSMathLib::VIEW);

	{
		float transpose[16];
		float inverse[16];

		cg::pi::ar::lockOutput();
		//vsml->loadMatrix( VSMathLib::AUX0 , glTransformationMatrix );
		for ( int j = 0 ; j < 4 ; ++j )
		{
			for ( int i = 0 ; i < 4 ; ++i )
			{
				int k = i + j * 4;
				int l = i * 4 + j;
				transpose[l] = glTransformationMatrix[k];
			}
		}
		cg::pi::ar::unlockOutput();

		//	inverse
		blaf::gluInvertMatrix(transpose,inverse);
		vsml->loadMatrix( VSMathLib::AUX0 , inverse );

		float newCamDirection[4];
		float newCamTarget[4];
		float newCamUp[4];

		vsml->multMatrixPoint( VSMathLib::AUX0 , camDirection , newCamDirection );
		vsml->multMatrixPoint( VSMathLib::AUX0 , camUp , newCamUp );

		{//	movement
			if ( straightFactor )
			{
				if ( straightBeginMilis )
				{
					int ms = glutGet( GLUT_ELAPSED_TIME );
					int dms = ms - straightBeginMilis;
					straightBeginMilis = ms;
					float dt = dms * 0.001;
					float ds = straightVelocity * dt * straightFactor;
					for ( int k = 0 ; k < 4 ; ++k )
					{
						camPosition[k] += newCamDirection[k] * ds;
					}
				}
				else
					straightBeginMilis = glutGet( GLUT_ELAPSED_TIME );
			}
		}

		for ( int k = 0 ; k < 4 ; ++k )
		{
			newCamTarget[k] = camPosition[k] + newCamDirection[k] * camZoom;
		}

		vsml->lookAt(
			camPosition[0] , camPosition[1] , camPosition[2] ,
			newCamTarget[0] , newCamTarget[1] , newCamTarget[2] ,
			newCamUp[0] , newCamUp[1] , newCamUp[2] )
			;
	}

	
	{//	Cube [begin]
		for ( float x = -10.0f ; x < 12.5f ; x += 5.0f )
		{
			int xColor = x < -7.5f || x > 7.5f;

			for ( float y = -10.0f ; y < 12.5f ; y += 5.0f )
			{
				int yColor = y < -7.5f || y > 7.5f;

				for ( float z = -10.0f ; z < 12.5f ; z += 5.0f )
				{
					int zColor = z < -7.5f || z > 7.5f;
					int index = xColor * 4 + yColor * 2 + zColor;

					if ( index )
					{
						glUseProgram( shaders[index].getProgramIndex() );
						vsml->pushMatrix( VSMathLib::MODEL );
						vsml->translate( VSMathLib::MODEL , x , y , z );
						teapot.render();
						vsml->popMatrix( VSMathLib::MODEL );
					}
				}
			}
		}
	}// Cube [end]

	glutSwapBuffers();
}



//!	Timer event handler to refresh the screen.
/*!
	Refreshes the screen every 33 miliseconds to keep a frame rate close to 30 frames per second.

	/param	value	The identifier of the timer which called the event. Ignored in this case, as only one timer is created in the whole application.
*/
void timeElapsed(int value)
{
	glutPostRedisplay();
	glutTimerFunc( 33 , timeElapsed , 0 );
}



//!	Keyboard key press event handler.
/*!
	-	w	If moving backwards, stop. Otherwise, start moving forward.
	-	s	If moving forward, stop. Otherwise, start moving backwards.
*/
void keyPress(unsigned char key, int xx, int yy)
{
	switch (key)
	{
	case 'w':
		if ( ! keyboardLowKeysDown['w'-'a'] )
		{
			if ( keyboardLowKeysDown['s'-'a'] )
			{
				straightFactor = 0;
				straightBeginMilis = 0;
			}
			else
			{
				straightFactor = 1;
				straightBeginMilis = glutGet( GLUT_ELAPSED_TIME );
			}
		}
		break;
	case 's':
		if ( ! keyboardLowKeysDown['s'-'a'] )
		{
			if ( keyboardLowKeysDown['w'-'a'] )
			{
				straightFactor = 0;
				straightBeginMilis = 0;
			}
			else
			{
				straightFactor = -1;
				straightBeginMilis = glutGet( GLUT_ELAPSED_TIME );
			}
		}
		break;
	}
	if ( islower(key) )
		keyboardLowKeysDown[key-'a'] = true;
}

//!	Keyboard key up event handler.
/*!
	-	w	If moving forward, stop. Otherwise, start moving backwards.
	-	s	If moving backwards, stop. Otherwise, start moving forward.
*/
void keyUp(unsigned char key, int xx, int yy)
{
	switch (key)
	{
	case 'w':
		if ( keyboardLowKeysDown['w'-'a'] )
		{
			if ( keyboardLowKeysDown['s'-'a'] )
			{
				straightFactor = -1;
				straightBeginMilis = glutGet( GLUT_ELAPSED_TIME );
			}
			else
			{
				straightFactor = 0;
				straightBeginMilis = 0;
			}
		}
		break;
	case 's':
		if ( keyboardLowKeysDown['s'-'a'] )
		{
			if ( keyboardLowKeysDown['w'-'a'] )
			{
				straightFactor = 1;
				straightBeginMilis = glutGet( GLUT_ELAPSED_TIME );
			}
			else
			{
				straightFactor = 0;
				straightBeginMilis = 0;
			}
		}
		break;
	}
	if ( islower(key) )
		keyboardLowKeysDown[key-'a'] = false;
}



//!	Initialize one of the shaders for the teapots to be placed in the scene.
/*!
	Every shader initialized with this function implements only a lighting model using Phong's algorithm with a point of light, changing only the diffuse component.

	A total of eight different shaders may be initialized here, including the three primary colors RGB, the three secondary colors YCM, black and white.

	\param	color	Defines the color to be defined. Also represents the index of the shader instance to be initialized. This value is truncated at the maximum allowed, if greater.

	\sa	initCubeShaders()
*/
void initCubeSideShader( unsigned color )
{
	color = ( color > 7 ) ? 7 : color;
	shaders[color].init();
	shaders[color].loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/point/phong.vert.glsl")
		;
	shaders[color].loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/point/phong.frag.glsl")
		;

	// set semantics for the shader variables
	shaders[color].setProgramOutput( 0 , "color" );
	shaders[color].setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	shaders[color].setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	shaders[color].prepareProgram();

	VSGLInfoLib::getProgramInfo(shaders[color].getProgramIndex());
	printf("%s\n", shaders[color].getAllInfoLogs().c_str());

	//	uniforms
	{
		float lpos[4] = { 30.0 , 5.0 , 0.0 , 1.0 };
		float ldif[3];
		switch (color)
		{
		case 0://	black
			ldif[0] = 0.0;
			ldif[1] = 0.0;
			ldif[2] = 0.0;
			break;
		case 1://	blue
			ldif[0] = 0.0;
			ldif[1] = 0.0;
			ldif[2] = 1.0;
			break;
		case 2://	green
			ldif[0] = 0.0;
			ldif[1] = 1.0;
			ldif[2] = 0.0;
			break;
		case 3://	cyan
			ldif[0] = 0.0;
			ldif[1] = 1.0;
			ldif[2] = 1.0;
			break;
		case 4://	red
			ldif[0] = 1.0;
			ldif[1] = 0.0;
			ldif[2] = 0.0;
			break;
		case 5://	magenta
			ldif[0] = 1.0;
			ldif[1] = 0.0;
			ldif[2] = 1.0;
			break;
		case 6://	yellow
			ldif[0] = 1.0;
			ldif[1] = 1.0;
			ldif[2] = 0.0;
			break;
		case 7://	white
		default:
			ldif[0] = 1.0;
			ldif[1] = 1.0;
			ldif[2] = 1.0;
		}
		float lspc[3] = { 1.0 , 1.0 , 1.0 };
		shaders[color].setUniform( "lightPosition" , lpos );
		shaders[color].setUniform( "diffuse" , ldif );
		shaders[color].setUniform( "specular" , lspc );
		shaders[color].setUniform( "shininess" , 100.0f );
	}
}



//!	Initialize all the shaders for the teapots to be placed in the scene.
/*!
	Calls the initializer function for every shader instance to be used with the teapots.

	\sa	initCubeSideShader
*/
void initCubeShaders()
{
	for ( unsigned s = 0 ; s < 8 ; ++s )
		initCubeSideShader(s);
}



//!	Define the OpenGL options, load the models to be used and set initial values for global variables.
/*!
	Loads the model for the teapots to be placed in the scene, aborting the program in case of failure.

	Also, sets the transformation matrix to the identity.

	Enables the depth test, multisampling and defines the background color to 50% gray.
*/
void init()
{
	// load models
	if ( ! teapot.load("../models/teapot.3ds") )
	{
		using std::cerr;
		using std::endl;
		cerr
			<<	teapot.getErrors()
			<<	endl;
		getchar();
		exit(1);
	}

	{
		glTransformationMatrix[0] = 1;
		glTransformationMatrix[1] = 0;
		glTransformationMatrix[2] = 0;
		glTransformationMatrix[3] = 0;

		glTransformationMatrix[4] = 0;
		glTransformationMatrix[5] = 1;
		glTransformationMatrix[6] = 0;
		glTransformationMatrix[7] = 0;

		glTransformationMatrix[8] = 0;
		glTransformationMatrix[9] = 0;
		glTransformationMatrix[10] = 1;
		glTransformationMatrix[11] = 0;

		glTransformationMatrix[12] = 0;
		glTransformationMatrix[13] = 0;
		glTransformationMatrix[14] = 0;
		glTransformationMatrix[15] = 1;
	}

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}



//!	Initialize the Very Simple Math Library.
/*!
	Retrieve the VSML instance and set the uniform names for the matrices:
	- Project-View-Model
	- Normal
	- View-Model
	- View

	These matrices are required by the Phong lighting shaders, used to draw the teapots.
*/
void initVSL() {
	// Init VSML
	vsml = VSMathLib::getInstance();
	vsml->setUniformName(VSMathLib::PROJ_VIEW_MODEL, "pvm");
	vsml->setUniformName(VSMathLib::NORMAL, "normalMatrix");
	vsml->setUniformName(VSMathLib::VIEW_MODEL, "vm");
	vsml->setUniformName(VSMathLib::VIEW, "viewMatrix");
}



//!	Initialize the environment, set windows and callbacks and call the application main loop.
/*!
	Initializes the GLUT library and context.

	Creates and sets the callbacks for the virtual world window (Teapot World).

	Also initializes the GLEW library (required to load the models), and calls init(), initVSL() and initCubeShaders().

	Initializes the ARToolKit library. The ARToolKit library is also responsible for calling the application main loop.

	\sa	init(), initVSL(), initCubeShaders(), cg::pi::ar::init(), cg::pi::ar::run()
*/
int main(int argc, char **argv) {

	//  GLUT initialization
	glutInit(&argc, argv);

	glutInitDisplayMode(
		GLUT_DEPTH |
		GLUT_DOUBLE |
		GLUT_RGBA |
		GLUT_MULTISAMPLE )
		;

	// Set context
	glutInitContextVersion(3,3);

	glutInitWindowPosition(600,100);
	glutInitWindowSize(640,360);
	glutCreateWindow("Teapot World");


	//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutTimerFunc( 33 , timeElapsed , 0 );

	//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(keyPress);
	glutKeyboardUpFunc(keyUp);

	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS)
		;

	//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	if (!glewIsSupported("GL_VERSION_3_3")) {
		printf("OpenGL 3.3 not supported\n");
		getchar();
		exit(1);
	}

	VSGLInfoLib::getGeneralInfo();

	initCubeShaders();
	init();
	initVSL();

	cg::pi::ar::init(glTransformationMatrix);
	cg::pi::ar::run();

	return 0;

}