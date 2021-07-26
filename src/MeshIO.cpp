#include "Mesh.h"
#include "MeshIO.h"
#define TINYOBJLOADER_IMPLEMENTATION 
#include "tiny_obj_loader.h"
#include <iostream>


Mesh* LoadObjMeshFromFile(std::string inputfile) 
{
	Mesh* mesh = new Mesh();

	tinyobj::ObjReaderConfig reader_config;
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

		vertex.position.x = attrib.vertices[3 * idx.vertex_index + 0];
		vertex.position.y = attrib.vertices[3 * idx.vertex_index + 1];
		vertex.position.z = attrib.vertices[3 * idx.vertex_index + 2];
		vertex.normal.x = attrib.normals[3 * idx.normal_index + 0];
		vertex.normal.y = attrib.normals[3 * idx.normal_index + 1];
		vertex.normal.z = attrib.normals[3 * idx.normal_index + 2];
		vertex.uv.x = attrib.texcoords[2 * idx.texcoord_index + 0];
		vertex.uv.y = attrib.texcoords[2 * idx.texcoord_index + 1];
		//vertex.col = defaultColor;
		//vertex.material = XMUINT3(0, 0, 0);

		mesh->vertices[index] = vertex; //Copy vertex

	}

	return mesh;
}