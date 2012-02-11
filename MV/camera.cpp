#include "camera.h"

#include <VS/vsMathLib.h>

namespace cg
{
	//////////////////////////////////////////////////////////////////////////
	//
	//	CAMERA
	//
	//////////////////////////////////////////////////////////////////////////
	//
	//	constructors
	//
	Camera::Camera( Player *target)
		: SphericalPoint(5.0f,0.0f,0.0f)
		, _target(target)
	{}

	//
	//	core
	//
	void Camera::lookAt(VSMathLib *vsml) const
	{
		vsml->lookAt(
			this->x() , this->y() , this->z() ,
			_target->x() , _target->y() , _target->z() ,
			0.0f, 1.0f, 0.0f )
			;
	}



	//
	//	getters
	//
	float Camera::x() const
	{
		float distance = this->radial();
		return _target->x() - _target->direction.x() * distance;
		//if ( _target )
		//	return SphericalPoint::x() + _target->x();
		//else
		//	return SphericalPoint::x();
	}

	float Camera::y() const
	{
		float distance = this->radial();
		return _target->y() - _target->direction.y() * distance;
		//if ( _target )
		//	return SphericalPoint::y() + _target->y();
		//else
		//	return SphericalPoint::y();
	}

	float Camera::z() const
	{
		float distance = this->radial();
		return _target->z() - _target->direction.z() * distance;
		//if ( _target )
		//	return SphericalPoint::z() + _target->z();
		//else
		//	return SphericalPoint::z();
	}



	//
	//	operators
	//
	/*std::ostream& operator<<(std::ostream& out, const Camera& cam)
	{
		const SphericalPoint& sp = p;
		out
			<<	sp;
	}*/
}