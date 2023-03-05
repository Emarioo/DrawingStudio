#include "DrwStd/App.h"

#include "Engone/Engone.h"

int main(int argc, const char** argv) {
	using namespace engone;
	
	Engone engone;
	engone.setFlags(Engone::EngoneRenderOnResize);
	engone.addApplication(new App(&engone));
	
	engone.start();
	
	return 0;
}