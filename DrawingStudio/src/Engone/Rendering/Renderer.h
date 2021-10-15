#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

#include "Font.h"
#include "Buffer.h"

namespace renderer
{

	// GLFW stuff
	GLFWwindow* Init();
	GLFWwindow* GetWindow();
	int Width();
	int Height();

	// Render stuff
	// Set a fixed pixel height. Center text to point, fit inside width and height, text marker
	void DrawString(Font* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar=-1);
	void DrawRect();

#ifdef ENGONE_DEF

	static GLFWwindow* window;
	static TriangleBuffer textBuffer;
	static TriangleBuffer rectBuffer;
	static const int TEXT_BATCH = 40;
	static float verts[4 * 4 * TEXT_BATCH];
	GLFWwindow* Init()
	{
		if (!glfwInit()) {
			return nullptr;
		}
		window = glfwCreateWindow(1200, 800, "Drawing Studio", NULL, NULL);
		if (!window) {
			glfwTerminate();
			return nullptr;
		}

		glfwMakeContextCurrent(window);

		if (glewInit() != GLEW_OK) {
			return nullptr;
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);

		unsigned int index[TEXT_BATCH * 6];
		for (int i = 0; i < TEXT_BATCH; i++) {
			index[0 + 6 * i] = 2 + 4 * i;
			index[1 + 6 * i] = 1 + 4 * i;
			index[2 + 6 * i] = 0 + 4 * i;
			index[3 + 6 * i] = 0 + 4 * i;
			index[4 + 6 * i] = 3 + 4 * i;
			index[5 + 6 * i] = 2 + 4 * i;
		}
		
		textBuffer.Init(true, nullptr, 4 * 4 * TEXT_BATCH, index, 6 * TEXT_BATCH);
		textBuffer.SetAttrib(0, 4, 4, 0);
		float vert[]{
		0,0,0,1,
		1,0,1,1,
		1,1,1,0,
		0,1,0,0,
		};
		unsigned int indexr[]{
			0,1,2,
			2,3,0
		};
		rectBuffer.Init(false, vert, 4 * 4, indexr, 6);
		rectBuffer.SetAttrib(0, 4, 4, 0);

		return window;
	}
	GLFWwindow* GetWindow()
	{
		return window;
	}
	int Width()
	{
		int w;
		glfwGetWindowSize(window, &w, nullptr);
		return w;
	}
	int Height()
	{
		int h;
		glfwGetWindowSize(window, nullptr, &h);
		return h;
	}
	static void Insert4(float* ar, int ind, float f0, float f1, float f2, float f3)
	{
		ar[ind] = f0;
		ar[ind + 1] = f1;
		ar[ind + 2] = f2;
		ar[ind + 3] = f3;
	}
	void DrawRect()
	{
		rectBuffer.Draw();
	}
	void DrawString(Font* font, const std::string& text, bool center, float wantedHeight, float maxWidth, float maxHeight, int atChar)
	{
		if (text.length() == 0 && atChar == -1)
			return;
		if (font == nullptr) {
			std::cout << "DrawString: Font is null\n";
			return;
		}
		else if(font->texture.error){
			return;
		}
		else {
			font->texture.Bind();
		}
		std::vector<std::string> lines;
		std::vector<float> lineWidths;

		float spacing = 0;
		int lineIndex = 0;
		std::string word;
		float wordWidth = 0;
		lines.push_back("");
		lineWidths.push_back(0);
		for (int i = 0; i < text.size(); i++) {
			unsigned char chr = text[i];

			float added = wantedHeight * (font->charWid[chr] / (float)font->charSize);
			//if (wordWidth != 0 && lineWidths[lineIndex] != 0)
			added += spacing;

			if (wordWidth==0&&lineWidths[lineIndex] == 0 && chr == ' ') {// skip space in the beginning of line
				continue;
			}

			if (chr == '\n'&& lineWidths[lineIndex]!=0) { // new line
				if (lineWidths[lineIndex] + wordWidth < maxWidth) {
					lines[lineIndex] += word;
					lineWidths[lineIndex] += wordWidth;
					lineWidths.push_back(0);
					lines.push_back("");
				}
				else {
					lineWidths.push_back(wordWidth);
					lines.push_back(word);
				}
				word = "";
				wordWidth = 0;
				lineIndex++;
				continue;
			}
			if (lineWidths[lineIndex]+ wordWidth + added >maxWidth) {
				i--;
				if (word[0] == ' ') {
					word = word.substr(1);
					wordWidth -= spacing+wantedHeight * (font->charWid[chr] / (float)font->charSize);
				}
				lineWidths.push_back(wordWidth);
				lines.push_back(word);
				word = "";
				wordWidth = 0;
				lineIndex++;
				continue;
			}
			if (chr==' ') {
				lines[lineIndex] += word;
				lineWidths[lineIndex] += wordWidth;
				word = "";
				wordWidth = 0;
			}
			word += chr;
			wordWidth += added;
			if (i == text.size() - 1) {
				lines[lineIndex] += word;
				lineWidths[lineIndex] += wordWidth;
			}
		}
		//std::cout << rowWidths.size() << "\n";
		//std::cout << lineWidths.size()<<" "<<wantedHeight << " "<< (maxHeight/ lineWidths.size())<< "\n";
		
		if (lineWidths.size()*wantedHeight>maxHeight) {
			for (int i = 0; i < lineWidths.size(); i++) {
				//lineWidths[i] -= spacing * (lines[i].length() - 1);
				lineWidths[i] *= maxHeight / lineWidths.size()/wantedHeight;
				//lineWidths[i] += spacing*(lines[i].length()-1);
				//std::cout << rowWidths[i] << "\n";
			}
			wantedHeight = maxHeight / lineWidths.size();
		}

		//float accWidth = 0;
		/*
		if (atChar != -1) { // do marker
			float wid = 0;
			float hei = 0;
			for (int i = 0; i < atChar; i++) {
				int cha = text.at(i);
				if ((char)cha == '\n') {
					wid = 0;
					hei++;
					continue;
				}
				if (cha < 0) {
					cha += 256;
				}
				wid += font->charWid[cha];
			}
			float markerX = x + wid * ((wantedHeight / (16 / 9.f)) / font->charSize);
			float markerY = y + hei * wantedHeight;
			float wuv = font->charWid[0] / (float)font->imgSize;
			float wxy = wantedHeight * (font->charWid[0] / (float)font->charSize);
			float u = (0 % 16);
			float v = 15 - (0 / 16);
			
			Insert4(verts, 16 * indChar, markerX, markerY, (u) / 16, (v) / 16);
			Insert4(verts, 4 + 16 * indChar, markerX, markerY + wantedHeight, (u) / 16, (v + 1) / 16);
			Insert4(verts, 8 + 16 * indChar, markerX + wxy, markerY + wantedHeight, (u) / 16 + wuv, (v + 1) / 16);
			Insert4(verts, 12 + 16 * indChar, markerX + wxy, markerY, (u) / 16 + wuv, (v) / 16);

			indChar++;
		}
		*/
		float x = 0;
		float y = 0;
		if (center) {
			y = (maxHeight-wantedHeight*lines.size())/2;
		}
		int dataIndex = 0;
		for (int i = 0; i < lines.size();i++) {
			//std::cout << "[" << lines[i] << "] " << "\n";
			float x = 0;
			if (center)
				x = (maxWidth - lineWidths[i]) / 2;
			for (int j = 0; j < lines[i].length();j++) {
				char chr = lines[i][j];

				float wStride = wantedHeight * (font->charWid[chr] / (float)font->charSize);

				float wuv = font->charWid[chr] / (float)font->imgSize;
				float u = (chr % 16);
				float v = 15 - (chr / 16);

				Insert4(verts, 16 * dataIndex, x, y, (u) / 16, (v + 1) / 16);
				Insert4(verts, 4 + 16 * dataIndex, x, y + wantedHeight, (u) / 16, (v) / 16);
				Insert4(verts, 8 + 16 * dataIndex, x + wStride, y + wantedHeight, (u) / 16 + wuv, (v) / 16);
				Insert4(verts, 12 + 16 * dataIndex, x + wStride, y, (u) / 16 + wuv, (v + 1) / 16);

				x += wStride + spacing;

				if (dataIndex == TEXT_BATCH) {
					textBuffer.ModifyVertices(0, 4 * 4 * TEXT_BATCH, verts);
					textBuffer.Draw();
					dataIndex = 0;
				}
				else {
					dataIndex++;
				}
			}
			y += wantedHeight;
		}

		int charIndex = (((atChar != -1) + dataIndex) % TEXT_BATCH);
		//std::cout << (16*charIndex)<<" "<<(16*(TEXT_BATCH-charIndex));
		memset(&verts[16*charIndex], 0, 16*(TEXT_BATCH-charIndex));

		textBuffer.ModifyVertices(0, 4 * 4 * TEXT_BATCH, verts);
		textBuffer.Draw();
	}

#endif
}