/************************************************************************/
/* BLAF: Basic Linear Algebra Framework                                 */
/************************************************************************/
#pragma once

#include <iostream>

namespace blaf
{
	const double	dPIi	= 0.31830988618379067153776752674502872406891929148091;		//!	The constant value of 1/pi .
	const float		fPIi	= 0.31830988618379067153776752674502872406891929148091f;	//!	The constant value of 1/pi .
	const float		fPI2	= 1.5707963267948966192313216916397514420985846996876f;		//!	The constant value of pi/2 .
	const double	dPI		= 3.1415926535897932384626433832795028841971693993751;		//!	The constant value of pi.
	const float		fPI		= 3.1415926535897932384626433832795028841971693993751f;		//!	The constant value of pi.
	const float		f2PI	= 6.283185307179586476925286766559004f;						//!	The constant value of 2*pi .
	const double	d180i	= 0.0055555555555555555555555555555555555555555555555556;	//!	The constant value of 1/180 .
	const float		f180i	= 0.0055555555555555555555555555555555555555555555555556f;	//!	The constant value of 1/180 .

	//!	Convert radians to degrees.
	/*!
		/param	radians	The angle value, in radians, to convert.
	*/
	double	rad2deg( const double radians );

	//!	Convert radians to degrees.
	/*!
		/param	radians	The angle value, in radians, to convert.
	*/
	float	rad2deg( const float radians );

	//!	Convert degrees to radians.
	/*!
		/param	degrees	The angle value, in degrees, to convert.
	*/
	double	deg2rad( const double degrees );

	//!	Convert degrees to radians.
	/*!
		/param	degrees	The angle value, in degrees, to convert.
	*/
	float	deg2rad( const float degrees );
}