#include "Model.h"
#include <iostream>

Model::Model(const std::string& modelPath, const std::string& texturePath)
{
    LoadTexture(texturePath);
    LoadModel(modelPath);

    if (m_Texture) {
        for (auto& mesh : m_Meshes) {
            mesh->SetTexture(m_Texture);
        }
    }
}

Model::Model(const std::string& modelPath)
{
    LoadModel(modelPath);
}

void Model::Draw(Shader& shader) const
{
    shader.Bind();

    for (const auto& mesh : m_Meshes) {
        const Material& mat = mesh->GetMaterial();
        shader.SetUniformVec3("uMaterial.kA", mat.kA);
        shader.SetUniformVec3("uMaterial.kD", mat.kD);
        shader.SetUniformVec3("uMaterial.kS", mat.kS);
        shader.SetUniform1f("uMaterial.shine", mat.shine);

        if (mesh->HasTexture()) {
            mesh->GetTexture().Bind(0);
            shader.SetUniform1i("uTexture", 0);
            shader.SetUniform1i("uHasTexture", 1);
        }
        else {
            shader.SetUniform1i("uHasTexture", 0);
        }

        mesh->Draw();

        if (mesh->HasTexture()) {
            mesh->GetTexture().Unbind();
        }
    }

    shader.Unbind();
}

void Model::SetTexture(std::shared_ptr<Texture> tex) {
    m_Texture = tex;
    for (auto& mesh : m_Meshes) {
        mesh->SetTexture(m_Texture);
    }
}

void Model::SetMaterial(const Material& material) {
    for (auto& mesh : m_Meshes) {
        mesh->SetMaterial(material);
    }
}

void Model::LoadTexture(const std::string& path) {
    try {
        m_Texture = std::make_shared<Texture>(path);
    }
    catch (const std::exception& e) {
        std::cout << "ERROR::TEXTURE::Failed to load texture: " << path << std::endl;
    }
}

void Model::LoadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_Meshes.push_back(ProcessMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

std::unique_ptr<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        if (mesh->HasNormals()) {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        if (mesh->mTextureCoords[0]) {
            vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    auto resultMesh = std::make_unique<Mesh>(vertices, indices);

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        Material mat;
        aiColor3D color;

        if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
            mat.kA = glm::vec3(color.r, color.g, color.b);
        }
        else {
            mat.kA = glm::vec3(0.1f);
        }

        if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
            mat.kD = glm::vec3(color.r, color.g, color.b);
        }
        else {
            mat.kD = glm::vec3(0.8f);
        }

        if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
            mat.kS = glm::vec3(color.r, color.g, color.b);
        }
        else {
            mat.kS = glm::vec3(1.0f);
        }

        float shininess;
        if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS) {
            mat.shine = shininess;
        }
        else {
            mat.shine = 32.0f;
        }

        resultMesh->SetMaterial(mat);
    }
    else {
        Material defaultMat(glm::vec3(0.1f), glm::vec3(0.8f), glm::vec3(1.0f), 32.0f);
        resultMesh->SetMaterial(defaultMat);
    }

    return resultMesh;
}