#include "Pattern.h"
#include "ARWrapper.h"

#include <AR/ar.h>

namespace CGLibs {

	Pattern::Pattern(string src_file) {
		//sets src file name
		this->src_file = src_file;
		
		//load file, or exit in case of error
		char *c_name = strdup(src_file.c_str());
		id = arLoadPatt(c_name);
		free(c_name);
		if (id < 0) {
			cout << "error loading pattern Hiro" << endl;
			exit(0);
		}

		this->center[0] = 0.0;
		this->center[1] = 0.0;
		this->width = 80.0;
	}

	int Pattern::getId() {
		return id;
	}

	string Pattern::getName() {
		return src_file;
	}

	double* Pattern::getCenter() {
		return center;
	}

	double Pattern::getWidth() {
		return width;
	}

	bool Pattern::match(ARMarkerInfo marker_info) {
		return this->id == marker_info.id;
	}
}