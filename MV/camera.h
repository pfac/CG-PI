#pragma once

#include <VS/vsMathLib.h>

#include "blaf.h"
#include "player.h"

using blaf::space::SphericalPoint;

namespace cg
{
	class Camera : public SphericalPoint
	{
		Player *_target;
	public:
		Camera(Player *target);

		//	core
		void lookAt(VSMathLib *vsml) const;

		//	getters
		float x() const;
		float y() const;
		float z() const;

		//friend std::ostream& operator<<(std::ostream& out, const Camera& p);
	};
}