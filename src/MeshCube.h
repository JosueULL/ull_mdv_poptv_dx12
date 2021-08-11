#pragma once

#include "Mesh.h"

class MeshCube : public Mesh
{
public:
	MeshCube()
	{
		float s = 0.5f;

		// Front face
		glm::vec3 n = { 0, 0 ,0 };
		this->vertices.push_back({ { -s, s, -s },	 n,{ 0, 0 } }); // Upper Left		0
		this->vertices.push_back({ { s, s, -s }	,	n,{ 1, 0 } }); // Upper Right	1
		this->vertices.push_back({ { s, -s, -s },	n,{ 1, 1 } }); // Bottom right	2
		this->vertices.push_back({ {-s, -s, -s}	,	n,{0, 1} });		// Bottom left	3

		// Back face
		this->vertices.push_back({ { -s, s, s }	,	n,{ 1, 0 } }); // Upper Left		4
		this->vertices.push_back({ { s, s, s }	,	n,{ 0, 0 } }); // Upper Right	5
		this->vertices.push_back({ { s, -s, s }	,	n,{ 0, 1 } }); // Bottom right	6
		this->vertices.push_back({ { -s, -s, s },	n,{ 1, 1 } }); // Bottom left	7

		// Left face
		this->vertices.push_back({ { -s, -s, -s }	,n,{ 1, 1 } }); // Upper Left		8
		this->vertices.push_back({ { -s, -s, s }	,n,{ 0, 1 } }); // Upper Right	9
		this->vertices.push_back({ { -s, s, s }		,n,{ 0, 0 } }); // Bottom right	10
		this->vertices.push_back({ { -s, s, -s }	,n,{ 1, 0 } }); // Bottom left	11

		// Right face
		this->vertices.push_back({ { s, -s, s }		,n,{ 1, 1 } }); // Upper Left		12
		this->vertices.push_back({ { s, -s, -s }	,n,{ 0, 1 } }); // Upper Right	13
		this->vertices.push_back({ { s, s, -s }		,n,{ 0, 0 } }); // Bottom right	14
		this->vertices.push_back({ { s, s, s }		,n,{ 1, 0 } }); // Bottom left	15

		// Top face
		this->vertices.push_back({ { -s, -s, -s }	,n,{ 0, 0 } }); // Upper Left		16
		this->vertices.push_back({ { s, -s, -s }	,n,{ 1, 0 } }); // Upper Right	17
		this->vertices.push_back({ { s, -s, s }		,n,{ 1, 1 } }); // Bottom right	18
		this->vertices.push_back({ { -s, -s, s }	,n,{ 0, 1 } }); // Bottom left	19

		// Bottom face
		this->vertices.push_back({ { -s, s, s }		,n,{ 0, 0 } }); // Upper Left		120
		this->vertices.push_back({ { s, s, s }		,n,{ 1, 0 } }); // Upper Right	21
		this->vertices.push_back({ { s, s, -s }		,n,{ 1, 1 } }); // Bottom right	22
		this->vertices.push_back({ { -s, s, -s }	,n,{ 0, 1 } }); // Bottom left	23

		this->indices = { 
			0, 1, 2, // front face
			2, 3, 0,

			6, 5, 4, // back face
			4, 7, 6,

			8, 9, 10, // left face
			10, 11, 8,

			12, 13, 14, // right face
			14, 15, 12,

			16, 17, 18, // top face
			18, 19, 16,

			20, 21, 22, // bottom face
			22, 23, 20,
		};
	}

	~MeshCube() {
		this->vertices.clear();
	}
};