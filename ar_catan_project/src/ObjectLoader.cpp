#include "ObjectLoader.h"
#include "VertexBufferLayout.h"
#include <iostream>

ObjectLoader* ObjectLoader::inst_ = NULL;

ObjectLoader* ObjectLoader::getInstance()
{
	if (inst_ == NULL) {
		inst_ = new ObjectLoader();
	}
	return (inst_);
}

std::vector<std::shared_ptr<Mesh>>  ObjectLoader::loadObjectFromFile(const std::string& filepath, Meshtype type)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
	{ 
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return std::vector<std::shared_ptr<Mesh>>();
	}
	std::string directory = filepath.substr(0, filepath.find_last_of('/'));

	std::vector<std::shared_ptr<Mesh>> meshes;

	for (int i = 0; i < scene->mRootNode->mNumChildren; i++) {
		//process meshes
		aiNode* node = scene->mRootNode->mChildren[i];

		for(unsigned int i=0; i<node->mNumMeshes; i++) {
			aiMesh*mesh=scene->mMeshes[node->mMeshes[i]];

			meshes.push_back(processMesh(mesh,scene,type));
		}
	}

	return meshes;
}

std::shared_ptr<Mesh> ObjectLoader::loadCubeVTN()
{
	//load data and prepare vertex arrays, shaders, and textures

	std::vector<VertexPNT> vertices;

	// Front face
	vertices.push_back(VertexPNT(- 1.0, -1.0, 1.0, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f));	// left bottom        0
	vertices.push_back(VertexPNT(1.0, -1.0, 1.0, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f));	// right bottom       1
	vertices.push_back(VertexPNT(-1.0, 1.0, 1.0, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f));	// left top           2
	vertices.push_back(VertexPNT(1.0, 1.0, 1.0, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f));		// right top          3
	// back face
	vertices.push_back(VertexPNT(-1.0, -1.0, -1.0, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f));	// left bottom        4
	vertices.push_back(VertexPNT(1.0, -1.0, -1.0, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f));	// right bottom       5
	vertices.push_back(VertexPNT(-1.0, 1.0, -1.0, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f));	// left top           6
	vertices.push_back(VertexPNT(1.0, 1.0, -1.0, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f));	// right top          7

	// left side face
	vertices.push_back(VertexPNT(-1.0, -1.0, -1.0, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f));	// left bottom        8
	vertices.push_back(VertexPNT(-1.0, -1.0, 1.0, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f));	// right bottom       9
	vertices.push_back(VertexPNT(-1.0, 1.0, -1.0, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f));	// left top           10
	vertices.push_back(VertexPNT(-1.0, 1.0, 1.0, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f));	// right top          11

	// right side face
	vertices.push_back(VertexPNT(1.0, -1.0,  1.0,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f)); // left bottom        12
	vertices.push_back(VertexPNT(1.0, -1.0, -1.0,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f)); // right bottom       13
	vertices.push_back(VertexPNT(1.0,  1.0,  1.0,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f)); // left top           14
	vertices.push_back(VertexPNT(1.0,  1.0, -1.0,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f)); // right top          15

	// bottom face
	vertices.push_back(VertexPNT(-1.0, -1.0, -1.0, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f)); // left bottom        16
	vertices.push_back(VertexPNT(1.0, -1.0, -1.0, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f));	 // right bottom       17
	vertices.push_back(VertexPNT(-1.0, -1.0,  1.0, 0.0f, -1.0f,  0.0f, 0.0f, 1.0f)); // left top           18
	vertices.push_back(VertexPNT(1.0, -1.0,  1.0, 0.0f, -1.0f,  0.0f, 1.0f, 1.0f));	 // right top          19

	// top face
	vertices.push_back(VertexPNT(-1.0,  1.0,  1.0, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f)); // left bottom        20
	vertices.push_back(VertexPNT(1.0,  1.0,  1.0, 0.0f,  1.0f,  0.0f, 1.0f, 0.0f));	 // right bottom       21
	vertices.push_back(VertexPNT(-1.0,  1.0, -1.0, 0.0f,  1.0f,  0.0f, 0.0f, 1.0f)); // left top           22
	vertices.push_back(VertexPNT(1.0,  1.0, -1.0, 0.0f,  1.0f,  0.0f, 1.0f, 1.0f));  // right top          23

	

	std::vector<unsigned int> indices{
		0, 1, 2,
		1, 2, 3,
		4, 5, 6,
		5, 6, 7,
		8, 9, 10,
		9, 10, 11,
		12, 13, 14,
		13, 14, 15,
		16, 17, 18,
		17, 18, 19,
		20, 21, 22,
		21, 22, 23
	};
	
	return std::make_shared<Mesh>(vertices,indices,Material(1.0));
}

ObjectLoader::~ObjectLoader()
{

}

std::shared_ptr<Mesh> ObjectLoader::processMesh(aiMesh* mesh, const aiScene* scene, Meshtype type) {

	//calc indices. same procedure for all mesh types
	std::vector<unsigned int> indices;

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	switch (type) {
	case P:
		{
			std::vector<VertexP> verticesP;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				VertexP vertex;
				glm::vec3 vector(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				vertex.Position = vector;
				verticesP.push_back(vertex);
			}

			return std::make_shared<Mesh>(verticesP, indices,Material(1.0));
		}

	case PN:
		{
			std::vector<VertexPN> verticesPN;
			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				VertexPN vertex;
				glm::vec3 vector(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				vertex.Position = vector;
				if (mesh->HasNormals()) {
					vector = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
					vertex.Normal = vector;
				}
				else {
					//skip vertices with no normals
					continue;
				}
				verticesPN.push_back(vertex);
			}

			return std::make_shared<Mesh>(verticesPN, indices,Material(1.0));
		}
	case PNC:
		{
			std::vector<VertexPNC> verticesPNC;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				VertexPNC vertex;
				glm::vec3 vector(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				vertex.Position = vector;
				if (mesh->HasNormals()) {
					vector = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
					vertex.Normal = vector;
				}
				else {
					//skip vertices with no normals
					continue;
				}
				if (mesh->HasVertexColors(0)) {
				
					float alpha = (mesh->GetNumColorChannels() == 4) ? mesh->mColors[0][i].a : 1.0;
					glm::vec4 color(mesh->mColors[0][i].r, mesh->mColors[0][i].g, mesh->mColors[0][i].b, alpha);
					vertex.Color = color;
				}
				else {
					//set black color
					vertex.Color = glm::vec4(0.0f, 0.0f,0.0f,1.0f);
				}

				verticesPNC.push_back(vertex);
			}

			Material mat = Material(1.0);

			if (mesh->mMaterialIndex >= 0) {
				//->load colors from material
						
				aiColor3D ambient(1.f, 1.f, 1.f);
				scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
					
				aiColor3D diffuse(1.f, 1.f, 1.f);
				scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
				//std::cout << "Num Mat " << diffuse.r << std::endl;

				aiColor3D specular(1.f, 1.f, 1.f);
				scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_SPECULAR, specular);

				float shininess(1.);
				scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_SHININESS, shininess);
				//if (shininess == 0)
					//shininess = 0.1;

				mat = Material(glm::vec4(ambient.r, ambient.g, ambient.b, 1.0), glm::vec4(diffuse.r, diffuse.g, diffuse.b, 1.0), glm::vec4(specular.r, specular.g, specular.b, 1.0), shininess);
			}
			
			
			return std::make_shared<Mesh>(verticesPNC, indices,mat);
		}

	case PNT:
		{
			std::vector<VertexPNT> verticesPNT;
			//std::vector<std::shared_ptr<Texture>> textures;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
				VertexPNT vertex;
				glm::vec3 vector(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
				vertex.Position = vector;
				if (mesh->HasNormals()) {
					vector = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
					vertex.Normal = vector;
				}
				else {
					//skip vertices with no normals
					continue;
				}
				if (mesh->HasTextureCoords(0)) {

					glm::vec2 texCoords(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
					vertex.TexCoords = texCoords;
				}
				else {
					//skip vertices with no texture coordinate
					vertex.TexCoords = glm::vec2(0.0f, 0.0f);
				}

				verticesPNT.push_back(vertex);
			}

			Material mat = Material(1.0);

			//load textures from material
			if (mesh->mMaterialIndex >= 0) { 
				aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				std::vector<std::shared_ptr<Texture>>diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::diffuse);
				//textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
				std::vector<std::shared_ptr<Texture>>specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::specular);
				//textures.insert(textures.end(), specularMaps.begin(), specularMaps.end()); 
				float shininess(1.);
				scene->mMaterials[mesh->mMaterialIndex]->Get(AI_MATKEY_SHININESS, shininess);

				mat = Material(diffuseMaps[0], specularMaps[0], shininess);
			}

			return std::make_shared<Mesh>(verticesPNT, indices,mat);
		}

	}
	return std::make_shared<Mesh>(std::vector<VertexP>(),std::vector<unsigned int>(),Material(1.0));
}

std::vector<std::shared_ptr<Texture>> ObjectLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType t_type)
{
	std::cout << "M " << type << " " << mat->GetTextureCount(type) << std::endl;

	std::vector<std::shared_ptr<Texture>> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		std::cout << type << " " << str.C_Str() << std::endl;
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j]->getPath().data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			std::shared_ptr<Texture> texture = std::make_shared<Texture>(str.C_Str(), t_type);
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
		}
	}

	return textures;
};
