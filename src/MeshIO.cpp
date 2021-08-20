#include "Mesh.h"
#include "MeshIO.h"
#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"
#include <iostream>
#include <unordered_map>


Mesh* LoadObjMeshFromFile(std::string inputfile) 
{
	Mesh* mesh = new Mesh();
	/*
	tinyobj::ObjReaderConfig reader_config;
	reader_config.triangulate = true;
	reader_config.mtl_search_path = "./"; // Path to material files

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(inputfile, reader_config)) {
		if (!reader.Error().empty()) {
			std::cerr << "TinyObjReader: " << reader.Error();
		}

		std::wstring inputfileName(inputfile.begin(), inputfile.end());
		std::wostringstream message;
		message << L"Object File" << inputfileName << "not found or wrong format" << std::endl;

		throw("File not found");
	}


	if (!reader.Warning().empty()) {
		std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();
	//auto& materials = reader.GetMaterials(); Materials will be programmed in the game

	// We don't loop over shapes, we are asuming one shape per file
	size_t s = 0;
	// Loop over faces(polygon) to get indexes
	size_t index_offset = 0;
	unsigned int countIndex = 0;
	std::map<unsigned int, tinyobj::index_t> mapTOVITOGI; // Map connecting TinyObject vertex index to TinyObject general index
	std::map<unsigned int, unsigned int>  mapTOVIIndex; // Map connecting Tiny Object Vertex index to out mesh vertices index
	// This code requires these mapping to avoid assumming consecutive vertex indexes in the obj file
	for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
		int fv = shapes[s].mesh.num_face_vertices[f];
		for (size_t v = 0; v < fv; v++) {
			unsigned int index;
			tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
			index = static_cast<unsigned int>(idx.vertex_index);

			mapTOVITOGI[index] = idx;
			if (mapTOVIIndex.find(index) == mapTOVIIndex.end()) { // if this Tinyobject vertex index has not been processed
				mapTOVIIndex[index] = countIndex++;
			}
			mesh->indices.push_back(mapTOVIIndex[index]);
		}
		index_offset += fv;
	}
	// Prepare vertice vector to allocate enough elements.
	mesh->vertices.resize(mapTOVIIndex.size());
	for (auto it = mapTOVIIndex.begin(); it != mapTOVIIndex.end(); it++) {
		unsigned int vid = it->first;
		unsigned int index = it->second;
		auto fresult = mapTOVITOGI.find(vid);
		if (fresult == mapTOVITOGI.end()) // The vertex index is not in the map
			break;
		// If everything is ok, we get the general index in tinyobj representation
		tinyobj::index_t idx = fresult->second;

		// With this general index we can compose the vertex structure
		Vertex vertex;

		vertex.position.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
		vertex.position.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
		vertex.position.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];
		vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
		vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
		vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
		
		vertex.uv.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
		vertex.uv.y = 1 - attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
		
		mesh->vertices[index] = vertex; //Copy vertex

	}
	*/

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			vertex.uv = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(mesh->vertices.size());
				mesh->vertices.push_back(vertex);
			}

			mesh->indices.push_back(uniqueVertices[vertex]);
		}
	}

	return mesh;
}

