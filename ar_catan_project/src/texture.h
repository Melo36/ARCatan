#pragma once

#include "Renderer.h"
#include <opencv2/imgproc.hpp>

enum TextureType{diffuse,specular,None};

class Texture
{
private:
	unsigned int m_rendererID;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_width, m_height, m_BPP;
public:
	Texture(const std::string& path, TextureType type = TextureType::None);
	Texture(cv::Mat img);
	~Texture();

	TextureType m_type;

	void bind(unsigned int slot = 0) const;
	void unBind() const;

	void setTexture(cv::Mat img);

	std::string getPath();

	inline int getWidth() const { return m_width; };
	inline int getHeight() const { return m_height; };
	inline unsigned int getRendererID() const { return m_rendererID; };
};