//	basic linear algebra framework
#include <math.h>

#include "blaf.h"

namespace blaf
{
	namespace space
	{
		//////////////////////////////////////////////////////////////////////////
		//	Generic Point
		//////////////////////////////////////////////////////////////////////////
		//	core
		float Point::length() const
		{
			float x = this->x();
			float y = this->y();
			float z = this->z();
			return sqrt( x*x + y*y + z*z );
		}

		Point& Point::normalize()
		{
			*this /= this->length();
			return *this;
		}

		//	operators
		bool Point::operator==(const Point& p) const
		{
			return p.x() == this->x()
				&& p.y() == this->y()
				&& p.z() == this->z()
				;
		}

		bool Point::operator!=(const Point& p) const
		{
			return !( *this == p );
		}

		bool Point::operator<(const Point& p) const
		{
			return this->length() < p.length();
		}

		bool Point::operator>(const Point& p) const
		{
			return this->length() > p.length();
		}

		bool Point::operator<=(const Point& p) const
		{
			return *this < p || *this == p;
		}

		bool Point::operator>=(const Point& p) const
		{
			return *this > p || *this == p;
		}

		std::ostream& operator<<(std::ostream& out, const Point& p)
		{
			out
				<<	'('
				<<	p.x()
				<<	','
				<<	p.y()
				<<	','
				<<	p.z()
				<<	')';
			return out;
		}



		//////////////////////////////////////////////////////////////////////////
		//	Cartesian Point
		//////////////////////////////////////////////////////////////////////////
		//	constructors
		CartesianPoint::CartesianPoint()
			: _x(0)
			, _y(0)
			, _z(0)
		{}

		CartesianPoint::CartesianPoint(
			const float x,
			const float y,
			const float z)
			: _x(x)
			, _y(y)
			, _z(z)
		{}

		CartesianPoint::CartesianPoint(
			const float xyz[])
			: _x(xyz[0])
			, _y(xyz[1])
			, _z(xyz[2])
		{}

		CartesianPoint::CartesianPoint(
			const Point& p)
			: _x(p.x())
			, _y(p.y())
			, _z(p.z())
		{}

		//	getters
		float CartesianPoint::x() const
		{
			return _x;
		}

		float CartesianPoint::y() const
		{
			return _y;
		}

		float CartesianPoint::z() const
		{
			return _z;
		}

		//	setters
		void CartesianPoint::x(const float x)
		{
			_x = x;
		}

		void CartesianPoint::y(const float y)
		{
			_y = y;
		}

		void CartesianPoint::z(const float z)
		{
			_z = z;
		}

		void CartesianPoint::xyz(
			const float x,
			const float y,
			const float z)
		{
			this->x(x);
			this->y(y);
			this->z(z);
		}

		void CartesianPoint::xyz(const float xyz[])
		{
			this->xyz( xyz[0] , xyz[1] , xyz[2] );
		}

		void CartesianPoint::set(const Point& p)
		{
			this->xyz( p.x() , p.y() , p.z() );
		}



		//	operators
		CartesianPoint& CartesianPoint::operator=(const Point& p)
		{
			this->set(p);
			return *this;
		}

		Point& CartesianPoint::operator+=(const Point& p)
		{
			this->x( this->x() + p.x() );
			this->y( this->y() + p.y() );
			this->z( this->z() + p.z() );
			return *this;
		}

		Point& CartesianPoint::operator-=(const Point& p)
		{
			this->x( this->x() - p.x() );
			this->y( this->y() - p.y() );
			this->z( this->z() - p.z() );
			return *this;
		}

		Point& CartesianPoint::operator-=(const CartesianPoint& p)
		{
			*this += ( -p );
			return *this;
		}

		Point& CartesianPoint::operator*=(const float f)
		{
			this->x( this->x() * f );
			this->y( this->y() * f );
			this->z( this->z() * f );
			return *this;
		}

		Point& CartesianPoint::operator/=(const float f)
		{
			*this *= ( 1 / f );
			return *this;
		}

		CartesianPoint CartesianPoint::operator-() const
		{
			CartesianPoint r(*this);
			r *= -1.0f;
			return r;
		}

		CartesianPoint CartesianPoint::operator+(const Point& p)
		{
			CartesianPoint r(*this);
			r += p;
			return r;
		}

		CartesianPoint CartesianPoint::operator-(const Point& p)
		{
			CartesianPoint r(*this);
			r -= p;
			return r;
		}

		CartesianPoint CartesianPoint::operator*(const float f)
		{
			CartesianPoint r(*this);
			r *= f;
			return r;
		}

		CartesianPoint CartesianPoint::operator/(const float f)
		{
			CartesianPoint r(*this);
			r /= f;
			return r;
		}

		//std::ostream& operator<<(std::ostream& out, const CartesianPoint& p)
		//{
		//	const Point& sp = p;
		//	out
		//		<<	sp;
		//	return out;
		//}



											//////////////////////////////////////////////////////////////////////////
		//	Cylindrical
		//////////////////////////////////////////////////////////////////////////
		//	constructor
		CylindricalPoint::CylindricalPoint()
			: _radius(0)
			, _y(0)
			, _teta(0)
		{}

		CylindricalPoint::CylindricalPoint(
			const float radius,
			const float y,
			const float teta)
			: _radius(radius)
			, _y(y)
			, _teta(teta)
		{}

		CylindricalPoint::CylindricalPoint(const CylindricalPoint& p)
			: _radius( p.radius() )
			, _y( p.y() )
			, _teta( p.teta() )
		{}

		//	core
		float CylindricalPoint::length() const
		{
			float r = this->radius();
			float y = this->y();
			return sqrt( r*r + y*y );
		}

		//	getters
		float CylindricalPoint::radius() const
		{
			return _radius;
		}

		float CylindricalPoint::y() const
		{
			return _y;
		}

		float CylindricalPoint::teta() const
		{
			return _teta;
		}

		float CylindricalPoint::x() const
		{
			return sin( this->teta() ) * this->radius();
		}

		float CylindricalPoint::z() const
		{
			return cos( this->teta() ) * this->radius();
		}

		//	setters
		void CylindricalPoint::radius(const float radius)
		{
			if ( radius < 0 )
			{
				_radius = ( - radius );
				float teta = this->teta();
				if ( teta >= fPI )
					this->teta( teta - fPI );
				else
					this->teta( teta + fPI );
			}
			else
				_radius = radius;
		}

		void CylindricalPoint::y(const float y)
		{
			_y = y;
		}

		void CylindricalPoint::teta(const float teta)
		{
			float t = teta;
			if ( t < 0.0f )
			{
				t += f2PI;
				while ( t < 0.0f )
					t += f2PI;
			}
			else
				while ( t > f2PI )
					t -= f2PI;
			_teta = t;
		}

		void CylindricalPoint::set(const CylindricalPoint& p)
		{
			this->radius( p.radius() );
			this->y( p.y() );
			this->teta( p.teta() );
		}

		//	operators
		CylindricalPoint& CylindricalPoint::operator=(const CylindricalPoint& p)
		{
			this->set( p );
		}

		Point& CylindricalPoint::operator+=(const Point& p)
		{
			//	TODO
			return *this;
		}

		Point& CylindricalPoint::operator-=(const Point& p)
		{
			//	TODO
			return *this;
		}

		Point& CylindricalPoint::operator*=(const float f)
		{
			this->radius( this->radius() * f );
			this->y( this->y() * f );
			return *this;
		}

		Point& CylindricalPoint::operator/=(const float f)
		{
			*this *= ( 1 / f );
			return *this;
		}

		CylindricalPoint CylindricalPoint::operator-() const
		{
			CylindricalPoint p(*this);
			float teta = p.teta();
			if ( teta >= fPI )
				p.teta( teta - fPI );
			else
				p.teta( teta - fPI );
			p.y( - p.y() );
			return p;
		}

		CylindricalPoint CylindricalPoint::operator*(const float f)
		{
			CylindricalPoint r(*this);
			r *= f;
			return r;
		}

		//std::ostream& operator<<(std::ostream& out, const CylindricalPoint& p)
		//{
		//	const Point& sp = p;
		//	out
		//		<<	sp;
		//	return out;
		//}

		

		//////////////////////////////////////////////////////////////////////////
		//	Spherical Point
		//////////////////////////////////////////////////////////////////////////
		//	constructor
		SphericalPoint::SphericalPoint()
			: _radial(0)
			, _azimuthal(0)
			, _polar(0)
		{}

		SphericalPoint::SphericalPoint(
			const float radial,
			const float azimuthal,
			const float polar)
			: _radial(radial)
			, _azimuthal(azimuthal)
			, _polar(polar)
		{}

		//	getters
		float SphericalPoint::radial() const
		{
			return _radial;
		}

		float SphericalPoint::azimuthal() const
		{
			return _azimuthal;
		}

		float SphericalPoint::polar() const
		{
			return _polar;
		}

		float SphericalPoint::x() const
		{
			return this->radial() * cos( this->polar() ) * sin( this->azimuthal() );
		}

		float SphericalPoint::y() const
		{
			return this->radial() * sin( this->polar() );
		}

		float SphericalPoint::z() const
		{
			return this->radial() * cos( this->polar() ) * cos( this->azimuthal() );
		}

		//	setters
		void SphericalPoint::radial(const float radial)
		{
			if ( radial < 0.0f )
			{
				_radial = ( - radial );
				float azimuthal = this->azimuthal();
				if ( azimuthal < fPI )
					this->azimuthal( azimuthal + fPI );
				else
					this->azimuthal( azimuthal - fPI );
				this->polar( - this->polar() );
			}
			else
				_radial = radial;
		}

		void SphericalPoint::azimuthal(const float azimuthal)
		{
			float a = azimuthal;
			if ( a < 0.0f )
			{
				do 
				{
					a += f2PI;
				} while ( a < 0.0f );
			}
			else
				while ( a > f2PI )
					a -= f2PI;
			_azimuthal = a;
		}

		void SphericalPoint::polar(const float polar)
		{
			float p;
			if ( polar > fPI2 )
				p = fPI2;
			else if ( polar < (- fPI2) )
				p = (- fPI2);
			else
				p = polar;
		}

		void SphericalPoint::set(
			const float radial,
			const float azimuthal,
			const float polar)
		{
			this->radial( radial );
			this->azimuthal( azimuthal );
			this->polar( polar );
		}



		//
		//	operators
		//
		Point& SphericalPoint::operator+=(const Point& p)
		{
			//	TODO
			return *this;
		}

		Point& SphericalPoint::operator-=(const Point& p)
		{
			//	TODO
			return *this;
		}

		Point& SphericalPoint::operator*=(const float f)
		{
			this->radial( this->radial() * f );
			return *this;
		}

		Point& SphericalPoint::operator/=(const float f)
		{
			*this *= ( 1 / f );
			return *this;
		}

		//std::ostream& operator<<(std::ostream& out, const SphericalPoint& p)
		//{
		//	out
		//		<<	'('
		//		<<	p.x()
		//		<<	','
		//		<<	p.y()
		//		<<	','
		//		<<	p.z()
		//		<<	')'
		//		;
		//	return out;
		//}
	}
}