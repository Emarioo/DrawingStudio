#pragma once

#include <vector>
#include "Objects/Layer.h"

#include <iostream>

namespace history
{
	enum class HistoryType
	{
		Draw,
		
	};
	class History
	{
	public:
		History(HistoryType type) : type(type){}

		HistoryType type;
	};
	struct Pixel
	{
		Pixel() = default;
		Pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) :x(x), y(y), r(r), g(g), b(b), a(a) {}
		void Set(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
			this->x = x;
			this->y = y;
			this->r = r;
			this->g = g;
			this->b = b;
			this->a = a;
		}
		int x, y;
		unsigned char r, g, b, a;
	};
	class DrawHistory : public History
	{
	public:
		DrawHistory(Layer* layer) : History(HistoryType::Draw), layer(layer) {}
		~DrawHistory()
		{
			for (int i = 0; i < data.size(); i++) {
				delete[] data[i];
			}
		}

		Layer* layer;
		std::vector<Pixel*> data;
		unsigned int written = 0;
		unsigned int batchSize = 20;

		void AddPixel(int x,int y,unsigned char r, unsigned char g,unsigned char b,unsigned char a)
		{
			return;
			if (data.size() * batchSize < written + 1) {
				data.push_back(new Pixel[batchSize]);
			}
			int index = written - (data.size() - 1) * batchSize;
			//std::cout << "Set " << index << " " << x << " " << y << "\n";
			data.back()[index].Set(x,y,r,g,b,a);

			written++;
		}
		void Revert()
		{
			return;
			//std::cout << written <<" D:"<<data.size()<< "\n";
			while(written>0){
				written--;
				int batchIndex = (written / batchSize);
				int index = written - (batchIndex) * batchSize;
				//std::cout << batchIndex <<" " <<index << "\n";
				Pixel& pixel = data[batchIndex][index];
				//std::cout << pixel.x<<" "<<pixel.y<<"\n";
				layer->Set(pixel.x, pixel.y, pixel.r, pixel.g, pixel.b, pixel.a);
			}
		}

	};

	void AddHistory(History* history);
	void Init();
	History* GetHistory();
}