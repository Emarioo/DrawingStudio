#pragma once

#include "Objects/Layer.h"

namespace tools
{
	enum Tool
	{
		ToolPencil,
		ToolBrush
	};
	void SetTool(Tool tool);
	Tool GetTool();
	void UseTool(Layer* layer, int x, int y);
	void DragTool(Layer* layer, int fromX, int fromY, int toX, int toY);
	void Brush(Layer* layer, int x,int y);
	void DragBrush(Layer* layer, int fromX,int fromY,int toX,int toY);
	void Pencil(Layer* layer, int x,int y);
	void DragPencil(Layer* layer, int fromX, int fromY, int toX, int toY);
	void Line(Layer* layer, int fromX, int fromY, int toX, int toY);
}