#pragma once
#include <assimp/material.h>
#include <iostream>
#include <memory>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"



class Model
{
    private:
        std::vector<std::shared_ptr<Mesh>> meshes;
        std::vector<Texture> textures_loaded;
        std::string directory;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        std::shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,std::string typeName);
        unsigned int textureFromFile(const std::string &path, const std::string &directory);

    public:
        Model(std::string path)
        {
            loadModel(path);
        }
        void Draw();


};


void Model::Draw()
{
    for( unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->Draw();
    }
}

void Model::loadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(
            path, 
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals
    );

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);

}
    

void Model::processNode(aiNode *node, const aiScene *scene)
{

    // meshes.reserve(node->mNumMeshes);

    std::cout << "node->mNumMeshes: " << node->mNumMeshes << std::endl;

    for( unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
        // meshes.push_back(std::make_shared<Mesh>(processMesh(mesh, scene)));
        
    }
    
    // children nodeを処理
    // nodeが階層構造になっているときに、親子関係を定義できる。
    for( unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

    std::cout << "meshes.size(): " << meshes.size() << std::endl;

}


std::shared_ptr<Mesh> Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    std::cout << "mesh->mNumVertices: " << mesh->mNumVertices << std::endl;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++){
        Vertex vertex;
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        glm::vec3 normal;
        normal.x = mesh->mNormals[i].x;
        normal.y = mesh->mNormals[i].y;
        normal.z = mesh->mNormals[i].z;
        vertex.Normal = normal;
   
        // assimpは最大8つまでですが、今は1つだけを見ようということらしい。
        // if(mesh->mTextureCoords[0])
        if(mesh->HasTextureCoords(0))
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else{
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }


        vertices.push_back(vertex);

    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0){
        std::cout << "Material index: " << mesh->mMaterialIndex << std::endl;
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        
        // diffuse map (color?)
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // specular map
        std::vector<Texture> specularMaps = loadMaterialTextures(
            material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        // normal map
        std::vector<Texture> normalMaps = loadMaterialTextures(
            material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

        // Ambient map
        std::vector<Texture> ambientMaps = loadMaterialTextures(
            material, aiTextureType_AMBIENT, "texture_ambient");
        textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
    }
    else {
        std::cerr << "Material index is negative." << std::endl;
    }

    std::cout << "loaded_Texture >>> " << std::endl;
    for (auto t : textures)
    {
        std::cout << t.id << " " << t.type << " " << t.path << std::endl;
    }

    return std::make_shared<Mesh>(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        aiReturn error = mat->GetTexture(type, i, &str);
        if (error != aiReturn_SUCCESS)
        {
            std::cerr << "Can't load the texture." << std::endl;
            continue;
        }


        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(textures_loaded[j].path.data() == str.C_Str())
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if(!skip)
        {
            Texture texture;
            texture.id = textureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}

unsigned int Model::textureFromFile(const std::string &path, const std::string &directory)
{
    std::string tex = directory + '/' + path;

    std::cout << "textureFromFilr :"<<  tex << std::endl;


    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // texture option
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate texture
    int width, height, nrChannels;
    unsigned char *data = stbi_load(tex.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
    

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else 
    {
        std::cerr << "Can't load the texture." << std::endl;

    }

    stbi_image_free(data);

    return texture;
}
