
#pragma once

// glm
#include <glm/glm.hpp>

// project
#include "opengl.hpp"
#include "cgra/cgra_mesh.hpp"


inline static cgra::gl_mesh createBoundingBoxMesh(const glm::vec3 &minv, const glm::vec3 &maxv) {
	
	cgra::mesh_builder mb;

	mb.push_vertex(cgra::mesh_vertex{ glm::vec3(maxv.x, maxv.y, maxv.z) });
	mb.push_vertex(cgra::mesh_vertex{ glm::vec3(maxv.x, maxv.y, minv.z) });
	mb.push_vertex(cgra::mesh_vertex{ glm::vec3(maxv.x, minv.y, maxv.z) });
	mb.push_vertex(cgra::mesh_vertex{ glm::vec3(maxv.x, minv.y, minv.z) });
	mb.push_vertex(cgra::mesh_vertex{ glm::vec3(minv.x, maxv.y, maxv.z) });
	mb.push_vertex(cgra::mesh_vertex{ glm::vec3(minv.x, maxv.y, minv.z) });
	mb.push_vertex(cgra::mesh_vertex{ glm::vec3(minv.x, minv.y, maxv.z) });
	mb.push_vertex(cgra::mesh_vertex{ glm::vec3(minv.x, minv.y, minv.z) });

	mb.push_indices({
		0, 1, 0, 2, 0, 4,
		1, 3, 1, 5, 2, 3,
		2, 6, 3, 7, 4, 5,
		4, 6, 5, 7, 6, 7
	});

	mb.mode = GL_LINES;

	return mb.build();
}