#pragma once

#include <GL/glew.h>
#include <vector>

struct LayoutElement {
	unsigned int type;
	unsigned int count;
	bool normalized;

	static unsigned int GetSize(unsigned int type) {
		switch (type) {
		case GL_FLOAT: return 4;
		case GL_UNSIGNED_INT: return 4;
		}
		// assert
		return 0;
	}

};

class VertexBufferLayout {
public:
	VertexBufferLayout()
		: m_Stride(0) {}


	void PushFloat(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += LayoutElement::GetSize(GL_FLOAT) * count;
	}

	inline unsigned int GetStride() const { return m_Stride; };
	inline const std::vector<LayoutElement>& GetElements() const { return m_Elements;  }

private:
	std::vector<LayoutElement> m_Elements;
	unsigned int m_Stride;
};
