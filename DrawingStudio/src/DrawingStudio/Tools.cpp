#include "Tools.h"

#include <iostream>

namespace tools
{
	static Tool tool=ToolPencil;
	static unsigned char red=0, green=0, blue=0, alpha=255;
	static float size=3;
	void SetTool(Tool name)
	{
		tool = name;
	}
	Tool GetTool()
	{
		return tool;
	}
	void SetColor(float r, float g, float b, float a)
	{
		red = r*255;
		green = g*255;
		blue = b*255;
		alpha = a*255;
		//std::cout << r << " " << g << " " << b << "\n";
	}
	void SetSize(float s)
	{
		size = s;
	}
	float GetSize()
	{
		return size;
	}
	void UseTool(Layer* layer, float x, float y)
	{
		using namespace history;
		switch (tool) {
		case ToolPencil:
			//AddHistory(new DrawHistory(layer));
			Pencil(layer, x, y);
			break;
		case ToolBrush:
			//AddHistory(new DrawHistory(layer));
			Brush(layer,x,y);
			break;
		}
	}
	void DragTool(Layer* layer, float fromX, float fromY, float toX, float toY)
	{
		switch (tool) {
		case ToolPencil:
			DragPencil(layer, fromX, fromY,toX,toY);
			break;
		case ToolBrush:
			DragBrush(layer, fromX, fromY,toX,toY);
			break;
		}
	}
	void Brush(Layer* layer, float x, float y)
	{
		int SIZE = (int)size;
		float off = 0.5;
		if (SIZE/2.f- SIZE/2 != 0) {
			off = 0;
		}
		for (int i = 0; i < SIZE;i++) {
			for (int j = 0; j < SIZE; j++) {
				float tx = i-SIZE/2;
				float ty = j-SIZE/2;
				if ((SIZE / 2.f) * (SIZE / 2.f) >= pow(tx+off, 2) + pow(ty+off, 2)) {
					Pencil(layer, (int)(x+off) + (int)tx, (int)(y+off) + (int)ty);
				}
			}
		}
		SetColor(1, 0, 1, 1);
		Pencil(layer, (int)(x + .5), (int)(y + .5));
		SetColor(0, 0, 0, 1);
	}
	struct vec
	{
		vec(float x,float y) : x(x), y(y) {}
		float x, y;
		
		vec clone()
		{
			return { x,y };
		}
		float length()
		{
			return sqrt(x * x + y * y);
		}
		void normalize()
		{
			float len = length();
			x /= len;
			y /= len;
		}
		vec operator *(float t)
		{
			return { x * t,y * t };
		}
		vec operator /(float t)
		{
			return { x / t,y / t };
		}
		vec operator +(vec& v)
		{
			return { x + v.x,y+v.y };
		}
		vec operator -(vec& v)
		{
			return { x -v.x,y-v.y};
		}
		std::string str()
		{
			return std::to_string(x) + " " + std::to_string(y);
		}
	};
	void DragBrush(Layer* layer, float fromX, float fromY, float toX, float toY)
	{
		//vec from = { floor(fromX),floor(fromY )};
		//vec to = { floor(toX),floor(toY) };

		vec from = { (fromX),(fromY) };
		vec to = { (toX),(toY) };

		float dx = to.x - from.x;
		float dy = to.y - from.y;
		
		if (toX-fromX == 0 && toY-fromY == 0)
		{
			return;
		}

		SetColor(1,0,0,1);
		if(dx==0&&abs(dy)==1||abs(dx)==1&&dy==0||dx==0&&dy==0){
			return;
		}
		if (abs(dx)>abs(dy)) {
			vec dir = { dx / abs(dx),dy / abs(dx) };
			vec normal = {dir.y,-dir.x};
			vec normal2 = {-dir.y,dir.x};
			
			float step = size/2 / normal.length();
			float step2 = size/2 / normal.length();
			vec point = normal * step;
			vec point2 = normal2 * step2;
			std::cout << point.y <<" "<<point2.y<<"\n";
			//std::cout << dx<<" "<<dy << " N: "<<normal.str() << " P: " << point.str() << "\n";
			for (int i = 1; i < abs(dx); i++) {
				for (int j = 0; j < abs(point.y*2); j++) {
					Pencil(layer, from.x+ point.x+ i*dir.x, from.y +dir.x*point.y + round(dir.y * i +j));
				}
				for (int j = 0; j < abs(point2.y*2); j++) {
					Pencil(layer, from.x +point2.x+ i * dir.x, from.y +dir.x * point2.y + round(dir.y * i - j));
				}
			}
		}
		else {
			vec dir = { dx / abs(dy),dy / abs(dy) };
			vec normal = { -dir.y,dir.x };
			vec normal2 = { dir.y,-dir.x };
			float step = (int)size / 2 / normal.length();
			float step2 = (int)size / 2 / normal.length();
			std::cout << step << " " << step2<<"\n";
			vec point = normal * step;
			vec point2 = normal2 * step2;
			//std::cout << dx << " " << dy << " N: " << normal.str() << " P: " << point.str() << "\n";
			for (int i = 1; i < abs(dy); i++) {
				for (int j = 0; j < abs(point.x * 2); j++) {
					Pencil(layer, from.x + round(dir.y*point.x + i * dir.x+j), from.y + point.y + dir.y * i );
				}
				for (int j = 0; j < abs(point2.x * 2); j++) {
					Pencil(layer, from.x + dir.y*point2.x + i * dir.x -j, from.y + point2.y + dir.y * i);
				}
			}
		}
		SetColor(0, 1, 1, 1);
		Brush(layer, from.x, from.y);
		//std::cout << toX << " " << toY<<"\n";
		Brush(layer, to.x, to.y);
	}
	// All drawing functions use this function
	void Pencil(Layer* layer, int x, int y)
	{
		if (x<0 || y<0 || x>layer->width - 1 || y>layer->height - 1)
			return;
		int index = y*layer->width + x;
		
		//history::History* hist = history::GetHistory();

		//((history::DrawHistory*)hist)->AddPixel(x,y, layer->data[index * 4], layer->data[index * 4 + 1], layer->data[index * 4 + 2], layer->data[index * 4 + 3]);

		layer->data[index * 4] = red;
		layer->data[index * 4 + 1] = green;
		layer->data[index * 4 + 2] = blue;
		layer->data[index * 4 + 3] = alpha;
		layer->needRefresh = true;
	}
	void DragPencil(Layer* layer, float fromX, float fromY, float toX, float toY)
	{
		float dx = toX-fromX;
		float dy = toY-fromY;
		/*if (dx == 0) {
			if (abs(dy) < 2) {
				Pencil(layer, toX, toY);
				return;
			}
		}
		else if (dy == 0) {
			if (abs(dx) < 2) {
				Pencil(layer, toX, toY);
				return;
			}
		}*/
		if (abs(dx)>abs(dy)) {
			float k = dy / dx;
			if (dx>0) {
				for (int i = 0; i <= dx; i++) {
					Pencil(layer, fromX+i, fromY+i*k);
				}
			}
			else {
				for (int i = 0; i <= abs(dx); i++) {
					Pencil(layer, fromX-i, fromY - i * k);
				}
			}
		}
		else {
			float k = dx / dy;
			if (dy > 0) {
				for (int i = 0; i <= dy; i++) {
					Pencil(layer, fromX+i*k, fromY+i);
				}
			}
			else {
				for (int i = 0; i <= abs(dy); i++) {
					Pencil(layer, fromX - i * k, fromY-i);
				}
			}
		}
	}
}