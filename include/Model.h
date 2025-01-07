#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>





class Model
{
    private:
        std::vector<std::shared_ptr<Mesh>> meshes;
        // std::vector<Mesh> meshes;
        std::string directory;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        std::shared_ptr<Mesh> processMesh(aiMesh *mesh, const aiScene *scene);

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
            aiProcess_Triangulate | aiProcess_FlipUVs
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


    // for (auto v : vertices)
    // {
    //     std::cout << v.Position.x << " " << v.Position.y << " " << v.Position.z << std::endl;
    // }
    //
    // for (auto i : indices)
    // {
    //     std::cout << i << std::endl;
    // }
    //

    return std::make_shared<Mesh>(vertices, indices);
}


