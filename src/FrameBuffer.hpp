#pragma once

#include <glad/glad.h>
#include<GLFW/glfw3.h>

class FrameBuffer {
public:
    FrameBuffer(unsigned int width, unsigned int height);
    ~FrameBuffer();

    //Zmienne do postprocesingu kolor�w
    bool invertColors = false;
    bool blackAndWhite = false;
    bool redFilter = false;
    bool greenFilter = false;
    bool blueFilter = false;
    bool border = false;

    void Bind();
    void Unbind();
    void BindTexture();
    unsigned int GetTextureID() const;
    void Inputs(GLFWwindow* window);

private:
    unsigned int FBO, textureID, RBO;
    unsigned int width, height;

    void Init();
};
