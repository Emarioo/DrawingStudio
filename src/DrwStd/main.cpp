#include "DrwStd/App.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#undef STB_IMAGE_WRITE_IMPLEMENTATION
#undef STB_IMAGE_IMPLEMENTATION

void run(){
	using namespace engone;
    
    // TODO: Support command line arguments?
    
    StartApp();
}

int main(int argc, const char** argv) {
	run();
	return 0;
}
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){
	run();
	return 0;
}