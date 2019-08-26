#include "Buffer.h"




VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	glGenBuffers(1, &m_Renderer_ID);
	glBindBuffer(GL_ARRAY_BUFFER, m_Renderer_ID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_Renderer_ID);
}

void VertexBuffer::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Renderer_ID);

}

void VertexBuffer::UnBind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

IndexBuffer::IndexBuffer(const void* data, unsigned int count)
{
	glGenBuffers(1, &m_Renderer_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Renderer_ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}


IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_Renderer_ID);
}

void IndexBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Renderer_ID);
}

void IndexBuffer::UnBind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

VertixAttrib::VertixAttrib()
{
}

VertixAttrib::~VertixAttrib()
{
}

void VertixAttrib::AddBuffer(VertexBuffer& vb, BufferLayout& layout)
{
	vb.Bind();
	auto elements = layout.GetElements();
	auto t_size = layout.GetTypeSize();
	int offset = 0;
	for (auto element : elements)
	{
		glEnableVertexAttribArray(element._id);
		glVertexAttribPointer(
			element._id,
			element.count,
			element.type,
			element.normalize ? GL_NORMALIZE : GL_FALSE,
			layout.GetStride() * t_size,
			(void*)offset);
		offset += element.count * t_size;
	}
}
