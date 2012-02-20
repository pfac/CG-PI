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

//#include <vector>
//

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

#define	TEAPOT_WORLD
#define	PATTERN_CONTROL

//#define SHADER_GROUP_COUNT 3
//#define SHADER_COUNT 4
//
//
//std::ostream *f;
VSMathLib *vsml;
VSShaderLib shader1;
VSShaderLib shader2;

//	teapot cube colors
//	0	black
//	1	blue
//	2	green
//	3	cyan
//	4	red
//	5	magenta
//	6	yellow
//	7	white
VSShaderLib shaders[8];

VSResModelLib teapot;



//
// Camera Position
//
float camX = 0, camY = 5, camZ = 5;

float camDirection[4] = { 0.0f , 0.0f , -1.0f , 0.0f };
float camPosition[4] = { 0.0f , 0.0f , 0.0f , 1.0f };
float camTarget[4];
float camUp[4] = { 0.0f , 1.0f , 0.0f , 0.0f };
float camZoom = 5.0f;


//
//// Mouse Tracking Variables
//int startX, startY, tracking = 0;
//
//// Camera Spherical Coordinates
//float alpha = 0.0f, beta = 0.0f;
//float r = 5.0f;
//
//// Frame counting and FPS computation
//long myTime,timebase = 0,frame = 0;
//char s[32];
//
//unsigned shader_now, shader_group_now;

#include "player.h"
using cg::Player;
Player *player;

#include "keyboard.h"

#include "camera.h"
using cg::Camera;
Camera *cam;


float glEulerAngles[3];
float glTransformationMatrix[16];



// ------------------------------------------------------------
//
// Reshape Callback Function
//
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


// ------------------------------------------------------------
//
// Render stuff
//
void renderScene(void) {

	glClear(
		GL_COLOR_BUFFER_BIT |
		GL_DEPTH_BUFFER_BIT )
		;

	vsml->loadIdentity(VSMathLib::MODEL);
	vsml->loadIdentity(VSMathLib::VIEW);

	{
#ifdef PATTERN_CONTROL
		cg::pi::ar::lockOutput();
		//for ( int i = 0 ; i < 4 ; ++i )
		//{
		//	for ( int j = 0 ; j < 4 ; ++j )
		//	{
		//		int k = j * 4 + i;
		//		cout
		//			<<	glTransformationMatrix[k]
		//			<<	'\t';
		//	}
		//	cout
		//		<<	endl;
		//}
		vsml->loadMatrix( VSMathLib::AUX0 , glTransformationMatrix );
		cg::pi::ar::unlockOutput();
		float newCamDirection[4];
		float newCamUp[4];
		for ( int k = 0 ; k < 4 ; ++k )
		{
			newCamDirection[k] = camDirection[k];
			newCamUp[k] = camUp[k];
		}
		vsml->multMatrixPoint( VSMathLib::AUX0 , camDirection , newCamDirection );
		vsml->multMatrixPoint( VSMathLib::AUX0 , camUp , newCamUp );
		//for ( int k = 0 ; k < 4 ; ++k )
		//{
		//	cout
		//		<<	camDirection[k]
		//	<<	'\t';
		//}
		//cout
		//	<<	endl;
		//getchar();
		for ( int k = 0 ; k < 4 ; ++k )
		{
			camTarget[k] = camPosition[k] + newCamDirection[k] * camZoom;
		}

		vsml->lookAt(
			camPosition[0] , camPosition[1] , camPosition[2] ,
			camTarget[0] , camTarget[1] , camTarget[2] ,
			//camUp[0] , camUp[1] , camUp[2] )
			newCamUp[0] , newCamUp[1] , newCamUp[2] )
			;
#else
		for ( int k = 0 ; k < 4 ; ++k )
		{
			camTarget[k] = camPosition[k] + camDirection[k] * camZoom;
		}

		vsml->lookAt(
			camPosition[0] , camPosition[1] , camPosition[2] ,
			camTarget[0] , camTarget[1] , camTarget[2] ,
			camUp[0] , camUp[1] , camUp[2] )
			;
#endif
	}

	//vsml->lookAt(camX, camY, camZ, 0,0,0, 0,1,0);
	//cam->lookAt(vsml);
	//vsml->lookAt(
	//	0.0f , 0.0f , 0.0f,
	//	0.0f , 0.0f , -1.0f,
	//	0.0f , 1.0f , 0.0f
	//	);

	//cg::pi::ar::lockOutput();
	//{
	//	for ( int i = 0 ; i < 4 ; ++i )
	//	{
	//		for ( int j = 0 ; j < 4 ; ++j )
	//		{
	//			int k = j * 4 + i;
	//			cout
	//				<<	glTransformationMatrix[k]
	//			<<	'\t';
	//		}
	//		cout
	//			<<	endl;
	//	}
	//	//for ( int w = 0 ; w < 3 ; ++w )
	//	//{
	//	//	cout
	//	//		<<	glEulerAngles[w]
	//	//		<<	'\t';
	//	//}
	//	//cout
	//	//	<<	endl;


	//	//float *glModelMatrix;
	//	//float *glModifiedTransformationMatrix;
	//	//glModelMatrix = vsml->get( VSMathLib::MODEL );
	//	//vsml->loadMatrix( VSMathLib::AUX0 , glTransformationMatrix );
	//	//vsml->multMatrix( VSMathLib::AUX0 , glModelMatrix );
	//	//glModifiedTransformationMatrix = vsml->get( VSMathLib::AUX0 );
	//	//vsml->loadMatrix( VSMathLib::MODEL , glModifiedTransformationMatrix );
	//	vsml->loadMatrix( VSMathLib::VIEW , glTransformationMatrix );

	//	//vsml->rotate( VSMathLib::MODEL , glEulerAngles[2] , 0.0f , 0.0f , 1.0f );
	//	//vsml->rotate( VSMathLib::MODEL , glEulerAngles[1] , 0.0f , 1.0f , 0.0f );
	//	//vsml->rotate( VSMathLib::MODEL , glEulerAngles[0] , 1.0f , 0.0f , 0.0f );
	//}
	//cg::pi::ar::unlockOutput();
	
	//std::cout
	//	<<	'('
	//	<<	camX
	//	<<	','
	//	<<	camY
	//	<<	','
	//	<<	camZ
	//	<<	')'
	//	<<	std::endl
	//	<<	*cam
	//	<<	std::endl;
	{

		// set the shader to render models
		//glUseProgram(shaders[shader_group_now][shader_now].getProgramIndex());
		//glUseProgram(shaders[0].getProgramIndex());
		// start counting primitives
		// render array of models
		//for (float x = -2.0f ; x < 3.0f ; x += 2.0f) {
		//	for (float z = -2.0f; z < 3.0f ; z += 2.0f) {
		//		vsml->pushMatrix(VSMathLib::MODEL);
		//		vsml->translate(VSMathLib::MODEL, x, 0.0f, z);
		//		if ( x < -1.0f || x > 1.0f || z < -1.0f || z > 1.0f )
		//			teapot.render();
		//		vsml->popMatrix(VSMathLib::MODEL);
		//	}
		//}



		//	Cube
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





		// stop counting primitives

		//{
		//	//glUseProgram(player.shaders[shader_group_now].getProgramIndex());
		//	glUseProgram(shader2.getProgramIndex());
		//	vsml->pushMatrix(VSMathLib::MODEL);
		//	//vsml->translate(VSMathLib::MODEL, player.position.x, player.position.y, player.position.z);
		//	//vsml->translate(VSMathLib::MODEL, 0, 0, 0);
		//	teapot.render();
		//	vsml->popMatrix(VSMathLib::MODEL);
		//}

		{
#ifdef PATTERN_CONTROL
			//float originalDirection[4];
			//float computedDirection[4];

			//{
			//	originalDirection[0] = player->direction.x();
			//	originalDirection[1] = player->direction.y();
			//	originalDirection[2] = player->direction.z();
			//	originalDirection[3] = 1;
			//}

			//cg::pi::ar::lockOutput();
			////for ( int w = 0 ; w < 3 ; ++w )
			////{
			////	cout
			////		<<	glEulerAngles[w]
			////		<<	'\t';
			////	//}
			//vsml->loadMatrix( VSMathLib::AUX0 , glTransformationMatrix );
			//cg::pi::ar::unlockOutput();

			////	check matrix
			//float *glViewMatrix = vsml->get( VSMathLib::AUX0 );
			//for ( int i = 0 ; i < 4 ; ++i )
			//{
			//	for ( int j = 0 ; j < 4 ; ++j )
			//	{
			//		int k = j * 4 + i;
			//		cout
			//			<<	glViewMatrix[k]
			//		<<	'\t';
			//	}
			//	cout
			//		<<	endl;
			//}

			//vsml->multMatrixPoint( VSMathLib::AUX0 , originalDirection , computedDirection );
			//player->direction.x( computedDirection[0] );
			//player->direction.y( computedDirection[1] );
			//player->direction.z( computedDirection[2] );
			//player->direction.normalize();
#endif

			//glUseProgram( shaders[0].getProgramIndex() );
			////player->draw(vsml);
			//vsml->pushMatrix( VSMathLib::MODEL );
			//vsml->translate( VSMathLib::MODEL , 0.0 , 0.0 , 0.0 );
			//teapot.render();
			//vsml->popMatrix( VSMathLib::MODEL );
		}
	}

	glutSwapBuffers();
}




//#ifdef	PATTERN_CONTROL
void timeElapsed(int value)
{
	glutPostRedisplay();
	glutTimerFunc( 33 , timeElapsed , 0 );
}
//#endif

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



// --------------------------------------------------------
//
// Shader Stuff
//
GLuint setupShaders() {

	//// Shader for fonts and models
	//shaders[0][0].init();
	///*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	//shaders[0][0].loadShader(
	//	VSShaderLib::VERTEX_SHADER,
	//	"../shaders/light/direct/diffuse.vert.glsl")
	//	;
	//shaders[0][0].loadShader(
	//	VSShaderLib::FRAGMENT_SHADER,
	//	"../shaders/light/direct/diffuse.frag.glsl")
	//	;

	//// set semantics for the shader variables
	//shaders[0][0].setProgramOutput( 0 , "color" );
	//shaders[0][0].setVertexAttribName(
	//	VSShaderLib::VERTEX_COORD_ATTRIB,
	//	"position")
	//	;
	////shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	//shaders[0][0].setVertexAttribName(
	//	VSShaderLib::NORMAL_ATTRIB,
	//	"normal")
	//	;

	//shaders[0][0].prepareProgram();

	//VSGLInfoLib::getProgramInfo(shaders[0][0].getProgramIndex());
	//printf("%s\n", shaders[0][0].getAllInfoLogs().c_str());

	////	uniforms
	////float lpos[4] = { 1.0 , 0.0 , 1.0 , 0.0 };
	////shaders[0].setUniform( "lightPos" , lpos );
	//{
	//	float ldir[3] = { 1.0 , 0.0 , 0.0 };
	//	float ldif[3] = { 1.0 , 0.0 , 0.0 };
	//	shaders[0][0].setUniform( "light_dir" , ldir );
	//	shaders[0][0].setUniform( "diffuse" , ldif );
	//}








	////	shader#2
	//// Shader for fonts and models
	//shaders[0][1].init();
	///*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	//shaders[0][1].loadShader(
	//	VSShaderLib::VERTEX_SHADER,
	//	"../shaders/light/direct/difamb.vert.glsl")
	//	;
	//shaders[0][1].loadShader(
	//	VSShaderLib::FRAGMENT_SHADER,
	//	"../shaders/light/direct/difamb.frag.glsl")
	//	;

	//// set semantics for the shader variables
	//shaders[0][1].setProgramOutput( 0 , "color" );
	//shaders[0][1].setVertexAttribName(
	//	VSShaderLib::VERTEX_COORD_ATTRIB,
	//	"position")
	//	;
	////shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	//shaders[0][1].setVertexAttribName(
	//	VSShaderLib::NORMAL_ATTRIB,
	//	"normal")
	//	;

	//shaders[0][1].prepareProgram();

	//VSGLInfoLib::getProgramInfo(shaders[0][1].getProgramIndex());
	//printf("%s\n", shaders[0][1].getAllInfoLogs().c_str());

	////	uniforms
	//{
	//	float ldir[3] = { 1.0f , 0.0f , 0.0f };
	//	float ldif[3] = { 1.0f , 0.0f , 0.0f };
	//	float lamb[3] = { 0.3f , 0.0f , 0.0f };
	//	shaders[0][1].setUniform( "light_dir" , ldir );
	//	shaders[0][1].setUniform( "diffuse" , ldif );
	//	shaders[0][1].setUniform( "ambient" , lamb );
	//}
	//







	////	shader#3
	//// Shader for fonts and models
	//shaders[0][2].init();
	///*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	//shaders[0][2].loadShader(
	//	VSShaderLib::VERTEX_SHADER,
	//	"../shaders/light/direct/gouraud.vert.glsl")
	//	;
	//shaders[0][2].loadShader(
	//	VSShaderLib::FRAGMENT_SHADER,
	//	"../shaders/light/direct/gouraud.frag.glsl")
	//	;

	//// set semantics for the shader variables
	//shaders[0][2].setProgramOutput( 0 , "color" );
	//shaders[0][2].setVertexAttribName(
	//	VSShaderLib::VERTEX_COORD_ATTRIB,
	//	"position")
	//	;
	////shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	//shaders[0][2].setVertexAttribName(
	//	VSShaderLib::NORMAL_ATTRIB,
	//	"normal")
	//	;

	//shaders[0][2].prepareProgram();

	//VSGLInfoLib::getProgramInfo(shaders[0][2].getProgramIndex());
	//printf("%s\n", shaders[0][2].getAllInfoLogs().c_str());

	////	uniforms
	//{
	//	float ldir[3] = { 1.0f , 0.0f , 0.0f };
	//	float ldif[3] = { 1.0f , 0.0f , 0.0f };
	//	float lamb[3] = { 0.3f , 0.0f , 0.0f };
	//	float lspc[3] = { 1.0f , 1.0f , 1.0f };
	//	shaders[0][2].setUniform( "light_dir" , ldir );
	//	shaders[0][2].setUniform( "diffuse" , ldif );
	//	shaders[0][2].setUniform( "ambient" , lamb );
	//	shaders[0][2].setUniform( "specular" , lspc );
	//	shaders[0][2].setUniform( "shininess" , 100.0f );
	//}




	////	shader#4
	//// Shader for fonts and models
	//shaders[0][3].init();
	///*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	//shaders[0][3].loadShader(
	//	VSShaderLib::VERTEX_SHADER,
	//	"../shaders/light/direct/phong.vert.glsl")
	//	;
	//shaders[0][3].loadShader(
	//	VSShaderLib::FRAGMENT_SHADER,
	//	"../shaders/light/direct/phong.frag.glsl")
	//	;

	//// set semantics for the shader variables
	//shaders[0][3].setProgramOutput( 0 , "color" );
	//shaders[0][3].setVertexAttribName(
	//	VSShaderLib::VERTEX_COORD_ATTRIB,
	//	"position")
	//	;
	////shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	//shaders[0][3].setVertexAttribName(
	//	VSShaderLib::NORMAL_ATTRIB,
	//	"normal")
	//	;

	//shaders[0][3].prepareProgram();

	//VSGLInfoLib::getProgramInfo(shaders[0][3].getProgramIndex());
	//printf("%s\n", shaders[0][3].getAllInfoLogs().c_str());

	////	uniforms
	//{
	//	float ldir[3] = { 1.0f , 0.0f , 0.0f };
	//	float ldif[3] = { 1.0f , 0.0f , 0.0f };
	//	float lamb[3] = { 0.3f , 0.0f , 0.0f };
	//	float lspc[3] = { 1.0f , 1.0f , 1.0f };
	//	shaders[0][3].setUniform( "light_dir" , ldir );
	//	shaders[0][3].setUniform( "diffuse" , ldif );
	//	shaders[0][3].setUniform( "ambient" , lamb );
	//	shaders[0][3].setUniform( "specular" , lspc );
	//	shaders[0][3].setUniform( "shininess" , 100.0f );
	//}





	////	shader#5
	//// Shader for fonts and models
	//shaders[1][3].init();
	///*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	//shaders[1][3].loadShader(
	//	VSShaderLib::VERTEX_SHADER,
	//	"../shaders/light/point/phong.vert.glsl")
	//	;
	//shaders[1][3].loadShader(
	//	VSShaderLib::FRAGMENT_SHADER,
	//	"../shaders/light/point/phong.frag.glsl")
	//	;

	//// set semantics for the shader variables
	//shaders[1][3].setProgramOutput( 0 , "color" );
	//shaders[1][3].setVertexAttribName(
	//	VSShaderLib::VERTEX_COORD_ATTRIB,
	//	"position")
	//	;
	////shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	//shaders[1][3].setVertexAttribName(
	//	VSShaderLib::NORMAL_ATTRIB,
	//	"normal")
	//	;

	//shaders[1][3].prepareProgram();

	//VSGLInfoLib::getProgramInfo(shaders[1][3].getProgramIndex());
	//printf("%s\n", shaders[1][3].getAllInfoLogs().c_str());

	////	uniforms
	//{
	//	float lpos[4] = { 30.0 , 5.0 , 0.0 , 1.0 };
	//	float ldif[3] = { 1.0 , 0.0 , 0.0 };
	//	//float lamb[3] = { 0.3 , 0.0 , 0.0 };
	//	float lspc[3] = { 1.0 , 1.0 , 1.0 };
	//	shaders[1][3].setUniform( "lightPosition" , lpos );
	//	shaders[1][3].setUniform( "diffuse" , ldif );
	//	//shaders[1][3].setUniform( "ambient" , lamb );
	//	shaders[1][3].setUniform( "specular" , lspc );
	//	shaders[1][3].setUniform( "shininess" , 100.0f );
	//}





	////	shader#6
	//// Shader for fonts and models
	//shaders[2][3].init();
	///*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	//shaders[2][3].loadShader(
	//	VSShaderLib::VERTEX_SHADER,
	//	"../shaders/light/spot/phong.vert.glsl")
	//	;
	//shaders[2][3].loadShader(
	//	VSShaderLib::FRAGMENT_SHADER,
	//	"../shaders/light/spot/phong.frag.glsl")
	//	;

	//// set semantics for the shader variables
	//shaders[2][3].setProgramOutput( 0 , "color" );
	//shaders[2][3].setVertexAttribName(
	//	VSShaderLib::VERTEX_COORD_ATTRIB,
	//	"position")
	//	;
	////shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	//shaders[2][3].setVertexAttribName(
	//	VSShaderLib::NORMAL_ATTRIB,
	//	"normal")
	//	;

	//shaders[2][3].prepareProgram();

	//VSGLInfoLib::getProgramInfo(shaders[2][3].getProgramIndex());
	//printf("%s\n", shaders[2][3].getAllInfoLogs().c_str());

	////	uniforms
	//{
	//	float spos[4] = { 0.0 , 1.0 , 0.0 , 1.0 };
	//	float sdir[3] = { 1.0 , -1.0 , 0.0 };
	//	float ldif[3] = { 1.0 , 0.0 , 0.0 };
	//	//float lamb[3] = { 0.3 , 0.0 , 0.0 };
	//	float lspc[3] = { 1.0 , 1.0 , 1.0 };
	//	shaders[2][3].setUniform( "spotPosition" , spos );
	//	shaders[2][3].setUniform( "spotDirection" , sdir );
	//	shaders[2][3].setUniform( "spotLimitAngle" , blaf::deg2rad(7.5f) );
	//	shaders[2][3].setUniform( "diffuse" , ldif );
	//	//shaders[1][3].setUniform( "ambient" , lamb );
	//	shaders[2][3].setUniform( "specular" , lspc );
	//	shaders[2][3].setUniform( "shininess" , 100.0f );
	//}





	/*
	//	player shader#1
	// Shader for fonts and models
	player.shaders[0].init();
	//shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");
	player.shaders[0].loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/direct/phong.vert.glsl")
		;
	player.shaders[0].loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/direct/phong.frag.glsl")
		;

	// set semantics for the shader variables
	player.shaders[0].setProgramOutput( 0 , "color" );
	player.shaders[0].setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	player.shaders[0].setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	player.shaders[0].prepareProgram();

	VSGLInfoLib::getProgramInfo(player.shaders[0].getProgramIndex());
	printf("%s\n", player.shaders[0].getAllInfoLogs().c_str());

	//	uniforms
	{
		float ldir[3] = { 1.0 , 0.0 , 0.0 };
		float ldif[3] = { 0.0 , 1.0 , 0.0 };
		//float lamb[3] = { 0.3 , 0.0 , 0.0 };
		float lspc[3] = { 1.0 , 1.0 , 1.0 };
		player.shaders[0].setUniform( "light_dir" , ldir );
		player.shaders[0].setUniform( "diffuse" , ldif );
		//shaders[0][3].setUniform( "ambient" , lamb );
		player.shaders[0].setUniform( "specular" , lspc );
		player.shaders[0].setUniform( "shininess" , 100.0f );
	}




	//	player shader#2
	// Shader for fonts and models
	player.shaders[1].init();
	//shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");
	player.shaders[1].loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/point/phong.vert.glsl")
		;
	player.shaders[1].loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/point/phong.frag.glsl")
		;

	// set semantics for the shader variables
	player.shaders[1].setProgramOutput( 0 , "color" );
	player.shaders[1].setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	player.shaders[1].setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	player.shaders[1].prepareProgram();

	VSGLInfoLib::getProgramInfo(player.shaders[1].getProgramIndex());
	printf("%s\n", player.shaders[1].getAllInfoLogs().c_str());

	//	uniforms
	{
		float lpos[4] = { 30.0 , 5.0 , 0.0 , 1.0 };
		float ldif[3] = { 0.0 , 1.0 , 0.0 };
		//float lamb[3] = { 0.3 , 0.0 , 0.0 };
		float lspc[3] = { 1.0 , 1.0 , 1.0 };
		player.shaders[1].setUniform( "lightPosition" , lpos );
		player.shaders[1].setUniform( "diffuse" , ldif );
		//shaders[1][3].setUniform( "ambient" , lamb );
		player.shaders[1].setUniform( "specular" , lspc );
		player.shaders[1].setUniform( "shininess" , 100.0f );
	}





	//	player shader#3
	// Shader for fonts and models
	player.shaders[2].init();
	//shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");
	player.shaders[2].loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/spot/phong.vert.glsl")
		;
	player.shaders[2].loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/spot/phong.frag.glsl")
		;

	// set semantics for the shader variables
	player.shaders[2].setProgramOutput( 0 , "color" );
	player.shaders[2].setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	player.shaders[2].setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	player.shaders[2].prepareProgram();

	VSGLInfoLib::getProgramInfo(player.shaders[2].getProgramIndex());
	printf("%s\n", player.shaders[2].getAllInfoLogs().c_str());

	//	uniforms
	{
		float spos[4] = { 0.0 , 1.0 , 0.0 , 1.0 };
		float sdir[3] = { 1.0 , -1.0 , 0.0 };
		float ldif[3] = { 0.0 , 1.0 , 0.0 };
		//float lamb[3] = { 0.3 , 0.0 , 0.0 };
		float lspc[3] = { 1.0 , 1.0 , 1.0 };
		player.shaders[2].setUniform( "spotPosition" , spos );
		player.shaders[2].setUniform( "spotDirection" , sdir );
		player.shaders[2].setUniform( "spotLimitAngle" , DEG2RAD(7.5f) );
		player.shaders[2].setUniform( "diffuse" , ldif );
		//shaders[1][3].setUniform( "ambient" , lamb );
		player.shaders[2].setUniform( "specular" , lspc );
		player.shaders[2].setUniform( "shininess" , 100.0f );
	}
	*/







	//	shader1
	shader1.init();
	shader1.loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/point/phong.vert.glsl")
		;
	shader1.loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/point/phong.frag.glsl")
		;

	// set semantics for the shader variables
	shader1.setProgramOutput( 0 , "color" );
	shader1.setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	shader1.setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	shader1.prepareProgram();

	VSGLInfoLib::getProgramInfo(shader1.getProgramIndex());
	printf("%s\n", shader1.getAllInfoLogs().c_str());

	//	uniforms
	{
		float lpos[4] = { 30.0 , 5.0 , 0.0 , 1.0 };
		float ldif[3] = { 1.0 , 0.0 , 0.0 };
		//float lamb[3] = { 0.3 , 0.0 , 0.0 };
		float lspc[3] = { 1.0 , 1.0 , 1.0 };
		shader1.setUniform( "lightPosition" , lpos );
		shader1.setUniform( "diffuse" , ldif );
		//shader1.setUniform( "ambient" , lamb );
		shader1.setUniform( "specular" , lspc );
		shader1.setUniform( "shininess" , 100.0f );
	}








	//	shader2
	shader2.init();
	shader2.loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/point/phong.vert.glsl")
		;
	shader2.loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/point/phong.frag.glsl")
		;

	// set semantics for the shader variables
	shader2.setProgramOutput( 0 , "color" );
	shader2.setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	shader2.setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	shader2.prepareProgram();

	VSGLInfoLib::getProgramInfo(shader2.getProgramIndex());
	printf("%s\n", shader2.getAllInfoLogs().c_str());

	//	uniforms
	{
		float lpos[4] = { 30.0 , 5.0 , 0.0 , 1.0 };
		float ldif[3] = { 0.0 , 1.0 , 0.0 };
		//float lamb[3] = { 0.3 , 0.0 , 0.0 };
		float lspc[3] = { 1.0 , 1.0 , 1.0 };
		shader2.setUniform( "lightPosition" , lpos );
		shader2.setUniform( "diffuse" , ldif );
		//shader2.setUniform( "ambient" , lamb );
		shader2.setUniform( "specular" , lspc );
		shader2.setUniform( "shininess" , 100.0f );
	}







	




	return(1);
}



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
			ldif[0] = 1.0;
			ldif[0] = 1.0;
		}
		float lspc[3] = { 1.0 , 1.0 , 1.0 };
		shaders[color].setUniform( "lightPosition" , lpos );
		shaders[color].setUniform( "diffuse" , ldif );
		shaders[color].setUniform( "specular" , lspc );
		shaders[color].setUniform( "shininess" , 100.0f );
	}
}



void initCubeShaders()
{
	for ( unsigned s = 0 ; s < 8 ; ++s )
		initCubeSideShader(s);
}



// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//
int init()
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

	player = new Player();

	cg::keyboard::init(player);

	cam = new Camera(player);

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

	return 1;
}



void initVSL() {
	// Init VSML
	vsml = VSMathLib::getInstance();
	vsml->setUniformName(VSMathLib::PROJ_VIEW_MODEL, "pvm");
	vsml->setUniformName(VSMathLib::NORMAL, "normalMatrix");
	vsml->setUniformName(VSMathLib::VIEW_MODEL, "vm");
	vsml->setUniformName(VSMathLib::VIEW, "viewMatrix");
}




//////////////////////////////////////////////////////////////////////////
// Cleanup
//
void cleanup()
{
	delete cam;
	delete player;
}



// ------------------------------------------------------------
//
// Main function
//
int main(int argc, char **argv) {

	//  GLUT initialization
	glutInit(&argc, argv);

#ifdef	TEAPOT_WORLD
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
//#ifndef	PATTERN_CONTROL
//	glutIdleFunc(renderScene);
//#else
	glutTimerFunc( 1000 , timeElapsed , 0 );
//#endif

	//	Mouse and Keyboard Callbacks
	//glutKeyboardFunc(processKeys);
	//{
	glutKeyboardFunc(cg::keyboard::keyDown);
	glutKeyboardUpFunc(cg::keyboard::keyUp);
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

	setupShaders();
	initCubeShaders();
	init();
	initVSL();

#ifdef	PATTERN_CONTROL
	cg::pi::ar::init(glTransformationMatrix,glEulerAngles);
	cg::pi::ar::run();
#else
	//  GLUT main loop
	glutMainLoop();
#endif

	cleanup();

#else
	cg::pi::ar::init();
	cg::pi::ar::run();
#endif

	return 0;

}