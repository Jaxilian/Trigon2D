#include "Renderer.h"
#include <glad/gl.h>
#include "GLFW/glfw3.h"
#include <stdio.h>
#include <vector>
#include <sstream>
#include <fstream>

static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	Renderer::GetInstance()->SetPixelScale(Vector2(1/width, 1/height));
	glViewport(0, 0, width, height);
}
Renderer* Renderer::m_pInstance = nullptr;


void
Renderer::CreateWindow(const WindowSettings& settings)
{
	if (!m_GlfwInitialized) glfwInit();
	m_GlfwInitialized = true;

	glfwWindowHint(GLFW_DECORATED, settings.m_Decorated);
	glfwWindowHint(GLFW_SAMPLES, settings.m_Samples);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (settings.m_Mode == WindowMode::WindowedFullscreen) m_pWindow = glfwCreateWindow(settings.m_Width, settings.m_Height, settings.m_pName, glfwGetPrimaryMonitor(), NULL);

	if (settings.m_Mode == WindowMode::Fullscreen)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

		m_pWindow = glfwCreateWindow(settings.m_Width, settings.m_Height, settings.m_pName, glfwGetPrimaryMonitor(), NULL);
	}

	if (settings.m_Mode == WindowMode::Windowed) m_pWindow = glfwCreateWindow(settings.m_Width, settings.m_Height, settings.m_pName, NULL, NULL);

	if (m_pWindow == NULL)
	{
		printf("Failed to create window\n");
		return;
	}

	glfwMakeContextCurrent(m_pWindow);
	glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetFramebufferSizeCallback(m_pWindow, FrameBufferSizeCallback);
}

void
Renderer::SwapBuffers() 
{
	glfwSwapBuffers(m_pWindow);
	glfwPollEvents();
}

bool 
Renderer::ShouldQuit()
{
	return glfwWindowShouldClose(m_pWindow);
}

void
Renderer::DrawQuad(const uint& vertexBuffer)
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
		0,                 
		3,                 
		GL_FLOAT,           
		GL_FALSE,         
		0,                 
		(void*)0            
	);
	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
}

void
Renderer::DestroyWindow() 
{
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

Renderer::~Renderer() 
{
	 DestroyWindow(); 
	 ReleaseGlad(); 
}

void		
Renderer::LoadGlad() 
{
	if (!m_GladLoaded) gladLoadGL(glfwGetProcAddress);
	m_GladLoaded = true;

	int width;
	int height;

	glfwGetWindowSize(m_pWindow, &width, &height);

	glViewport(0,0,width,height);

	glClearColor(0.2f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	glGenVertexArrays(1, &m_VertexArrayId);
	glBindVertexArray(m_VertexArrayId);
}

Vector2		
Renderer::GetWindowSize()
{
	int x, y;
	glfwGetWindowSize(m_pWindow,&x,&y);
	return Vector2(x,y);
}


uint			
Renderer::LoadShaders(const char* vertexFile, const char* pixelFile)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertexFile, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertexFile);
		getchar();
		return 0;
	}

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(pixelFile, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	printf("Compiling shader : %s\n", vertexFile);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	printf("Compiling shader : %s\n", pixelFile);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}


	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void
Renderer::ReleaseGlad()
{
	if (!m_GladLoaded) return;
	glDeleteVertexArrays(1, &m_VertexArrayId);
}

WindowSettings
Renderer::DefaultWindowSetting()
{
	WindowSettings settings;
	settings.m_Decorated	= true;
	settings.m_Height		= 600;
	settings.m_Width		= 800;
	settings.m_pName		= "Empty";
	settings.m_Samples		=16;
	settings.m_Mode			= WindowMode::Windowed;
	return settings;
}