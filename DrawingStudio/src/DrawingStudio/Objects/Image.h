#pragma once
/*
Store an image consisting of multiple layers
*/


#include <vector>
#include "Layer.h"
#include "../AssetManager.h"

class Image
{
public:
	Image()=default;

	float x, y,width,height;

	std::vector<Layer*> layers;
	Layer* selectedLayer;
	bool hidden = false;
	
	void Init(int w,int h)
	{
		width = w;
		height = h;
		layers.push_back(new Layer());
		layers.back()->Init(w, h);
		selectedLayer = layers.back();
	}
	void Render()
	{
		if (hidden)
			return;
		assets::GetShader("basic")->SetVec2("uPos", { x, y});
		for (int i = 0; i < layers.size(); i++) {
			layers[i]->Render();
		}
	}
	
};