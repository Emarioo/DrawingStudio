#pragma once

#include "Objects/Layer.h"

#include "History.h"

namespace tools
{
	enum Tool
	{
		ToolPencil,
		ToolBrush
	};
	void SetTool(Tool tool);
	Tool GetTool();
	void SetColor(float r, float g, float b, float a);
	void SetSize(float size);
	float GetSize();
	void UseTool(Layer* layer, float x, float y);
	void DragTool(Layer* layer, float fromX, float fromY, float toX, float toY);
	void Brush(Layer* layer, float x, float y);
	void DragBrush(Layer* layer, float fromX, float fromY, float toX, float toY);
	void Pencil(Layer* layer, int x, int y);
	void DragPencil(Layer* layer, float fromX, float fromY, float toX, float toY);
	void Line(Layer* layer, float fromX, float fromY, float toX, float toY);
}