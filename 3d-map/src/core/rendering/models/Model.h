#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include <string>
#include <vector>
#include <memory>
#include "../Texture.h"
#include "../Mesh.h"
#include "../Shader.h"

class Model
{
public:
    Model(const std::string& modelPath, const std::string& texturePath);
    ~Model() = default;

    void Draw(Shader& shader) const;
private:
    void LoadTexture(const std::string& path);
    void LoadModel(const std::string& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    std::unique_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
private:
    std::vector<std::unique_ptr<Mesh>> m_Meshes;
    std::unique_ptr<Texture> m_Texture;

};
