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
						//!<	0 : black
						//!<	1 : blue
						//!<	2 : green
						//!<	3 : cyan
						//!<	4 : red
						//!<	5 : magenta
						//!<	6 : yellow
						//!<	7 : white

VSResModelLib teapot;	//!<	Model used to draw the teapots.



float camDirection[4] = { 0.0f , 0.0f , -1.0f , 0.0f };	//!<	Default camera looking direction.
float camPosition[4] = { 0.0f , 0.0f , 0.0f , 1.0f };	//!<	Camera position, set by default to the world coordinate system origin.
float camUp[4] = { 0.0f , 1.0f , 0.0f , 0.0f };			//!<	Default camera up vector (defines the vertical orientation of the camera).
float camZoom = 5.0f;									//!<	Distance units between the camera and the target.



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
		cg::pi::ar::lockOutput();
		vsml->loadMatrix( VSMathLib::AUX0 , glTransformationMatrix );
		cg::pi::ar::unlockOutput();



		float newCamDirection[4];
		float newCamTarget[4];
		float newCamUp[4];

		vsml->multMatrixPoint( VSMathLib::AUX0 , camDirection , newCamDirection );
		vsml->multMatrixPoint( VSMathLib::AUX0 , camUp , newCamUp );
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



//!	Timer event to refresh the screen.
/*!
	Refreshes the screen every 33 miliseconds to keep a frame rate close to 30 frames per second.

	/param	value	The identifier of the timer which called the event. Ignored in this case, as only one timer is created in the whole application.
*/
void timeElapsed(int value)
{
	glutPostRedisplay();
	glutTimerFunc( 33 , timeElapsed , 0 );
}

//
//// ------------------------------------------------------------
////
//// Events from the Keyboard
////
//
//void processKeys(unsigned char key, int xx, int yy)
//{
//	switch(key) {
//
//	case 27:
//
//		glutLeaveMainLoop();
//		break;
//
//	case 'z': r -= 0.1f;
//		camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//		camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//		camY = r *   						     sin(beta * 3.14f / 180.0f);
//		break;
//	case 'x': r += 0.1f;
//		camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//		camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//		camY = r *   						     sin(beta * 3.14f / 180.0f);
//		break;
//	case 'm': glEnable(GL_MULTISAMPLE); break;
//	case 'n': glDisable(GL_MULTISAMPLE); break;
//	case 'k': VSProfileLib::Reset(); break;
//	case 'p':
//		{
//			std::string s = VSProfileLib::DumpLevels();
//			printf("%s\n", s.c_str());
//		}
//		break;
//
//
//
//	case 'l':
//		shader_now = ( shader_now + 1 ) % SHADER_COUNT;
//		//printf("Current shader: %d\n", shader_now);
//		return;
//	case 'L':
//		if (shader_now)
//			shader_now = ( shader_now - 1 ) % SHADER_COUNT;
//		else
//			shader_now = SHADER_COUNT - 1;
//		return;
//	case 'o':
//		shader_group_now = ( shader_group_now + 1 ) % SHADER_GROUP_COUNT;
//		return;
//
//
//
//
//
//
//	}
//	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//	camY = r *   						     sin(beta * 3.14f / 180.0f);
//
//	//  uncomment this if not using an idle func
//	//	glutPostRedisplay();
//}
//
//
//// ------------------------------------------------------------
////
//// Mouse Events
////
//
//void processMouseButtons(int button, int state, int xx, int yy)
//{
//	// start tracking the mouse
//	if (state == GLUT_DOWN)  {
//		startX = xx;
//		startY = yy;
//		if (button == GLUT_LEFT_BUTTON)
//			tracking = 1;
//		else if (button == GLUT_RIGHT_BUTTON)
//			tracking = 2;
//	}
//
//	//stop tracking the mouse
//	else if (state == GLUT_UP) {
//		if (tracking == 1) {
//			alpha -= (xx - startX);
//			beta += (yy - startY);
//		}
//		else if (tracking == 2) {
//			r += (yy - startY) * 0.01f;
//		}
//		tracking = 0;
//	}
//}
//
//// Track mouse motion while buttons are pressed
//void processMouseMotion(int xx, int yy)
//{
//
//	int deltaX, deltaY;
//	float alphaAux, betaAux;
//	float rAux;
//
//	deltaX =  - xx + startX;
//	deltaY =    yy - startY;
//
//	// left mouse button: move camera
//	if (tracking == 1) {
//
//
//		alphaAux = alpha + deltaX;
//		betaAux = beta + deltaY;
//
//		if (betaAux > 85.0f)
//			betaAux = 85.0f;
//		else if (betaAux < -85.0f)
//			betaAux = -85.0f;
//
//		rAux = r;
//
//		camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
//		camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
//		camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);
//	}
//	// right mouse button: zoom
//	else if (tracking == 2) {
//
//		alphaAux = alpha;
//		betaAux = beta;
//		rAux = r + (deltaY * 0.01f);
//
//		camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
//		camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
//		camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);
//	}
//
//
//	//  uncomment this if not using an idle func
//	//	glutPostRedisplay();
//}
//
//
//void mouseWheel(int wheel, int direction, int x, int y) {
//
//	r += direction * 0.1f;
//	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
//	camY = r *   						     sin(beta * 3.14f / 180.0f);
//
//	//  uncomment this if not using an idle func
//	//	glutPostRedisplay();
//}
//
//



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
	//glutKeyboardFunc(processKeys);
	//{
	//glutKeyboardFunc(cg::keyboard::keyDown);
	//glutKeyboardUpFunc(cg::keyboard::keyUp);
	//}
	//glutMouseFunc(processMouseButtons);
	//glutMotionFunc(processMouseMotion);
	//glutMouseWheelFunc ( mouseWheel ) ;

	////	return from main loop
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