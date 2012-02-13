/************************************************************************/
/* BLAF: Basic Linear Algebra Framework                                 */
/************************************************************************/
#pragma once

#include <iostream>

namespace blaf
{
	const double	dPIi	= 0.31830988618379067153776752674502872406891929148091;//	 1/pi
	const float		fPIi	= 0.31830988618379067153776752674502872406891929148091f;//	 1/pi
	const float		fPI2	= 1.5707963267948966192313216916397514420985846996876f;//	 pi/2
	const double	dPI		= 3.1415926535897932384626433832795028841971693993751;//	pi
	const float		fPI		= 3.1415926535897932384626433832795028841971693993751f;//	pi
	const float		f2PI	= 6.283185307179586476925286766559004f;//	2*pi
	const double	d180i	= 0.0055555555555555555555555555555555555555555555555556;//		1/180
	const float		f180i	= 0.0055555555555555555555555555555555555555555555555556f;//	1/180

	double	rad2deg( const double radians );
	float	rad2deg( const float radians );
	double	deg2rad( const double degrees );
	float	deg2rad( const float degrees );

	namespace space
	{
		class Point
		{
		public:
			virtual float length() const;
			virtual Point& normalize();

			//	getters
			virtual float x() const = 0;
			virtual float y() const = 0;
			virtual float z() const = 0;

			//	operators
			virtual Point& operator+=(const Point& p) = 0;
			virtual Point& operator-=(const Point& p) = 0;
			virtual Point& operator*=(const float f) = 0;
			virtual Point& operator/=(const float f) = 0;
			virtual bool operator==(const Point& p) const;
			virtual bool operator!=(const Point& p) const;
			virtual bool operator<(const Point& p) const;
			virtual bool operator>(const Point& p) const;
			virtual bool operator<=(const Point& p) const;
			virtual bool operator>=(const Point& p) const;

			friend std::ostream& operator<<(std::ostream& out, const Point& p);
		};

		class CartesianPoint : public Point
		{
			float _x;
			float _y;
			float _z;

		public:
			//	instance
			CartesianPoint();
			CartesianPoint(const float x, const float y, const float z);
			CartesianPoint(const float xyz[]);
			CartesianPoint(const Point& p);

			//	getters
			virtual float x() const;
			virtual float y() const;
			virtual float z() const;

			//	setters
			void x(const float x);
			void y(const float y);
			void z(const float z);
			void xyz(const float x, const float y, const float z);
			void xyz(const float xyz[]);
			void set(const Point& p);

			//	operators
			CartesianPoint& operator=(const Point& p);
			Point& operator+=(const Point& p);
			Point& operator-=(const Point& p);
			Point& operator-=(const CartesianPoint& p);
			Point& operator*=(const float f);
			Point& operator/=(const float f);
			CartesianPoint operator-() const;
			CartesianPoint operator+(const Point& p);
			CartesianPoint operator-(const Point& p);
			CartesianPoint operator*(const float f);
			CartesianPoint operator/(const float f);

			//friend std::ostream& operator<<(std::ostream& out, const CartesianPoint& p);
		};

		class CylindricalPoint : public Point
		{
			float _radius;
			float _y;
			float _teta;
		public:
			CylindricalPoint();
			CylindricalPoint(
				const float radius,
				const float y,
				const float teta)
				;
			CylindricalPoint(const CylindricalPoint& p);

			//	core
			float length() const;

			//	getters
			float radius() const;
			float y() const;
			float teta() const;

			float x() const;
			float z() const;

			//	setters
			void radius(const float radius);
			void y(const float y);
			void teta(const float teta);
			void set(const CylindricalPoint& p);

			//	operators
			CylindricalPoint& operator=(const CylindricalPoint& p);
			Point& operator+=(const Point& p);
			Point& operator-=(const Point& p);
			Point& operator*=(const float f);
			Point& operator/=(const float f);
			CylindricalPoint operator-() const;
			//CylindricalPoint operator+(const Point& p);
			//CylindricalPoint operator-(const Point& p);
			CylindricalPoint operator*(const float f);
			//CylindricalPoint operator/(const float f);

			//friend std::ostream& operator<<(std::ostream& out, const CylindricalPoint& p);
		};

		class SphericalPoint : public Point
		{
			float _radial;
			float _azimuthal;
			float _polar;

		public:
			SphericalPoint();
			SphericalPoint(const float rap[]);
			SphericalPoint(
				const float radial,
				const float azimuthal,
				const float polar)
				;

			//	getters
			float radial() const;
			float azimuthal() const;
			float polar() const;

			virtual float x() const;
			virtual float y() const;
			virtual float z() const;

			//	setters
			void radial(const float radial);
			void azimuthal(const float azimuthal);
			void polar(const float polar);
			void set(
				const float radial,
				const float azimuthal,
				const float polar)
				;

			//	operators
			SphericalPoint& operator=(const SphericalPoint& p);
			Point& operator+=(const Point& p);
			Point& operator-=(const Point& p);
			Point& operator*=(const float f);
			Point& operator/=(const float f);
			SphericalPoint operator-() const;
			//SphericalPoint operator+(const Point& p);
			//SphericalPoint operator-(const Point& p);
			SphericalPoint operator*(const float f);
			//SphericalPoint operator/(const float f);

			//friend std::ostream& operator<<(std::ostream& out, const SphericalPoint& p);
		};
	}
}