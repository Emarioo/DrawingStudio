#pragma once
/*
Store the pixel data of this layer. A container the modification of it is done elsewhere.
*/

#include "Engone/Rendering/Texture.h"
#include "Engone/Rendering/Buffer.h"

class Layer
{
public:
	Layer()=default;
	
	engone::TriangleBuffer buffer;
	engone::Texture* texture;
	unsigned char* data;
	int width, height;
	bool needRefresh = false;

	bool hidden = false;

	void Init(int width, int height)
	{
		this->width = width;
		this->height = height;
		data = new unsigned char[width * height * 4];
		
		memset(data, 255, width * height * 4);

		float vert[]{
		-width,-height,0,0,
		width,-height,1,0,
		width,height,1,1,
		-width,height,0,1,
		};
		unsigned int index[]{
			0,1,2,
			2,3,0
		};

		buffer.Init(false,vert,16,index,6);
		buffer.SetAttrib(0, 4, 4, 0);
		texture = new engone::Texture();
		texture->Init(width, height, data);
	}
	void Set(int x,int y,unsigned char red,unsigned char green,unsigned char blue, unsigned alpha)
	{
		int index = y*width + x;
		data[index * 4] = red;
		data[index * 4 + 1] = green;
		data[index * 4 + 2] = blue;
		data[index * 4 + 3] = alpha;
		needRefresh = true;
	}
	void Refresh()
	{
		if(data!=nullptr)
			texture->SubData(0,0,width,height,data);
	}
	void Render()
	{
		if (hidden)
			return;
		if (data != nullptr) {
			if (needRefresh) {
				Refresh();
				needRefresh = false;
			}
			texture->Bind();
			buffer.Draw();
		}
	}
};