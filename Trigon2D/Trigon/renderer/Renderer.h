#ifndef RENDERER_H
#define RENDERER_H
#include "../math/Vector2.h"

typedef unsigned int	uint;
typedef unsigned long	uLong;

class GLFWwindow;


enum class WindowMode
{
	Windowed,
	Fullscreen,
	WindowedFullscreen

};


struct WindowSettings
{
	int			m_Width;
	int			m_Height;
	const char* m_pName;
	int			m_Samples;
	bool		m_Decorated;
	WindowMode	m_Mode;
};



class Renderer
{


private:
	GLFWwindow*			m_pWindow			=	nullptr;
	bool				m_GlfwInitialized	=	false;
	bool				m_GladLoaded		=	false;
	Vector2				m_PixelScale		=	Vector2(0,0);
	static Renderer*	m_pInstance;
	uint				m_VertexArrayId		=   -1;

public:

////////////////////////////////////////////////////////////

private:
	void				DestroyWindow();

public:
	void				CreateWindow(const WindowSettings& settings);
	WindowSettings		DefaultWindowSetting();
	void				LoadGlad();
	void				SwapBuffers();
	Vector2				GetWindowSize();
	bool				ShouldQuit();
	Renderer()			{ m_pInstance = this; }
	static Renderer*	GetInstance(){return m_pInstance;}
	void				SetPixelScale(const Vector2& scale) { m_PixelScale = scale; }
	Vector2				GetPixelScale(){ return  m_PixelScale;}
	void				DrawQuad(const uint& vertexBuffer);
	uint				LoadShaders(const char* vertexFile, const char* pixelFile);
	void				ReleaseGlad();


	~Renderer();
};


#endif // !RENDERER_H