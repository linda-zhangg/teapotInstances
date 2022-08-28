
// std
#include <stdexcept>

//glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "cgra_mesh.hpp"

#include "cgra/cgra_image.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


using namespace glm;
using namespace std;

namespace cgra {

	void gl_mesh::draw() {
		if (vao == 0) return;
		// bind our VAO which sets up all our buffers and data for us
		glBindVertexArray(vao);
		// tell opengl to draw our VAO using the draw mode and how many verticies to render
        if(!drawInstances){
            glDrawElementsInstanced(mode, index_count, GL_UNSIGNED_INT, 0, 1);
        } else{
            glDrawElementsInstanced(mode, index_count, GL_UNSIGNED_INT, 0, 100);
        }
	}

	void gl_mesh::destroy() {
		// delete the data buffers
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
        glDeleteBuffers(1, &colVbo);
        glDeleteBuffers(1, &instanceVbo);
	}


	gl_mesh mesh_builder::build() const {
        gl_mesh m;
        //populate transformation matrices for instancing
        m.instanceColors.clear();
        m.transformations.push_back(mat4(1.0f));   //always load the first one as the central teapot
        m.instanceColors.push_back(vec3(0.8, 1, 1)); //always load the central teapot as default color
        
        //got this transformation layout idea from tutorial slide links + some changes
        
        srand(static_cast<unsigned int>(glfwGetTime())); // get semi-random number
        float radius = 70;
        float offset = 20;
        for (unsigned int i = 0; i <99; i++){
            mat4 model = mat4(1.0f);
            //translation - infinity ring around central teapot
            float angle = (float)i / (float)50 * 360.0f;
            float displacement = (rand() % (int)(offset * 200)) / 100.0f - offset;
            float x = sin(angle) * radius + displacement;
            float y = tan(angle) * radius + displacement;
            float z = cos(angle) * radius + displacement;
            model = translate(model, vec3(x, y, z));
            //scale
            float scale = (rand() % 40) / 50.0;
            model = glm::scale(model, vec3(scale));
            //rotation
            float rotation = (rand() % 360);
            model = rotate(model, rotation, vec3(0.4f, 0.5f, 0.6f));

            m.transformations.push_back(model);
            
            m.instanceColors.push_back(vec3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX),static_cast <float> (rand()) / static_cast <float> (RAND_MAX)));
        }
        
        //load texture using help from image_hpp
        cgra::rgba_image texture_data("res/textures/checkerboard.jpg");
        //if not opening, please set custom work directory to work folder
        m.m_texture = texture_data.uploadTexture();
        
        glGenVertexArrays(1, &m.vao); // VAO stores information about how the buffers are set up
        glGenBuffers(1, &m.vbo); // VBO stores the vertex data
        glGenBuffers(1, &m.ibo); // IBO stores the indices that make up primitives

        // VAO
        //
        glBindVertexArray(m.vao);


        // VBO (single buffer, interleaved)
        //
        glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
        // upload ALL the vertex data in one buffer
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(mesh_vertex), &vertices[0], GL_STATIC_DRAW);

        // this buffer will use location=0 when we use our VAO
        glEnableVertexAttribArray(0);
        // tell opengl how to treat data in location=0 - the data is treated in lots of 3 (3 floats = vec3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), (void *)(offsetof(mesh_vertex, pos)));
        //glVertexAttribDivisor(0,1);

        // do the same thing for Normals but bind it to location=1
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), (void *)(offsetof(mesh_vertex, norm)));

        // do the same thing for UVs but bind it to location=2 - the data is treated in lots of 2 (2 floats = vec2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(mesh_vertex), (void *)(offsetof(mesh_vertex, uv)));

        //color vbo
        glGenBuffers(1,&m.colVbo);
        glBindVertexArray(m.vao);
        glBindBuffer(GL_ARRAY_BUFFER, m.colVbo);
        glBufferData(GL_ARRAY_BUFFER, m.instanceColors.size() * sizeof(glm::vec3), &m.instanceColors[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)(sizeof(vec3)));
        glVertexAttribDivisor(3, 1);
        
        //instance vbo
        glGenBuffers(1, &m.instanceVbo); //instance vbo
        glBindVertexArray(m.vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, m.instanceVbo);
        glBufferData(GL_ARRAY_BUFFER, m.transformations.size() * sizeof(glm::mat4), &m.transformations[0], GL_STATIC_DRAW);
        
        //for a mat4 attribute
        for (int i = 0; i < 4; i++) {
          glEnableVertexAttribArray(4 + i);
          glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)(i * sizeof(vec4)));
          glVertexAttribDivisor(4 + i, 1);
        }
        
        
        // IBO
        //
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ibo);
        // upload the indices for drawing primitives
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), &indices[0], GL_STATIC_DRAW);


        // set the index count and draw modes
        m.index_count = indices.size();
        m.mode = mode;

        // clean up by binding VAO 0 (good practice)
        glBindVertexArray(0);

        return m;
	}
}
