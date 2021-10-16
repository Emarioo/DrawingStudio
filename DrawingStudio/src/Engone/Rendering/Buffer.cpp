#include "Buffer.h"

#include <GL/glew.h>

#include <iostream>

/*
Only use this if OpenGL has been initialized.
*/
Buffer::Buffer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount)
	: dynamic(dynamic), vertexCount(vertexCount), indexCount(indexCount) {
	GenBuffers(vertices, indices);
}
Buffer::~Buffer() {
	glDeleteBuffers(1, &vertexArray);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &indexBuffer);
}
void Buffer::Init(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount)
{ // fcount in number of floats
	this->dynamic = dynamic;
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;

	GenBuffers(vertices, indices);
}
void Buffer::GenBuffers(void* vertices, void* indices) {
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	if (dynamic) glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_DYNAMIC_DRAW);
	else glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	if (dynamic) glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_DYNAMIC_DRAW);
	else glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

}
void Buffer::SetAttrib(unsigned int loc, unsigned int count, unsigned int stride, unsigned int offset) {
	glEnableVertexAttribArray(loc);
	//glEnableVertexArrayAttrib(va, loc); old?
	glVertexAttribPointer(loc, count, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const void*)(offset * sizeof(float)));
}
void Buffer::SetAttribI(unsigned int loc, unsigned int count, unsigned int stride, unsigned int offset) {
	glEnableVertexAttribArray(loc);
	//glEnableVertexArrayAttrib(va, loc); old?
	glVertexAttribIPointer(loc, count, GL_UNSIGNED_SHORT, stride * sizeof(int), (const void*)(offset * sizeof(int)));
}
void Buffer::ModifyVertices(unsigned int offset, unsigned int icount, void* data) {
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, offset, icount * sizeof(float), data);
}
void Buffer::ModifyIndices(unsigned int offset, unsigned int count, void* data) {// count in number of floats
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, count * sizeof(unsigned int), data);
}
TriangleBuffer::TriangleBuffer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount)
: Buffer(dynamic,vertices,vertexCount,indices,indexCount) {}
void TriangleBuffer::Draw() {
	glBindVertexArray(vertexArray);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}
LineBuffer::LineBuffer(bool dynamic, void* vertices, unsigned int vertexCount, void* indices, unsigned int indexCount)
	: Buffer(dynamic, vertices, vertexCount, indices, indexCount) {}
void LineBuffer::Draw() {
	glBindVertexArray(vertexArray);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, nullptr);
}