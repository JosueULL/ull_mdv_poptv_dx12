#pragma once

#include <vector>
#include <glm/glm.hpp>

#pragma warning(disable : 4201)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

using namespace std;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
	glm::vec3 color;

	bool operator==(const Vertex& other) const {
		return position == other.position && color == other.color && uv == other.uv && normal == other.normal;
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return (((hash<glm::vec3>()(vertex.position) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.uv) << 1) >> 1) ^
				(hash<glm::vec3>()(vertex.normal) << 1);
		}
	};
}

class Mesh 
{
public:
	vector<Vertex> vertices;
	vector<int> indices;

	virtual ~Mesh() {
		vertices.clear();
		indices.clear();
	}

};
