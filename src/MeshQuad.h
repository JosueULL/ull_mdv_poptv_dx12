#pragma once

#include "Mesh.h"

class MeshQuad : public Mesh
{
public:
	MeshQuad()
	{
		glm::vec3 n = { 0, 0 , 0};
		this->vertices.push_back({ { -1.0f, 1.0f, 0 }, n,{ 0, 0 } }); // Upper Left
		this->vertices.push_back({ { 1.0f, 1.0f, 0 }, n,{ 1, 0 } }); // Upper Right
		this->vertices.push_back({ { 1.0f, -1.0f, 0 }, n,{ 1, 1 } }); // Bottom right
		this->vertices.push_back({ { -1.0f, -1.0f, 0 }, n,{ 0, 1 } }); // Bottom left

		this->indices = { 0, 1, 2, 2, 3, 0 };
	}
};