#pragma once

#include <fstream>
#include <string>

#include "Texture.h"
#include "../Utilities.h"

class Font
{
public:
	Font() = default;
	Font(std::string path);
	~Font();
	// The path can end with .txt or .png or no format. All of them will be read.
	bool Init(std::string path);
	Texture texture;
	int charWid[256];

	int imgSize = 1024;
	int charSize = imgSize / 16;// Grid

	bool error=false;
};

#ifdef ENGONE_DEF

Font::Font(std::string path)
{
	Init(path);
}
Font::~Font()
{
	//if (texture != nullptr) {
		//texture.Unbind();
		//texture.~Texture();
	//}
}
bool Font::Init(std::string path)
{
	int dot = path.find_last_of('.');
	if (dot != -1) {
		path = path.substr(0,dot);
	}
	std::vector<std::string> list = engone::ReadFile(path+".txt");
	if (list.size() == 0) {
		std::cout << "empty file or the file is gone\n";
		error = true;
		return true;
	}else if (list.size() == 2) {
		charWid[0] = std::stoi(list.at(0));
		int num = std::stoi(list.at(1));
		for (int i = 1; i < 256; i++) {
			charWid[i] = num;
		}
	} else {
		int i = 0;
		for (std::string s : list) {
			std::vector<std::string> list2 = engone::SplitString(s, ",");
			for (std::string s2 : list2) {
				charWid[i] = std::stoi(s2);
				i++;
			}
		}
	}
	error = texture.Init(path + ".png");
	//if(error)
	//std::cout << error << " err\n";
	return error;
}
#endif