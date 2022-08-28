
#pragma once

// glm
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// project
#include "opengl.hpp"
#include "cgra/cgra_mesh.hpp"

#include <string>

#define U_TEXTURE 2 //texture binded to location 2 in vertex shader

// Basic model that holds the shader, mesh and transform for drawing.
// Can be copied and/or modified for adding in extra information for drawing
// including colors for diffuse/specular, and textures for texture mapping etc.
struct basic_model {
	GLuint shader = 0;
	cgra::gl_mesh mesh;
	glm::vec3 color;
	glm::mat4 modelTransform{1.0};
    //phong shading fields
    glm::vec3 lightcolor;
    glm::vec3 speccolor;
    float shininess;
    
    //bounding box
    //glm::mat4 boundingBox = glm::mat4(1.0f);
    
    //booleans to determine what type of colour the fragment shader should load
    bool loadTexture = false;
    bool useColorInstances = false;

	void draw(const glm::mat4 &view, const glm::mat4 proj) {
		using namespace glm;

		// calculate the modelview transform
		mat4 modelview = view * modelTransform;

		// load shader and variables
		glUseProgram(shader);
		glUniformMatrix4fv(glGetUniformLocation(shader, "uProjectionMatrix"), 1, false, value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(shader, "uModelViewMatrix"), 1, false, value_ptr(modelview));
		glUniform3fv(glGetUniformLocation(shader, "uColor"), 1, value_ptr(color));
        //phong shading uniforms
        glUniform3fv(glGetUniformLocation(shader, "ulightcolor"), 1, value_ptr(lightcolor));
        glUniform3fv(glGetUniformLocation(shader, "uspeccolor"), 1, value_ptr(speccolor));
        glUniform1f(glGetUniformLocation(shader, "ushininess"), shininess);

        //texture uniform
        glUniform1i(U_TEXTURE, 0);
        
        //bounding box
        //glUniformMatrix4fv(glGetUniformLocation(shader, "uBoundingBox"), 1, GL_FALSE, glm::value_ptr(boundingBox));
        
        //bools
        glUniform1i(glGetUniformLocation(shader, "loadTexture"), loadTexture);
        glUniform1i(glGetUniformLocation(shader, "useColorInstances"), useColorInstances);
        
		// draw the mesh
		mesh.draw(); 
	}
};
