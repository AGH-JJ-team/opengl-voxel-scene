#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shaderClass.hpp"
#include "Camera.hpp"

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces);
    ~Skybox();

    void Draw(Camera& camera, Shader& skyboxShader, unsigned int width, unsigned int height);

private:
    unsigned int VAO, VBO, EBO, cubemapTexture;
    void loadCubemap(const std::vector<std::string>& faces);
};
