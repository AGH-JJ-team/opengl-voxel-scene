#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <iostream>

#include<glad/glad.h>
//#include<GLFW/glfw3.h>
//#include<stb/stb_image.h>
//#include<glm/glm.hpp>
//#include<glm/gtc/matrix_transform.hpp>
//#include<glm/gtc/type_ptr.hpp>


bool Model::LoadFromFile(const std::string& filepath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return false;
    }

    ProcessNode(scene->mRootNode, scene);
    return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

Model::Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    Mesh result;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // Positions
        result.vertices.push_back(mesh->mVertices[i].x);
        result.vertices.push_back(mesh->mVertices[i].y);
        result.vertices.push_back(mesh->mVertices[i].z);

        // Normals
        if (mesh->HasNormals()) {
            result.normals.push_back(mesh->mNormals[i].x);
            result.normals.push_back(mesh->mNormals[i].y);
            result.normals.push_back(mesh->mNormals[i].z);
        }

        // Texture Coordinates - handle cases where they might not exist
        if (mesh->mTextureCoords[0]) {
            // Use only the first set of texture coordinates
            result.texCoords.push_back(mesh->mTextureCoords[0][i].x);
            result.texCoords.push_back(mesh->mTextureCoords[0][i].y);
        }
        else {
            // Default texture coordinates if none exist
            result.texCoords.push_back(0.0f);
            result.texCoords.push_back(0.0f);
        }
    }

    // Process indices - handle both triangles and quads
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        // Convert quads to triangles (1 quad = 2 triangles)
        if (face.mNumIndices == 4) {
            // First triangle
            result.indices.push_back(face.mIndices[0]);
            result.indices.push_back(face.mIndices[1]);
            result.indices.push_back(face.mIndices[2]);

            // Second triangle
            result.indices.push_back(face.mIndices[0]);
            result.indices.push_back(face.mIndices[2]);
            result.indices.push_back(face.mIndices[3]);
        }
        else if (face.mNumIndices == 3) {
            // Regular triangle
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                result.indices.push_back(face.mIndices[j]);
            }
        }
    }

    return result;
}

void Model::SetupMesh() {
    for (auto& mesh : meshes) {
        glGenVertexArrays(1, &mesh.VAO);
        glGenBuffers(1, &mesh.VBO);
        glGenBuffers(1, &mesh.EBO);

        glBindVertexArray(mesh.VAO);

        // Create interleaved vertex data
        std::vector<float> interleavedData;
        for (size_t i = 0; i < mesh.vertices.size() / 3; i++) {
            // Position
            interleavedData.push_back(mesh.vertices[i * 3]);
            interleavedData.push_back(mesh.vertices[i * 3 + 1]);
            interleavedData.push_back(mesh.vertices[i * 3 + 2]);

            // Normal
            if (!mesh.normals.empty()) {
                interleavedData.push_back(mesh.normals[i * 3]);
                interleavedData.push_back(mesh.normals[i * 3 + 1]);
                interleavedData.push_back(mesh.normals[i * 3 + 2]);
            }

            // Texture Coordinates
            if (!mesh.texCoords.empty()) {
                interleavedData.push_back(mesh.texCoords[i * 2]);
                interleavedData.push_back(mesh.texCoords[i * 2 + 1]);
            }
        }

        // Calculate stride
        GLsizei stride = 3 * sizeof(float); // position
        if (!mesh.normals.empty()) stride += 3 * sizeof(float);
        if (!mesh.texCoords.empty()) stride += 2 * sizeof(float);

        glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
        glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), mesh.indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);

        // Normal attribute (if exists)
        if (!mesh.normals.empty()) {
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        }

        // Texture coordinate attribute (if exists)
        if (!mesh.texCoords.empty()) {
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                (void*)((3 + (!mesh.normals.empty() ? 3 : 0)) * sizeof(float)));
        }

        glBindVertexArray(0);
    }
}

void Model::Render() const {
    for (const auto& mesh : meshes) {
        glBindVertexArray(mesh.VAO);
        glDrawElements(GL_TRIANGLES, mesh.GetIndexCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void Model::Clear() {
    for (auto& mesh : meshes) {
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBO);
        glDeleteBuffers(1, &mesh.EBO);
    }
    meshes.clear();
}