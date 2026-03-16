#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(unsigned int w, unsigned int h) : width(w), height(h) {
    Init();
}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &textureID);
    glDeleteRenderbuffers(1, &RBO);
}

void FrameBuffer::Init() {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Tekstura framebuffer
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

    // Renderbuffer (g³êbia + stencil)
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer niekompletny!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void FrameBuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::BindTexture() {
    glBindTexture(GL_TEXTURE_2D, textureID);
}

unsigned int FrameBuffer::GetTextureID() const {
    return textureID;
}

void FrameBuffer::Inputs(GLFWwindow* window)
{
	// Obs³uga przycisku R do zmiany koloru na negatyw.
	static bool rKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !rKeyPressed)
	{
		invertColors = !invertColors;
		rKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
	{
		rKeyPressed = false;
	}

	// Obs³uga przycisku T do zmiany koloru na czarno-bia³y.
	static bool tKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !tKeyPressed)
	{
		blackAndWhite = !blackAndWhite;
		tKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
	{
		tKeyPressed = false;
	}

	// Obs³uga przycisku Y do zmiany koloru na nasycony czerwieni¹.
	static bool yKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && !yKeyPressed)
	{
		redFilter = !redFilter;
		yKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE)
	{
		yKeyPressed = false;
	}

	// Obs³uga przycisku U do zmiany koloru na nasycony zieleni¹.
	static bool uKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && !uKeyPressed)
	{
		greenFilter = !greenFilter;
		uKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_RELEASE)
	{
		uKeyPressed = false;
	}

	// Obs³uga przycisku I do zmiany koloru na nasycony niebieskim.
	static bool iKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS && !iKeyPressed)
	{
		blueFilter = !blueFilter;
		iKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_RELEASE)
	{
		iKeyPressed = false;
	}

	// Obs³uga przycisku O do wykrywania krawêdzi.
	static bool oKeyPressed = false;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !oKeyPressed)
	{
		border = !border;
		oKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE)
	{
		oKeyPressed = false;
	}
}