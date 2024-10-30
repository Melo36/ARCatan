#pragma once
#include <string>
#include <iostream>

#include "Renderer.h"
#include "Texture.h"
#include "VertexBufferLayout.h"
#include "Material.h"

enum Meshtype {PNT, PNC,PN,P, EMPTY};

struct VertexPNT {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;

    VertexPNT(float px, float py, float pz, float nx, float ny, float nz, float u, float v) 
        :Position(glm::vec3(px,py,pz)),Normal(glm::vec3(nx,ny,nz)),TexCoords(glm::vec2(u,v))
    {}
    VertexPNT() {};
};

struct VertexPNC {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    //color
    glm::vec4 Color;
};

struct VertexPN {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
};

struct VertexP {
    // position
    glm::vec3 Position;
};

class Mesh {
public:
    Mesh();
    Mesh(std::vector<VertexPNT> vertices, std::vector<unsigned int> indices, Material material);
    Mesh(std::vector<VertexPNC> vertices, std::vector<unsigned int> indices, Material material);
    Mesh(std::vector<VertexPN> vertices, std::vector<unsigned int> indices, Material material);
    Mesh(std::vector<VertexP> vertices, std::vector<unsigned int> indices, Material material);
    void draw(std::shared_ptr<Shader> shader, Renderer* renderer, Material* overrideMaterial = nullptr);

    Mesh(const Mesh& other);

    //bool hasTextures = false;
private:
    //std::vector<std::shared_ptr<Texture>>  textures;
    std::shared_ptr<IndexBuffer> m_indexBuffer;
    std::shared_ptr<VertexBuffer> m_vertexBuffer;
    std::shared_ptr<VertexArray> m_vao;
    Meshtype m_type;
    Material m_material;

    void setupMeshPNT(std::vector<VertexPNT>  vertices, std::vector<unsigned int> indices);
    void setupMeshPNC(std::vector<VertexPNC>  vertices, std::vector<unsigned int> indices);
    void setupMeshPN(std::vector<VertexPN>  vertices, std::vector<unsigned int> indices);
    void setupMeshP(std::vector<VertexP>  vertices, std::vector<unsigned int> indices);
    //void setupTextures(std::shared_ptr<Shader> shader);

};