
#pragma once

// std
#include <string>
#include <sstream>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// project
#include "cgra_mesh.hpp"
#include "cgra_shader.hpp"
#include <opengl.hpp>


// stream insertion operator
namespace glm {

	template <typename T, int N>
	inline std::ostream & operator<<(std::ostream &out, const glm::vec<N, T> &v) {
		out << '(';
		if (N > 0) out << v[0];
		for (size_t i = 1; i < N; ++i) {
			out << ", " << v[i];
		}
		out << ')';
		return out;
	}

}

namespace cgra {

	namespace detail {
		inline void to_stream(std::ostream &) {}

		template <typename ArgT0, typename ...ArgTs>
		inline void to_stream(std::ostream &out, const ArgT0 &arg, const ArgTs &... args) {
			out << arg;
			to_stream(out, args...);
		}
	}


	// helper function to return a string from given arguments
	// requires every argument has an overload for the '<<' operator
	template <typename ...ArgTs>
	inline std::string stringf(const ArgTs &... args) {
		ostringstream oss;
		detail::to_stream(oss, args...);
		return oss.str();
	}


	// slices and stacks must be 2 or more
	inline mesh_builder cgraSphere(float radius, int slices = 10, int stacks = 10) {
		assert(slices >= 2 && stacks >= 2 && radius > 0);

		using namespace glm;

		int slices2 = slices * 2;

		// precompute sin/cos values for the range of phi
		std::vector<float> phi_vector;
		std::vector<float> sin_phi_vector;
		std::vector<float> cos_phi_vector;

		for (int j = 0; j <= slices2; ++j) {
			float phi = 2 * pi<float>() * float(j) / slices2;
			phi_vector.push_back(phi);
			sin_phi_vector.push_back(std::sin(phi));
			cos_phi_vector.push_back(std::cos(phi));
		}

		// compute vertices
		mesh_builder mb;
		for (int i = 0; i <= stacks; ++i) {
			float theta = pi<float>() * float(i) / stacks;
			float sin_theta = std::sin(theta);
			float cos_theta = std::cos(theta);

			for (int j = 0; j <= slices2; ++j) {
				mesh_vertex v;
				v.norm = vec3(
					sin_theta * cos_phi_vector[j],
					sin_theta * sin_phi_vector[j],
					cos_theta
				);
				v.pos = v.norm * radius;
				v.uv = vec2(float(j) / slices2, float(i) / stacks);
				mb.push_vertex(v);
			}
		}

		// compute indices
		for (int i = 0; i < stacks; ++i) {
			for (int j = 0; j < slices2; ++j) {
				int i0 = i * (slices2 + 1), i1 = (i + 1) * (slices2 + 1);
				int j0 = j, j1 = j + 1;
				// tri 1
				mb.push_index(i0 + j0);
				mb.push_index(i1 + j0);
				mb.push_index(i1 + j1);

				// tri 2
				mb.push_index(i0 + j0);
				mb.push_index(i1 + j1);
				mb.push_index(i0 + j1);

			}
		}

		return mb;
	}



	inline mesh_builder cgraCylinder(float base_radius, float top_radius, float height, int slices = 10) {
		assert(slices >= 2 && (base_radius > 0 || top_radius > 0) && height > 0);

		using namespace glm;

		int slices2 = slices * 2;

		// precompute sin/cos values for the range of phi
		std::vector<float> phi_vector;
		std::vector<float> sin_phi_vector;
		std::vector<float> cos_phi_vector;

		for (int j = 0; j <= slices2; ++j) {
			float phi = 2 * pi<float>() * float(j) / slices2;
			phi_vector.push_back(phi);
			sin_phi_vector.push_back(std::sin(phi));
			cos_phi_vector.push_back(std::cos(phi));
		}


		// compute vertices
		mesh_builder mb;

		// thanks ben, you shall forever be immortalized
		float bens_theta = pi<float>() / 2 * std::atan((base_radius - top_radius) / height);
		float sin_bens_theta = std::sin(bens_theta);
		float cos_bens_theta = std::cos(bens_theta);

		for (int i = 0; i < 2; ++i) {
			float t = float(i) / 1;
			float z = height * t;
			float width = base_radius + (top_radius - base_radius) * t;

			for (int j = 0; j <= slices2; ++j) {
				mesh_vertex v;
				v.pos = vec3(
					width * cos_phi_vector[j],
					width * sin_phi_vector[j],
					z
				);
				v.norm = vec3(
					cos_bens_theta * cos_phi_vector[j],
					cos_bens_theta * sin_phi_vector[j],
					sin_bens_theta
				);
				v.uv = vec2(float(j) / slices2, t);
				mb.push_vertex(v);
			}
		}

		// compute indices (body)
		for (int j = 0; j < slices2; ++j) {
			int i0 = 0, i1 = slices2 + 1;
			int j0 = j, j1 = j + 1;
			
			// tri 1
			if (base_radius > 0) {
				mb.push_index(i0 + j0);
				mb.push_index(i0 + j1);
				mb.push_index(i1 + j1);
			}

			// tri 2
			if (top_radius > 0) {
				mb.push_index(i0 + j0);
				mb.push_index(i1 + j1);
				mb.push_index(i1 + j0);
			}
		}

		// bottom
		if (base_radius > 0) {
			mesh_vertex c;
			c.norm = vec3(0, 0, -1);
			int center = mb.push_vertex(c);
			for (int j = 0; j <= slices2; ++j) {
				mesh_vertex v;
				v.pos = vec3(
					base_radius * cos_phi_vector[j],
					base_radius * sin_phi_vector[j],
					0
				);
				v.norm = vec3(0, 0, -1);
				mb.push_vertex(v);
			}

			for (int j = 0; j <= slices2; ++j) {
				mb.push_index(center);
				mb.push_index((center + 1) + (j + 1) % slices2);
				mb.push_index((center + 1) + j % slices2);
			}
		}

		// top
		if (top_radius > 0) {
			mesh_vertex c;
			c.pos = vec3(0, 0, height);
			c.norm = vec3(0, 0, 1);
			int center = mb.push_vertex(c);
			for (int j = 0; j <= slices2; ++j) {
				mesh_vertex v;
				v.pos = vec3(
					top_radius * cos_phi_vector[j],
					top_radius * sin_phi_vector[j],
					height
				);
				v.norm = vec3(0, 0, 1);
				mb.push_vertex(v);
			}

			for (int j = 0; j <= slices2; ++j) {
				mb.push_index(center);
				mb.push_index((center + 1) + j % slices2);
				mb.push_index((center + 1) + (j + 1) % slices2);
			}
		}

		return mb;
	}

	inline mesh_builder cgraCone(float base_radius, float height, int slices = 10) {
		return cgraCylinder(base_radius, 0, height, slices);
	}

}