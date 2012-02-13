#include "pattern.h"

#include <AR/ar.h>

namespace cg
{
	namespace pi
	{
		Pattern::Pattern(const string filename)
			: _filename(filename)
		{
			//load file, or exit in case of error
			_id = arLoadPatt( _filename.c_str() );
			if ( _id < 0) {
				cerr
					<<	"Error loading pattern from file '"
					<<	_filename.c_str()
					<<	'\''
					<<	endl;
				exit(1);
			}

			_center[0] = 0.0;
			_center[1] = 0.0;
			_width = 80.0;
		}

		int Pattern::id() const
		{
			return _id;
		}

		string Pattern::filename() const
		{
			return _filename;
		}

		double Pattern::centerX() const
		{
			return _center[0];
		}

		double Pattern::centerY() const
		{
			return _center[1];
		}

		double Pattern::width() const
		{
			return _width;
		}

		bool Pattern::match(ARMarkerInfo marker_info) const
		{
			return _id == marker_info.id;
		}
	}
}