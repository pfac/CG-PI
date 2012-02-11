#include "player.h"

#include "GL/freeglut.h"

#include <VS/vsGLInfoLib.h>

#define DEG2RAD(d)	\
	( (d) * blaf::fPI * 0.00555555555555555555555555556f )
#define RAD2DEG(r)	\
	( (r) * 180 * blaf::fPIi )

namespace cg
{
	CylindricalPoint Player::defaultDirection( 1.0f , 0.0f , blaf::fPI );
	float Player::defaultVelocity = 10.0f;//	units per second
	float Player::defaultAngularVelocity = DEG2RAD(60.0f);//	rads per second



	//////////////////////////////////////////////////////////////////////////
	//	Instance
	//////////////////////////////////////////////////////////////////////////

	//
	//	Preparation
	//
	void Player::initShaders()
	{
		//	player object shader
		// Shader for fonts and models
		_shader.init();
		//shader.loadShader(VSShaderLib::VERTEX_SHADER, "../shaders/dirlightdiffambpix.vert");
		//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/dirlightdiffambpix.frag");
		_shader.loadShader(
			VSShaderLib::VERTEX_SHADER,
			"../shaders/light/direct/phong.vert.glsl")
			;
		_shader.loadShader(
			VSShaderLib::FRAGMENT_SHADER,
			"../shaders/light/direct/phong.frag.glsl")
			;

		// set semantics for the shader variables
		_shader.setProgramOutput( 0 , "color" );
		_shader.setVertexAttribName(
			VSShaderLib::VERTEX_COORD_ATTRIB,
			"position")
			;
		//shader.setVertexAttribName(VSShaderLib::TEXTURE_COORD_ATTRIB, "texCoord");
		_shader.setVertexAttribName(
			VSShaderLib::NORMAL_ATTRIB,
			"normal")
			;

		_shader.prepareProgram();

		VSGLInfoLib::getProgramInfo(_shader.getProgramIndex());
		printf("%s\n", _shader.getAllInfoLogs().c_str());

		//	uniforms
		{
			float ldir[3] = { 1.0 , 0.0 , 0.0 };
			float spos[4] = { 0.0 , 1.0 , 0.0 , 1.0 };
			float sdir[3] = { 1.0 , -1.0 , 0.0 };
			float ldif[3] = { 0.0 , 1.0 , 0.0 };
			//float lamb[3] = { 0.3 , 0.0 , 0.0 };
			float lspc[3] = { 1.0 , 1.0 , 1.0 };
			_shader.setUniform( "light_dir" , ldir );
			//_shader.setUniform( "spotPosition" , spos );
			//_shader.setUniform( "spotDirection" , sdir );
			//_shader.setUniform( "spotLimitAngle" , DEG2RAD(7.5f) );
			_shader.setUniform( "diffuse" , ldif );
			//shaders[1][3].setUniform( "ambient" , lamb );
			_shader.setUniform( "specular" , lspc );
			_shader.setUniform( "shininess" , 100.0f );
		}
	}

	void Player::initModels()
	{
		// load models
		if ( ! _model.load("../models/teapot.3ds") )
		{
			using std::cerr;
			using std::endl;
			cerr
				<<	_model.getErrors()
				<<	endl;
			getchar();
			exit(1);
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//
	//	PUBLIC
	//
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//
	//	Constructors
	//
	Player::Player()
		: _velocity(Player::defaultVelocity)
		, direction(Player::defaultDirection)
		, _w(Player::defaultAngularVelocity)
		, _movement(0.0f)
		, _wf(0.0f)
	{
		initModels();
		initShaders();
	}



	//
	//	Movement
	//
	void Player::move(float movement)
	{
		this->_t0 = glutGet( GLUT_ELAPSED_TIME );
		this->_movement = movement;
	}

	void Player::turn(float factor)
	{
		this->_wt0 = glutGet( GLUT_ELAPSED_TIME );
		this->_wf = factor;
	}

	void Player::forward()
	{
		this->move(1);
	}

	void Player::backward()
	{
		this->move(-1);
	}

	void Player::stop()
	{
		this->refresh();
		this->_movement = 0;
	}

	void Player::left()
	{
		this->turn(1);
		//std::cout << "Player is going left" << std::endl;
	}

	void Player::right()
	{
		this->turn(-1);
		//std::cout << "player is going right" << std::endl;
	}

	void Player::straight()
	{
		this->refresh();
		this->_wf = 0;
		//std::cout << "player is going straight" << std::endl;
	}

	void Player::refresh()
	{
		if ( this->_movement || this->_wf )
		{
			float dt;
			int t = glutGet( GLUT_ELAPSED_TIME );
			dt = ( t - this->_t0 ) * 0.001f;
			float ds = this->_velocity * dt * this->_movement;
			*this += ( this->direction * ds );
			//*this += ( Player::defaultDirection * ds );
			_t0 = t;

			//std::cout << "w: " << this->_w << std::endl;
			dt = ( t - this->_wt0 ) * 0.001f;
			float da = this->_w * dt * this->_wf;
			//std::cout << "da: " << da << std::endl;
			this->direction.teta( this->direction.teta() + da );
			_wt0 = t;
		}
	}



	//
	//	Draw
	//
	void Player::draw(VSMathLib *vsml)
	{
		/*glUseProgram(player.shaders[shader_group_now].getProgramIndex());
		vsml->pushMatrix(VSMathLib::MODEL);
		vsml->translate(VSMathLib::MODEL, player.position.x, player.position.y, player.position.z);
		teapot.render();
		vsml->popMatrix(VSMathLib::MODEL);*/

		this->refresh();
		glUseProgram( _shader.getProgramIndex() );
		vsml->pushMatrix( VSMathLib::MODEL );
		vsml->translate( VSMathLib::MODEL , this->x() , this->y() , this->z() );
		vsml->rotate(
			VSMathLib::MODEL,
			RAD2DEG( this->direction.teta() ),
			0.0f , 1.0f , 0.0f);
		_model.render();
		vsml->popMatrix( VSMathLib::MODEL );
	}



	//
	//	getters
	//
	float Player::x()
	{
		this->refresh();
		return CartesianPoint::x();
	}

	float Player::y()
	{
		this->refresh();
		return CartesianPoint::y();
	}

	float Player::z()
	{
		this->refresh();
		return CartesianPoint::z();
	}
}