#include "ARWrapper.h"

using namespace CGLibs;

int main(int argc, char **argv) {
	
	ARWrapper::init(&argc, argv);
	ARWrapper::run();
}