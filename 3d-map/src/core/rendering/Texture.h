#pragma once

#include <string>

class Texture {
public:
	Texture(const std::string& filePath);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;
	inline int GetWidth() const { return m_Width; };
	inline int GetHeight() const { return m_Height; };
	inline unsigned int GetId() const { return m_RendererId; }
private:
	unsigned int m_RendererId;
	std::string m_FilePath;
	unsigned char* m_Buffer;
	int m_Width, m_Height, m_BPP;
};
