#pragma once

#include <string>

class Texture
{
private:
	unsigned int id;
	unsigned char* buffer = nullptr;
	int width = 0, height, BPP = 0;
public:
	Texture();
	Texture(const std::string& path);
	~Texture();
	bool Init(const std::string& path);
	void Init(int w,int h,void* data);

	std::string filepath;

	void SubData(int x, int y, int w, int h, void* data);

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth();
	inline int GetHeight();

	bool error;
};