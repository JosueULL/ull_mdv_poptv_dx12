#pragma once

#include <vector>

using namespace std;

struct Vertex
{
	float position[3];
	float uv[2];
};


class Mesh 
{
public:
	vector<Vertex> vertices;
	vector<int> indices;
};