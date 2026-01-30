#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <memory>
#include "../Mesh.h"
#include "../Texture.h"
#include "../Shader.h"

class Model
{
public:
    Model(const std::string& modelPath, const std::string& texturePath);
    Model(const std::string& modelPath);

    ~Model() = default;

    void Draw(Shader& shader) const;

    void SetTexture(std::shared_ptr<Texture> tex);
    void SetMaterial(const Material& material);

    inline Texture& GetTexture() const { return *m_Texture; }
private:
    void LoadTexture(const std::string& path);
    void LoadModel(const std::string& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    std::unique_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene);
private:
    std::vector<std::unique_ptr<Mesh>> m_Meshes;
    std::shared_ptr<Texture> m_Texture;
};
