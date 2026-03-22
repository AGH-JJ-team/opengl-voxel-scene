#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <assimp/scene.h>

class Model {
public:
    struct Mesh {
        std::vector<float> vertices;
        std::vector<float> normals;
        std::vector<float> texCoords;
        std::vector<unsigned int> indices;

        unsigned int VAO = 0;
        unsigned int VBO = 0;
        unsigned int EBO = 0;

        unsigned int GetIndexCount() const { return indices.size(); }
    };

    Model() = default;
    ~Model() { Clear(); }

    bool LoadFromFile(const std::string& filepath);
    void SetupMesh();
    void Render() const;
    void Clear();

    const std::vector<Mesh>& GetMeshes() const { return meshes; }

private:
    std::vector<Mesh> meshes;

    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
};

#endif // MODEL_H