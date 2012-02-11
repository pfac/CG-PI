#pragma once
#ifndef ___PLAYER_H___
#define ___PLAYER_H___

//	Very Simple Library
#include <VS/vsMathLib.h>//		Matrix utilities (to draw)
#include <VS/vsResModelLib.h>//	Model
#include <VS/vsShaderLib.h>//	Shader

//	Basic Linear Algebra Framework
#include "blaf.h"
using blaf::space::CartesianPoint;
using blaf::space::CylindricalPoint;

namespace cg
{
	class Player : public CartesianPoint
	{
		static float defaultVelocity;
		static float defaultAngularVelocity;
		static CylindricalPoint defaultDirection;

		VSResModelLib _model;
		VSShaderLib _shader;
		float _velocity;
		float _w;//	angular velocity
		int _t0;
		int _wt0;//	angular movement start
		float _movement;
		float _wf;//	angular movement factor

		void initModels();
		void initShaders();

		void move(float movement);
		void turn(float factor);
		void refresh();
	public:
		CylindricalPoint direction;

		Player();

		//	movement
		void forward();
		void backward();
		void stop();
		void left();
		void right();
		void straight();

		//	draw
		void draw(VSMathLib *vsml);

		//	getters
		float x();
		float y();
		float z();

		//	setters
	};
}

#endif//___PLAYER_H___