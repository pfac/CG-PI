//
// Lighthouse3D.com VS*L OpenGL Sample
//
// Loading and displaying a Textured Model
//
// Uses:
//  Assimp library for model loading
//		http://assimp.sourceforge.net/
//  Devil for image loading
//		http://openil.sourceforge.net/
//  GLEW for OpenGL post 1.1 functions
//		http://glew.sourceforge.net/
//	TinyXML for font definition parsing
//		http://sourceforge.net/projects/tinyxml/
//
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
//
// If you do use it I would love to hear about it. 
// Just post a comment at Lighthouse3D.com

// Have Fun :-)

#ifndef _DEBUG
#pragma comment (lib,"l3dvsl")
#else
#pragma comment (lib,"l3dvsld")
#endif

#define PATTERN_CONTROL

#include <math.h>
#include <fstream>
#include <map>

#include <string>
using std::string;

#include <vector>


// include GLEW to access OpenGL 3.3 functions
#include <GL/glew.h>

// GLUT is the toolkit to interface with the OS
#include <GL/freeglut.h>

// Use Very Simple Libs
#include <VS/vslibs.h>

#include "camera.h"
#include "blaf.h"

#include "ar.h"

#define SHADER_GROUP_COUNT 3
#define SHADER_COUNT 4


std::ostream *f;
VSMathLib *vsml;
//VSShaderLib shader,shaderTess;
VSShaderLib shaders[ SHADER_GROUP_COUNT ][ SHADER_COUNT ];
VSFontLib vsfl2, vsfl;
VSResModelLib teapot;
unsigned int aSentence, profileSentence;

// Query to track the number of primitives
// issued by the tesselation shaders
GLuint counterQ;
unsigned int primitiveCounter = 0;

// Camera Position
float camX = 0, camY = 0, camZ = 5;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 0.0f, beta = 0.0f;
float r = 5.0f;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];

unsigned shader_now, shader_group_now;

#include "player.h"
using cg::Player;
Player *player;

#include "keyboard.h"

#include "camera.h"
using cg::Camera;
Camera *cam;

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
	//vsml->lookAt(camX, camY, camZ, 0,0,0, 0,1,0);
	cam->lookAt(vsml);
	
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
		glUseProgram(shaders[shader_group_now][shader_now].getProgramIndex());
		// start counting primitives
		// render array of models
		for (float x = -2.0f ; x < 3.0f ; x += 2.0f) {
			for (float z = -2.0f; z < 3.0f ; z += 2.0f) {
				vsml->pushMatrix(VSMathLib::MODEL);
				vsml->translate(VSMathLib::MODEL, x, 0.0f, z);
				if ( x < -1.0f || x > 1.0f || z < -1.0f || z > 1.0f )
					teapot.render();
				vsml->popMatrix(VSMathLib::MODEL);
			}
		}
		// stop counting primitives
		/*
		{
			glUseProgram(player.shaders[shader_group_now].getProgramIndex());
			vsml->pushMatrix(VSMathLib::MODEL);
			vsml->translate(VSMathLib::MODEL, player.position.x, player.position.y, player.position.z);
			teapot.render();
			vsml->popMatrix(VSMathLib::MODEL);
		}
		*/
		{
			player->draw(vsml);
		}
	}

	glutSwapBuffers();
}

#ifdef	PATTERN_CONTROL
void timeElapsed(int value)
{
	glutPostRedisplay();
	glutTimerFunc( 33 , timeElapsed , 0 );
}
#endif


// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy)
{
	switch(key) {

	case 27:

		glutLeaveMainLoop();
		break;

	case 'z': r -= 0.1f;
		camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
		camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
		camY = r *   						     sin(beta * 3.14f / 180.0f);
		break;
	case 'x': r += 0.1f;
		camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
		camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
		camY = r *   						     sin(beta * 3.14f / 180.0f);
		break;
	case 'm': glEnable(GL_MULTISAMPLE); break;
	case 'n': glDisable(GL_MULTISAMPLE); break;
	case 'k': VSProfileLib::Reset(); break;
	case 'p':
		{
			std::string s = VSProfileLib::DumpLevels();
			printf("%s\n", s.c_str());
		}
		break;



	case 'l':
		shader_now = ( shader_now + 1 ) % SHADER_COUNT;
		//printf("Current shader: %d\n", shader_now);
		return;
	case 'L':
		if (shader_now)
			shader_now = ( shader_now - 1 ) % SHADER_COUNT;
		else
			shader_now = SHADER_COUNT - 1;
		return;
	case 'o':
		shader_group_now = ( shader_group_now + 1 ) % SHADER_GROUP_COUNT;
		return;






	}
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

	//  uncomment this if not using an idle func
	//	glutPostRedisplay();
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy)
{
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON)
			tracking = 1;
		else if (button == GLUT_RIGHT_BUTTON)
			tracking = 2;
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else if (tracking == 2) {
			r += (yy - startY) * 0.01f;
		}
		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed
void processMouseMotion(int xx, int yy)
{

	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX =  - xx + startX;
	deltaY =    yy - startY;

	// left mouse button: move camera
	if (tracking == 1) {


		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f)
			betaAux = 85.0f;
		else if (betaAux < -85.0f)
			betaAux = -85.0f;

		rAux = r;

		camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);
	}
	// right mouse button: zoom
	else if (tracking == 2) {

		alphaAux = alpha;
		betaAux = beta;
		rAux = r + (deltaY * 0.01f);

		camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);
	}


	//  uncomment this if not using an idle func
	//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {

	r += direction * 0.1f;
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

	//  uncomment this if not using an idle func
	//	glutPostRedisplay();
}


// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {

	// Shader for fonts and models
	shaders[0][0].init();
	/*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	shaders[0][0].loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/direct/diffuse.vert.glsl")
		;
	shaders[0][0].loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/direct/diffuse.frag.glsl")
		;

	// set semantics for the shader variables
	shaders[0][0].setProgramOutput( 0 , "color" );
	shaders[0][0].setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	shaders[0][0].setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	shaders[0][0].prepareProgram();

	VSGLInfoLib::getProgramInfo(shaders[0][0].getProgramIndex());
	printf("%s\n", shaders[0][0].getAllInfoLogs().c_str());

	//	uniforms
	//float lpos[4] = { 1.0 , 0.0 , 1.0 , 0.0 };
	//shaders[0].setUniform( "lightPos" , lpos );
	{
		float ldir[3] = { 1.0 , 0.0 , 0.0 };
		float ldif[3] = { 1.0 , 0.0 , 0.0 };
		shaders[0][0].setUniform( "light_dir" , ldir );
		shaders[0][0].setUniform( "diffuse" , ldif );
	}








	//	shader#2
	// Shader for fonts and models
	shaders[0][1].init();
	/*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	shaders[0][1].loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/direct/difamb.vert.glsl")
		;
	shaders[0][1].loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/direct/difamb.frag.glsl")
		;

	// set semantics for the shader variables
	shaders[0][1].setProgramOutput( 0 , "color" );
	shaders[0][1].setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	shaders[0][1].setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	shaders[0][1].prepareProgram();

	VSGLInfoLib::getProgramInfo(shaders[0][1].getProgramIndex());
	printf("%s\n", shaders[0][1].getAllInfoLogs().c_str());

	//	uniforms
	{
		float ldir[3] = { 1.0f , 0.0f , 0.0f };
		float ldif[3] = { 1.0f , 0.0f , 0.0f };
		float lamb[3] = { 0.3f , 0.0f , 0.0f };
		shaders[0][1].setUniform( "light_dir" , ldir );
		shaders[0][1].setUniform( "diffuse" , ldif );
		shaders[0][1].setUniform( "ambient" , lamb );
	}
	







	//	shader#3
	// Shader for fonts and models
	shaders[0][2].init();
	/*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	shaders[0][2].loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/direct/gouraud.vert.glsl")
		;
	shaders[0][2].loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/direct/gouraud.frag.glsl")
		;

	// set semantics for the shader variables
	shaders[0][2].setProgramOutput( 0 , "color" );
	shaders[0][2].setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	shaders[0][2].setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	shaders[0][2].prepareProgram();

	VSGLInfoLib::getProgramInfo(shaders[0][2].getProgramIndex());
	printf("%s\n", shaders[0][2].getAllInfoLogs().c_str());

	//	uniforms
	{
		float ldir[3] = { 1.0f , 0.0f , 0.0f };
		float ldif[3] = { 1.0f , 0.0f , 0.0f };
		float lamb[3] = { 0.3f , 0.0f , 0.0f };
		float lspc[3] = { 1.0f , 1.0f , 1.0f };
		shaders[0][2].setUniform( "light_dir" , ldir );
		shaders[0][2].setUniform( "diffuse" , ldif );
		shaders[0][2].setUniform( "ambient" , lamb );
		shaders[0][2].setUniform( "specular" , lspc );
		shaders[0][2].setUniform( "shininess" , 100.0f );
	}




	//	shader#4
	// Shader for fonts and models
	shaders[0][3].init();
	/*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	shaders[0][3].loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/direct/phong.vert.glsl")
		;
	shaders[0][3].loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/direct/phong.frag.glsl")
		;

	// set semantics for the shader variables
	shaders[0][3].setProgramOutput( 0 , "color" );
	shaders[0][3].setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	shaders[0][3].setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	shaders[0][3].prepareProgram();

	VSGLInfoLib::getProgramInfo(shaders[0][3].getProgramIndex());
	printf("%s\n", shaders[0][3].getAllInfoLogs().c_str());

	//	uniforms
	{
		float ldir[3] = { 1.0f , 0.0f , 0.0f };
		float ldif[3] = { 1.0f , 0.0f , 0.0f };
		float lamb[3] = { 0.3f , 0.0f , 0.0f };
		float lspc[3] = { 1.0f , 1.0f , 1.0f };
		shaders[0][3].setUniform( "light_dir" , ldir );
		shaders[0][3].setUniform( "diffuse" , ldif );
		shaders[0][3].setUniform( "ambient" , lamb );
		shaders[0][3].setUniform( "specular" , lspc );
		shaders[0][3].setUniform( "shininess" , 100.0f );
	}





	//	shader#5
	// Shader for fonts and models
	shaders[1][3].init();
	/*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	shaders[1][3].loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/point/phong.vert.glsl")
		;
	shaders[1][3].loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/point/phong.frag.glsl")
		;

	// set semantics for the shader variables
	shaders[1][3].setProgramOutput( 0 , "color" );
	shaders[1][3].setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	shaders[1][3].setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	shaders[1][3].prepareProgram();

	VSGLInfoLib::getProgramInfo(shaders[1][3].getProgramIndex());
	printf("%s\n", shaders[1][3].getAllInfoLogs().c_str());

	//	uniforms
	{
		float lpos[4] = { 30.0 , 5.0 , 0.0 , 1.0 };
		float ldif[3] = { 1.0 , 0.0 , 0.0 };
		//float lamb[3] = { 0.3 , 0.0 , 0.0 };
		float lspc[3] = { 1.0 , 1.0 , 1.0 };
		shaders[1][3].setUniform( "lightPosition" , lpos );
		shaders[1][3].setUniform( "diffuse" , ldif );
		//shaders[1][3].setUniform( "ambient" , lamb );
		shaders[1][3].setUniform( "specular" , lspc );
		shaders[1][3].setUniform( "shininess" , 100.0f );
	}





	//	shader#6
	// Shader for fonts and models
	shaders[2][3].init();
	/*shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");*/
	shaders[2][3].loadShader(
		VSShaderLib::VERTEX_SHADER,
		"../shaders/light/spot/phong.vert.glsl")
		;
	shaders[2][3].loadShader(
		VSShaderLib::FRAGMENT_SHADER,
		"../shaders/light/spot/phong.frag.glsl")
		;

	// set semantics for the shader variables
	shaders[2][3].setProgramOutput( 0 , "color" );
	shaders[2][3].setVertexAttribName(
		VSShaderLib::VERTEX_COORD_ATTRIB,
		"position")
		;
	//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
	shaders[2][3].setVertexAttribName(
		VSShaderLib::NORMAL_ATTRIB,
		"normal")
		;

	shaders[2][3].prepareProgram();

	VSGLInfoLib::getProgramInfo(shaders[2][3].getProgramIndex());
	printf("%s\n", shaders[2][3].getAllInfoLogs().c_str());

	//	uniforms
	{
		float spos[4] = { 0.0 , 1.0 , 0.0 , 1.0 };
		float sdir[3] = { 1.0 , -1.0 , 0.0 };
		float ldif[3] = { 1.0 , 0.0 , 0.0 };
		//float lamb[3] = { 0.3 , 0.0 , 0.0 };
		float lspc[3] = { 1.0 , 1.0 , 1.0 };
		shaders[2][3].setUniform( "spotPosition" , spos );
		shaders[2][3].setUniform( "spotDirection" , sdir );
		shaders[2][3].setUniform( "spotLimitAngle" , blaf::deg2rad(7.5f) );
		shaders[2][3].setUniform( "diffuse" , ldif );
		//shaders[1][3].setUniform( "ambient" , lamb );
		shaders[2][3].setUniform( "specular" , lspc );
		shaders[2][3].setUniform( "shininess" , 100.0f );
	}





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




	




	return(1);
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

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	return 1;
}


void initVSL() {

	// Init VSML
	vsml = VSMathLib::getInstance();
	//vsml->setUniformBlockName("Matrices");
	vsml->setUniformName(VSMathLib::PROJ_VIEW_MODEL, "pvm");
	//vsml->setUniformName(VSMathLib::NORMAL, "normalMat");
	vsml->setUniformName(VSMathLib::NORMAL, "normalMatrix");
	vsml->setUniformName(VSMathLib::VIEW_MODEL, "vm");
	vsml->setUniformName(VSMathLib::VIEW, "viewMatrix");
}

//////////////////////////////////////////////////////////////////////////
// Cleanup
//
void cleanup()
{
	delete player;
}

// ------------------------------------------------------------
//
// Main function
//


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
	glutInitContextVersion (3, 3);
	//glutInitContextProfile (GLUT_CORE_PROFILE );

	glutInitWindowPosition(100,100);
	glutInitWindowSize(640,360);
	glutCreateWindow("Teapot World");


	//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
#ifndef	PATTERN_CONTROL
	glutIdleFunc(renderScene);
#else
	glutTimerFunc( 1000 , timeElapsed , 0 );
#endif

	//	Mouse and Keyboard Callbacks
	//glutKeyboardFunc(processKeys);
	{
		glutKeyboardFunc(cg::keyboard::keyDown);
		glutKeyboardUpFunc(cg::keyboard::keyUp);
	}
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc ( mouseWheel ) ;

	//	return from main loop
	glutSetOption(
		GLUT_ACTION_ON_WINDOW_CLOSE,
		GLUT_ACTION_GLUTMAINLOOP_RETURNS)
		;

	//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();
	if (!glewIsSupported("GL_VERSION_3_3")) {
		printf("OpenGL 3.3 not supported\n");
		exit(1);
	}

	VSGLInfoLib::getGeneralInfo();

	setupShaders();
	if (!init())
		printf("Could not Load the Model\n");
	initVSL();

#ifdef	PATTERN_CONTROL
	cg::pi::ar::init(player);
	cg::pi::ar::run();
#else
	//  GLUT main loop
	glutMainLoop();
#endif

	cleanup();

	return 0;

}