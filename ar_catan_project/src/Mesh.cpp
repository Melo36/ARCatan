#include "Mesh.h"

Mesh::Mesh()
    :m_type(EMPTY),m_material(Material(0.0))
{
}

Mesh::Mesh(std::vector<VertexPNT> vertices, std::vector<unsigned int> indices, Material material)
    :m_type(PNT), m_material(material)
{
    setupMeshPNT(vertices, indices);
}

Mesh::Mesh(std::vector<VertexPNC> vertices, std::vector<unsigned int> indices, Material material)
    :m_type(PNC), m_material(material)
{
    setupMeshPNC(vertices, indices);
}


Mesh::Mesh(std::vector<VertexPN> vertices, std::vector<unsigned int> indices, Material material)
    :m_type(PN), m_material(material)
{
    setupMeshPN(vertices,indices);
}

Mesh::Mesh(std::vector<VertexP> vertices, std::vector<unsigned int> indices, Material material)
    :m_type(P), m_material(material)
{
    setupMeshP(vertices, indices);
}

void Mesh::draw(std::shared_ptr<Shader> shader, Renderer* renderer, Material* overrideMaterial)
{
    if (m_type != EMPTY) {
        if (overrideMaterial == nullptr) {
            m_material.setMaterialUniforms(shader);
        }
        else {
            overrideMaterial->setMaterialUniforms(shader);
        }

        //render vertices
        renderer->draw(*m_vao, *m_indexBuffer, *shader);
    }
}

Mesh::Mesh(const Mesh& other)
    :m_indexBuffer(other.m_indexBuffer),m_vertexBuffer(other.m_vertexBuffer),m_vao(other.m_vao), m_type(other.m_type), m_material(other.m_material)
{
}

void Mesh::setupMeshPNT(std::vector<VertexPNT>  vertices, std::vector<unsigned int> indices)
{
    m_vao = std::make_shared<VertexArray>();
    m_vertexBuffer = std::make_shared<VertexBuffer>(&vertices[0], vertices.size() * sizeof(VertexPNT));

    VertexBufferLayout layout;
    layout.push<float>(3); //positions
    layout.push<float>(3); //normals
    layout.push<float>(2); //texture coordinates
    m_vao->addBuffer(*m_vertexBuffer, layout);

    m_indexBuffer = std::make_shared<IndexBuffer>(&indices[0], indices.size());
}

void Mesh::setupMeshPNC(std::vector<VertexPNC> vertices, std::vector<unsigned int> indices)
{
    m_vao = std::make_shared<VertexArray>();
    m_vertexBuffer = std::make_shared<VertexBuffer>(&vertices[0], vertices.size() * sizeof(VertexPNC));

    VertexBufferLayout layout;
    layout.push<float>(3); //positions
    layout.push<float>(3); //normals
    layout.push<float>(4); //colors
    m_vao->addBuffer(*m_vertexBuffer, layout);

    m_indexBuffer = std::make_shared<IndexBuffer>(&indices[0], indices.size());
}

void Mesh::setupMeshPN(std::vector<VertexPN>  vertices, std::vector<unsigned int> indices)
{
    GLCall(m_vao = std::make_shared<VertexArray>());
    GLCall(m_vertexBuffer = std::make_shared<VertexBuffer>(&vertices[0], vertices.size() * sizeof(VertexPN)));

    VertexBufferLayout layout;
    GLCall(layout.push<float>(3)); //positions
    GLCall(layout.push<float>(3)); //normals
    m_vao->addBuffer(*m_vertexBuffer, layout);
    GLCall(m_indexBuffer = std::make_shared<IndexBuffer>(&indices[0], indices.size()));
}

void Mesh::setupMeshP(std::vector<VertexP> vertices, std::vector<unsigned int> indices)
{
    m_vao = std::make_shared<VertexArray>();
    m_vertexBuffer = std::make_shared<VertexBuffer>(&vertices[0], vertices.size() * sizeof(VertexP));

    VertexBufferLayout layout;
    layout.push<float>(3); //positions
    m_vao->addBuffer(*m_vertexBuffer, layout);

    m_indexBuffer = std::make_shared<IndexBuffer>(&indices[0], indices.size());
}

/*
void Mesh::setupTextures(std::shared_ptr<Shader> shader) {
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int noneNr = 0;

    for (int i = 0; i < textures.size(); i++) {
        textures[i]->bind(i);

        std::string number;
        std::string name;
        switch (textures[i]->m_type) {
        case diffuse:
            name = "material.texture_d";
            number = std::to_string(diffuseNr++);
            break;
        case specular:
            name = "material.texture_s";
            number = std::to_string(specularNr++);
            break;
        case None:
            name = "texture";
            number = std::to_string(noneNr++);
            break;
        default:
            std::cout << "Setting texture uniforms failed: Unknown texture type.";
            continue;
        }
        shader->setUniform1i((name + number), i);
    }
}*/
