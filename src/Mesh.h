#pragma once

#include <vector>
#include <glm/glm.hpp>

using namespace std;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};


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