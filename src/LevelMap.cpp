#include "LevelMap.h"
#include "Scene.h"
#include "InstancedMeshRendererComponent.h"

#include <fstream>
#include <sstream>
#include <iostream>

LevelMap::LevelMap(Scene* scene, std::string levelPath) 
{
    scene->AddMesh("mesh.level.deadEnd", "Assets/Meshes/deadEnd.obj");
    scene->AddMesh("mesh.level.sideWall", "Assets/Meshes/sideWall.obj");
    scene->AddMesh("mesh.level.corridor", "Assets/Meshes/corridor.obj");
	scene->AddMesh("mesh.level.noWall", "Assets/Meshes/noWall.obj");
    scene->AddMesh("mesh.level.corner", "Assets/Meshes/corner.obj");

    scene->AddTexture("texture.level", "Assets/Textures/rock.jpg");

    Material* mat = scene->AddMaterial("mat.level", "Assets/Shaders/level.hlsl", true);
    mat->SetTexture(0, "texture.level");

	std::ifstream infile(levelPath);
    std::string line;
    while (std::getline(infile, line))
    {
        if (line[0] == '/' && line[1] == '/')
            continue;
        
        if (line.rfind("LEVEL", 0) == 0) {
            std::cout << "FOUNDLEVEL!" << std::endl;
            int w, h;

            // First line expected to be level W,H
            std::getline(infile, line);
            std::istringstream iss(line);
            if (!(iss >> w >> h)) { break; } // error

            int y = 0;
            while (std::getline(infile, line))
            {
                if (line.rfind("END", 0) == 0)
                    break;

                for (int x = 0; x < w; ++x) {
                    char c = line[x];
                    if (c != '#') {
                        pieces_[c].push_back({ x, y });
                    }
                }
                ++y;
            }
        }
        
        /*
        if (line.rfind("ENEMIES", 0) == 0) 
        {
            while (std::getline(infile, line))
            {
                if (line.rfind("END", 0) == 0)
                    break;

                int x, y;
                std::getline(infile, line);
                std::istringstream iss(line);
                if (!(iss >> x >> y)) { break; } // error

                ++y;
            }
        }
        */
    }

    

    for (auto const& x : pieces_)
    {
        int count = (int)x.second.size();
        
        InstancedMeshRendererComponent::InstanceData* instanceBuffer = new InstancedMeshRendererComponent::InstanceData[count];
        int i = 0;
        for (auto const& piece : x.second) {
            int rotation = 0;
            char c = x.first;
            if (c == CORNER_90 || c == CORRIDOR_90 || c == SIDEWALL_90 || c == DEADEND_90)
                rotation = 90;
            else if (c == CORNER_180 || c == SIDEWALL_180 || c == DEADEND_180)
                rotation = 180;
            else if (c == CORNER_270 || c == SIDEWALL_270 || c == DEADEND_270)
                rotation = -90;
            
            glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(piece.y * 2, 0, -piece.x * 2));
            t = glm::rotate(t, glm::radians((float)rotation), glm::vec3(0.0f, 1.0f, 0.0f));
            instanceBuffer[i] = { t };
            ++i;
        }

        std::string bufferId = "instanceBuffer";
        bufferId.push_back(x.first);
        InstanceBufferDef* iBufferDef = scene->AddInstanceBuffer(bufferId, instanceBuffer, count, sizeof(InstancedMeshRendererComponent::InstanceData) * count);

        std::string objId = "instanceObj";
        objId.push_back(x.first);
        SceneObject* so3 = scene->AddObject(objId);
        MeshRendererComponent* imrc = so3->AddComponent<MeshRendererComponent>();
        if (x.first == DEADEND_0 || x.first == DEADEND_90 || x.first == DEADEND_180 || x.first == DEADEND_270)
            imrc->SetMesh("mesh.level.deadEnd");
        else if (x.first == CORRIDOR_0 || x.first == CORRIDOR_90)
            imrc->SetMesh("mesh.level.corridor");
        else if (x.first == CORNER_0 || x.first == CORNER_90 || x.first == CORNER_180 || x.first == CORNER_270)
            imrc->SetMesh("mesh.level.corner");
        else if (x.first == SIDEWALL_0 || x.first == SIDEWALL_90 || x.first == SIDEWALL_180 || x.first == SIDEWALL_270)
            imrc->SetMesh("mesh.level.sideWall");
        else
            imrc->SetMesh("mesh.level.noWall");

        imrc->SetMaterial(mat);
        imrc->SetInstanceBuffer(iBufferDef);
    }
}