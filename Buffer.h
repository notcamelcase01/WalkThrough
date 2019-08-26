#pragma once


#include<GL/glew.h>
#include<vector>

class VertexBuffer
{
private:
	GLuint m_Renderer_ID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind();
	void UnBind();

};

class IndexBuffer
{
private:
	GLuint m_Renderer_ID;
	unsigned int m_Count;
public:
	IndexBuffer(const void* data, unsigned int count);
	~IndexBuffer();

	void Bind();
	void UnBind();

	inline unsigned int GetCount() { return m_Count; }

};

struct VertexBufferElement {

	GLint _id;
	unsigned int type;
	unsigned int count;
	bool normalize;

};

class BufferLayout {
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_typeSize;
	unsigned int m_Stride;
public:

	BufferLayout(unsigned int stride) :m_Stride(stride) { }

	template<typename T>
	void Push(GLint _id, unsigned int count, bool normalise)
	{
		static_assert(false);
	}
	template<>
	void Push<float>(GLint _id, unsigned int count, bool normalise)
	{
		m_typeSize = sizeof(float);
		m_Elements.push_back({ _id, GL_FLOAT, count, normalise });

	}

	inline std::vector<VertexBufferElement> GetElements() { return m_Elements; }
	inline unsigned int GetStride() { return m_Stride; }
	inline unsigned int GetTypeSize() { return m_typeSize; }

};



class VertixAttrib {
private:

public:
	VertixAttrib();
	~VertixAttrib();

	void AddBuffer(VertexBuffer& vb, BufferLayout& layout);
};


