#pragma once
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture{
    unsigned int id;
    std::string type;
    std::string path;
};
    


class Mesh {
    private:
        unsigned int VAO, VBO, EBO;

        void setupMesh();

    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        ~Mesh();

        // copy constructor
        Mesh(const Mesh &other) ;

        // copy assignment
        Mesh &operator=(const Mesh &other);



        void Draw();
};


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

// destructor
Mesh::~Mesh() {
    std::cout << "destructor" << std::endl;
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

// copy constructor
Mesh::Mesh(const Mesh &other) {
    std::cout << "copy constructor" << std::endl;
    this->vertices = other.vertices;
    this->indices = other.indices;
    this->textures = other.textures;

    setupMesh();
}

// copy assignment( class func)
Mesh & Mesh::operator=(const Mesh &other) {
    std::cout << "copy assignment" << std::endl;
    if (this == &other) {
        return *this;
    }

    this->vertices = other.vertices;
    this->indices = other.indices;
    // this->textures = other.textures;

    setupMesh();

    return *this;
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(Vertex),
            &vertices[0],
            GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            &indices[0],
            GL_STATIC_DRAW
    );

    // position
    glEnableVertexAttribArray(0);
    
    // vertex + 2 (texture test)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); 

    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));



    // VAOのbindの解除
    glBindVertexArray(0);

}

void Mesh::Draw() {



   
    for(unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}


