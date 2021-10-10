#include "Tools.h"

#include <iostream>

namespace tools
{
	static Tool tool=ToolPencil;
	void SetTool(Tool name)
	{
		tool = name;
	}
	Tool GetTool()
	{
		return tool;
	}
	void UseTool(Layer* layer, int x, int y)
	{
		switch (tool) {
		case ToolPencil:
			Pencil(layer, x, y);
			break;
		case ToolBrush:
			Brush(layer,x,y);
			break;
		}
	}
	void DragTool(Layer* layer, int fromX,int fromY, int toX,int toY)
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
	void Brush(Layer* layer, int x, int y)
	{
		int size = 20;
		for (int i = 0; i < size*2;i++) {
			float dy = sqrt(size * size - pow(i-size, 2));
			
			for (int j = -dy; j < dy; j++) {
				int tx = x - size + i;
				int ty = y + j;
				if (tx<0 || ty<0 || tx>layer->width - 1 || ty>layer->height-1)
					continue;
				Pencil(layer, tx, ty);
			}
		}
	}
	void DragBrush(Layer* layer, int fromX, int fromY,int toX, int toY)
	{
		int size = 10;

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
					for (int j = -size-1; j < size+1; j++) {
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
					for (int j = -size-1; j < size+1; j++) {
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
					for (int j = -size-1; j < size+1; j++) {
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
					for (int j = -size-1; j < size+1; j++) {
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
	void Pencil(Layer* layer, int x,int y)
	{
		int index = (y)*layer->width + x;

		layer->data[index * 4] = 0;
		layer->data[index * 4 + 1] = 0;
		layer->data[index * 4 + 2] = 0;
		layer->needRefresh = true;
	}
	void DragPencil(Layer* layer, int fromX, int fromY, int toX, int toY)
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