#include "Tools.h"

#include <iostream>

namespace tools
{
	static Tool tool=ToolPencil;
	static unsigned char red=0, green=0, blue=0, alpha=255;
	static float size=50;
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
	void UseTool(Layer* layer, float x, float y)
	{
		using namespace history;
		switch (tool) {
		case ToolPencil:
			AddHistory(new DrawHistory(layer));
			Pencil(layer, x, y);
			break;
		case ToolBrush:
			AddHistory(new DrawHistory(layer));
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
		for (int i = 0; i < size;i++) {
			for (int j = 0; j < size; j++) {
				float tx = i-size/2;
				float ty = j-size/2;
				//std::cout << size << " " << tx << " " << ty << "\n";
				if((size/2)*(size/2)>=pow(tx+.5,2)+pow(ty+.5,2))
					Pencil(layer, round(x+tx), round(y+ty));
			}
		}
	}
	void DragBrush(Layer* layer, float fromX, float fromY, float toX, float toY)
	{
		float dx = toX - fromX;
		float dy = toY - fromY;
		//std::cout << dx << " "<<dy<<"\n";
		if (abs(dx)>abs(dy)) {
			float k = dy / dx;
			
			float sided;
			if (dx > 0) {
				sided = -size;
			}
			else {
				sided = 0;
			}
			for (int i = 0; i < size; i++) {
				float d = sqrt(size * size - pow(i +sided, 2));

				for (int j = -d; j < d; j++) {
					int tx = fromX +sided + i;
					int ty = fromY + j;
					if (tx<0 || ty<0 || tx>layer->width - 1 || ty>layer->height - 1)
						continue;
					Pencil(layer, tx, ty);
				}
			}
			if (dx > 0) {
				for (int i = 0; i <= dx; i++) {
					for (int j = -size; j < size; j++) {
						int tx = fromX + i;
						int ty = fromY + j+i*k;
						if (tx<0 || ty<0 || tx>layer->width - 1 || ty>layer->height - 1)
							continue;
						Pencil(layer, tx, ty);
					}
				}
			}
			else {
				for (int i = 0; i <= abs(dx); i++) {
					for (int j = -size; j < size; j++) {
						int tx = fromX - i;
						int ty = fromY + j - i * k;
						if (tx<0 || ty<0 || tx>layer->width - 1 || ty>layer->height - 1)
							continue;
						Pencil(layer, tx, ty);
					}
				}
			}
			if (dx > 0) {
				sided = 0;
			}
			else {
				sided = -size;
			}
			for (int i = 0; i < size; i++) {
				float d = sqrt(size * size - pow(i+sided, 2));

				for (int j = -d; j < d; j++) {
					int tx = toX +sided+ i;
					int ty = toY + j;
					if (tx<0 || ty<0 || tx>layer->width - 1 || ty>layer->height - 1)
						continue;
					Pencil(layer, tx, ty);
				}
			}
		}
		else {
			float k = dx / dy;
			
			float sided;
			if (dy > 0) {
				sided = -size;
			}
			else {
				sided = 0;
			}
			for (int i = 0; i < size; i++) {
				float d = sqrt(size * size - pow(i+sided, 2));

				for (int j = -d; j < d; j++) {
					int tx = fromX + j;
					int ty = fromY +i+sided;
					if (tx<0 || ty<0 || tx>layer->width - 1 || ty>layer->height - 1)
						continue;
					Pencil(layer, tx, ty);
				}
			}
			
			if (dy > 0) {
				for (int i = 0; i <= dy; i++) {
					for (int j = -size; j < size; j++) {
						int tx = fromX + j+i*k;
						int ty = fromY + i;
						if (tx<0 || ty<0 || tx>layer->width - 1 || ty>layer->height - 1)
							continue;
						Pencil(layer, tx, ty);
					}
				}
			}
			else {
				for (int i = 0; i <= abs(dy); i++) {
					for (int j = -size; j < size; j++) {
						int tx = fromX + j -i*k;
						int ty = fromY - i;
						if (tx<0 || ty<0 || tx>layer->width - 1 || ty>layer->height - 1)
							continue;
						Pencil(layer, tx, ty);
					}
				}
			}
			if (dy>0) {
				sided = 0;
			}
			else {
				sided = -size;
			}
			for (int i = 0; i < size; i++) {
				float d = sqrt(size * size - pow(i+sided, 2));

				for (int j = -d; j < d; j++) {
					int tx = toX + j;
					int ty = toY +sided+ i;
					if (tx<0 || ty<0 || tx>layer->width - 1 || ty>layer->height - 1)
						continue;
					Pencil(layer, tx, ty);
				}
			}
		}
	}
	// All drawing functions use this function
	void Pencil(Layer* layer, int x, int y)
	{
		if (x<0 || y<0 || x>layer->width - 1 || y>layer->height - 1)
			return;
		int index = y*layer->width + x;
		
		history::History* hist = history::GetHistory();
		//std::cout << hist << "\n";

		((history::DrawHistory*)hist)->AddPixel(x,y, layer->data[index * 4], layer->data[index * 4 + 1], layer->data[index * 4 + 2], layer->data[index * 4 + 3]);

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
		if (dx == 0) {
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
		}
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