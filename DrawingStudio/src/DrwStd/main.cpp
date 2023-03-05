#include "DrwStd/App.h"

#include "Engone/Engone.h"

void run(){
	using namespace engone;
	Engone engone;
	engone.setFlags(Engone::EngoneRenderOnResize);
	engone.addApplication(new App(&engone));
	
	engone.start();
}

int main(int argc, const char** argv) {
	run();
	return 0;
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){
	run();
	return 0;
}