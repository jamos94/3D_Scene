#pragma once
#include <array>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glad/glad.h>

namespace model {

	struct Face {
		std::array<unsigned, 3> vi;
		std::array<unsigned, 3> ni;
		std::array<unsigned, 3> ti;
	};

	struct Mesh {
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<Face> faces;
	};

	class Model {
		GLuint vao, vbo;
		GLuint vertCount;
		GLenum vertexMode;

		struct Vertex {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texCoord;
		};

	public:
		Model(Mesh const& mesh, GLenum vertexMode)
			: vertexMode(vertexMode)
		{
			// Explode vertices
			std::vector<Vertex> vertices;
			for (auto const& face : mesh.faces) {
				for (int v = 0; v < 3; v++) {
					vertices.push_back(Vertex{
						mesh.positions[face.vi[v] - 1],
						mesh.normals[face.ni[v] - 1],
						mesh.texCoords[face.ti[v] - 1]
						});
				}
			}
			vertCount = static_cast<GLuint>(vertices.size());

			const GLuint POSITION_INDEX = 0;
			const GLuint NORMAL_INDEX = 1;
			const GLuint TEXCOORD_INDEX = 2;

			glCreateBuffers(1, &vbo);
			glNamedBufferStorage(vbo, vertCount * (3 + 3 + 2) * 4, vertices.data(), 0);

			glCreateVertexArrays(1, &vao);

			glEnableVertexArrayAttrib(vao, POSITION_INDEX);
			glEnableVertexArrayAttrib(vao, NORMAL_INDEX);
			glEnableVertexArrayAttrib(vao, TEXCOORD_INDEX);

			const GLuint stride = 4 * (3 + 3 + 2);

			const GLuint offsetPosition = 0;
			glVertexArrayVertexBuffer(vao, POSITION_INDEX, vbo, 0, stride);
			glVertexArrayAttribFormat(vao, POSITION_INDEX, 3, GL_FLOAT, GL_FALSE, offsetPosition);
			glVertexArrayAttribBinding(vao, POSITION_INDEX, POSITION_INDEX);

			const GLuint offsetNormal = offsetPosition + 4 * 3; // 2nd attribute offset by the size of the first one
			glVertexArrayVertexBuffer(vao, NORMAL_INDEX, vbo, 0, stride);
			glVertexArrayAttribFormat(vao, NORMAL_INDEX, 3, GL_FLOAT, GL_FALSE, offsetNormal);
			glVertexArrayAttribBinding(vao, NORMAL_INDEX, NORMAL_INDEX);

			const GLuint offsetTexCoord = offsetNormal + 4 * 3;
			glVertexArrayVertexBuffer(vao, TEXCOORD_INDEX, vbo, 0, stride);
			glVertexArrayAttribFormat(vao, TEXCOORD_INDEX, 2, GL_FLOAT, GL_FALSE, offsetTexCoord);
			glVertexArrayAttribBinding(vao, TEXCOORD_INDEX, TEXCOORD_INDEX);
		}

		Model(Model const&) = delete;
		Model& operator=(Model const&) = delete;

		~Model() {
			glDeleteVertexArrays(1, &vao);
			glDeleteBuffers(1, &vbo);
		}

		void draw() const {
			glBindVertexArray(vao);
			glDrawArrays(vertexMode, 0, vertCount);
			glBindVertexArray(0);
		}
	};

	static std::vector<float> createBox() {
		return {
			-1.f,  1.f,  1.f,    // Front-top-left
			 1.f,  1.f,  1.f,    // Front-top-right
			-1.f, -1.f,  1.f,    // Front-bottom-left
			 1.f, -1.f,  1.f,    // Front-bottom-right
			 1.f, -1.f, -1.f,    // Back-bottom-right
			 1.f,  1.f,  1.f,    // Front-top-right
			 1.f,  1.f, -1.f,    // Back-top-right
			-1.f,  1.f,  1.f,    // Front-top-left
			-1.f,  1.f, -1.f,    // Back-top-left
			-1.f, -1.f,  1.f,    // Front-bottom-left
			-1.f, -1.f, -1.f,    // Back-bottom-left
			 1.f, -1.f, -1.f,    // Back-bottom-right
			-1.f,  1.f, -1.f,    // Back-top-left
			 1.f,  1.f, -1.f     // Back-top-right
		};
	}

	static Mesh loadMesh(std::string const& path) {
		Mesh result;

		std::fstream file(path);

		std::string line;
		while (std::getline(file, line)) {
			std::istringstream lineS(line);

			std::string type;
			lineS >> type;

			if (type == "v") {
				float x, y, z;
				lineS >> x >> y >> z;

				result.positions.push_back(glm::vec3(x, y, z));
			}
			else if (type == "vn") {
				float x, y, z;
				lineS >> x >> y >> z;

				result.normals.push_back(glm::vec3(x, y, z));
			}
			else if (type == "vt") {
				float u, v;
				lineS >> u >> v;

				result.texCoords.push_back(glm::vec2(u, v));
			}
			else if (type == "f") {
				result.faces.push_back(Face{});
				auto& f = result.faces.back();

				sscanf_s(line.c_str(),
					"f %u/%u/%u %u/%u/%u %u/%u/%u",
					&f.vi[0], &f.ti[0], &f.ni[0],
					&f.vi[1], &f.ti[1], &f.ni[1],
					&f.vi[2], &f.ti[2], &f.ni[2]
				);
			}
		}

		return result;
	}

}
