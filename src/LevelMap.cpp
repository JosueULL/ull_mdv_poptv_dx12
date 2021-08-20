#include "System.h"
#include "SystemTime.h"
#include "LevelMap.h"
#include "Scene.h"
#include "InstancedMeshRendererComponent.h"
#include "MathUtil.h"
#include "glm/gtx/vector_angle.hpp"
#include <float.h>
unsigned int fp_control_state = _controlfp(_EM_INEXACT, _MCW_EM);

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdlib.h>

LevelMap::LevelMap(Scene* scene, std::string levelPath) :
    start_({0,0,0}),
    startRot_(0),
    exit_({0,0}),
    exitRot_(0),
    width_(0),
    height_(0),
    exitUnlocked_(false)
{
    BIND_OBSERVER(OnPickupOverlap, LevelTile);

    ParseLevelFile(levelPath);
    InstantiateLevelContent(scene);
}

void LevelMap::ParseLevelFile(std::string levelPath) 
{
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

            width_ = w;
            height_ = h;

            int y = 0;
            while (std::getline(infile, line))
            {
                if (line.rfind("END", 0) == 0)
                    break;

                for (int x = 0; x < w; ++x) {
                    char c = line[x];
                    if (c != NOTHING) {
                        pieces_[c].push_back({ x, y });
                        walkable_.push_back({ x, y });
                    }
                }
                ++y;
            }
        }

        if (line.rfind("START", 0) == 0) {
            int x, y, rot;

            // First line expected to be level X,Y,ROTATION
            std::getline(infile, line);
            std::istringstream iss(line);
            if (!(iss >> x >> y >> rot)) { break; } // error

            start_ = LevelToWorld(x, y);
            startRot_ = rot;

            while (std::getline(infile, line))
                if (line.rfind("END", 0) == 0)
                    break;
        }

        if (line.rfind("EXIT", 0) == 0) {
            int x, y, rot;

            // First line expected to be level X,Y,ROTATION
            std::getline(infile, line);
            std::istringstream iss(line);
            if (!(iss >> x >> y >> rot)) { break; } // error

            exit_ = { x, y };
            exitRot_ = rot;

            while (std::getline(infile, line))
                if (line.rfind("END", 0) == 0)
                    break;
        }

        if (line.rfind("PICKUP", 0) == 0) {

            while (std::getline(infile, line))
            {
                if (line.rfind("END", 0) == 0)
                    break;

                int x, y;

                // First line expected to be level X,Y,ROTATION
                std::istringstream iss(line);
                if (!(iss >> x >> y)) { break; } // error

                pickups_.push_back({ x, y });
            }
        }


        if (line.rfind("ENEMYPATH", 0) == 0)
        {
            std::vector<LevelTile> enemyPath;
            while (std::getline(infile, line))
            {
                if (line.rfind("END", 0) == 0)
                    break;

                int x, y;
                std::istringstream iss(line);
                if (!(iss >> x >> y)) { break; } // error
                enemyPath.push_back({x, y});
            }

            if (enemyPath.size() > 1) {
                glm::vec3 pos1 = LevelToWorld(enemyPath[0]);
                glm::vec3 pos2 = LevelToWorld(enemyPath[1]);
                float initAngle = glm::orientedAngle({ 0, 0, 1 }, glm::normalize(pos2 - pos1), { 0,1,0 });
                enemies_.push_back({ LevelToWorld(enemyPath[0]), 0, 1, enemyPath, initAngle });
            }
        }
    }
}

void LevelMap::InstantiateLevelContent(Scene* scene) {

    // Pickups ----------------------------------
    if (pickups_.size() > 0) {
        scene->AddMesh("mesh.level.key", "Assets/Meshes/key.obj");
        scene->AddTexture("texture.key", "Assets/Textures/key.jpg");

        Material* matkey = scene->AddMaterial("mat.key", "Assets/Shaders/key.hlsl", true);
        matkey->SetTexture(0, "texture.key");

        int pickupCount = (int)pickups_.size();
        pickupsBuffer_ = new InstancedMeshRendererComponent::InstanceData[pickupCount];
        UpdatePickups();
        InstanceBufferDef* pickupBufferDef = scene->AddInstanceBuffer("level.pickupInstanceBuffer", pickupsBuffer_, pickupCount, sizeof(InstancedMeshRendererComponent::InstanceData) * pickupCount);

        SceneObject* pickupsObj = scene->AddObject("level.pickups");
        MeshRendererComponent* mrc = pickupsObj->AddComponent<MeshRendererComponent>();
        mrc->SetMesh("mesh.level.key");
        mrc->SetMaterial(matkey);
        mrc->SetInstanceBuffer(pickupBufferDef);
    }

    // Enemies ----------------------------------
    if (enemies_.size() > 0) {
        scene->AddMesh("mesh.level.enemy", "Assets/Meshes/skull.obj");
        scene->AddTexture("texture.noise", "Assets/Textures/noise.jpg");

        Material* matEnemy = scene->AddMaterial("mat.enemy", "Assets/Shaders/skull.hlsl", true);
        matEnemy->SetTexture(0, "texture.noise");

        int enemiesCount = (int)enemies_.size();
        enemiesBuffer_ = new InstancedMeshRendererComponent::InstanceData[enemiesCount];
        UpdateEnemies();
        InstanceBufferDef* enemiesBufferDef = scene->AddInstanceBuffer("level.enemiesInstanceBuffer", enemiesBuffer_, enemiesCount, sizeof(InstancedMeshRendererComponent::InstanceData) * enemiesCount);

        SceneObject* enemyObj = scene->AddObject("level.enemies");
        MeshRendererComponent* mrcEnemy = enemyObj->AddComponent<MeshRendererComponent>();
        mrcEnemy->SetMesh("mesh.level.enemy");
        mrcEnemy->SetMaterial(matEnemy);
        mrcEnemy->SetInstanceBuffer(enemiesBufferDef);
    }

    // Exit ------------------------------------
    scene->AddMesh("mesh.level.exit", "Assets/Meshes/closedPortal.obj");
    scene->AddMesh("mesh.level.exit.open", "Assets/Meshes/portal.obj");

    Material* exitMat = scene->AddMaterial("mat.level.exit", "Assets/Shaders/portal.hlsl", false);
    exitMat->SetTexture(0, "texture.noise");

    SceneObject* exit = scene->AddObject("level.exit");
    exitMR_ = exit->AddComponent<MeshRendererComponent>();
    exitMR_->SetMesh("mesh.level.exit");
    exitMR_->SetMaterial(exitMat);
    exitMR_->GetTransform()->SetLocalPosition(LevelToWorld(exit_));
    exitMR_->GetTransform()->SetLocalRotation({ 0, exitRot_, 0 });

    // Level tiles ------------------------------------
    scene->AddMesh("mesh.level.deadEnd", "Assets/Meshes/deadEnd.obj");
    scene->AddMesh("mesh.level.sideWall", "Assets/Meshes/sideWall.obj");
    scene->AddMesh("mesh.level.corridor", "Assets/Meshes/corridor.obj");
    scene->AddMesh("mesh.level.noWall", "Assets/Meshes/noWall.obj");
    scene->AddMesh("mesh.level.corner", "Assets/Meshes/corner.obj");

    scene->AddTexture("texture.level", "Assets/Textures/rock.jpg");

    Material* mat = scene->AddMaterial("mat.level", "Assets/Shaders/level.hlsl", true);
    mat->SetTexture(0, "texture.level");

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
                rotation = 270;
            
            glm::mat4 t = glm::translate(glm::mat4(1.0f), LevelToWorld(piece));
            t = glm::rotate(t, -glm::radians((float)rotation), glm::vec3(0.0f, 1.0f, 0.0f));
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

        buffers_[x.first] = instanceBuffer;
    }

    // Random Vegetation -------------------------------------------------
    scene->AddMesh("mesh.level.weed", "Assets/Meshes/weed.obj");
    Material* matVeg = scene->AddMaterial("mat.level", "Assets/Shaders/level.hlsl", true);
    scene->AddTexture("texture.level.vegetation", "Assets/Textures/vegetation.png");
    matVeg->SetTexture(0, "texture.level.vegetation");

    int count = (int)(walkable_.size() * 0.5f);
    InstancedMeshRendererComponent::InstanceData* instanceBuffer = new InstancedMeshRendererComponent::InstanceData[count];
    InstanceBufferDef* iBufferDef = scene->AddInstanceBuffer("instanceBufferWeed", instanceBuffer, count, sizeof(InstancedMeshRendererComponent::InstanceData) * count);
    while(count>0) {
        int randomIndex = rand() % walkable_.size();
        LevelTile tile = walkable_[randomIndex];
        glm::mat4 t = glm::translate(glm::mat4(1.0f), LevelToWorld(tile));
        float rotation = (float)(90 * (rand() % 4));
        t = glm::rotate(t, -glm::radians((float)rotation), glm::vec3(0.0f, 1.0f, 0.0f));
        instanceBuffer[count-1] = { t };
        --count;
    }
    
    SceneObject* so3 = scene->AddObject("instanceObjWeed");
    MeshRendererComponent* imrc = so3->AddComponent<MeshRendererComponent>();
    imrc->SetMesh("mesh.level.weed");
    imrc->SetMaterial(matVeg);
    imrc->SetInstanceBuffer(iBufferDef);
}

void LevelMap::Update() {
    UpdateEnemies();
}

void LevelMap::UpdatePickups()
{
    int index = 0;
    for (auto const& p : pickups_)
    {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), LevelToWorld(p));
        pickupsBuffer_[index] = { t };
        ++index;
    }
}

void LevelMap::UpdateEnemies()
{
    float deltaTime = (float)System::instance().GetTime()->GetDeltaTime();
    int index = 0;
    for (auto& e : enemies_)
    {
        glm::vec3 toPos = LevelToWorld(e.path[e.toIndex]);
        glm::vec3 fromPos = LevelToWorld(e.path[e.fromIndex]);
        float dot = glm::dot(toPos - e.pos, toPos - fromPos);
        if (dot < 0) {
            e.pos = toPos;
            e.fromIndex = e.toIndex;
            ++e.toIndex;
            if (e.toIndex >= e.path.size())
                e.toIndex = 0;
            toPos = LevelToWorld(e.path[e.toIndex]);
        }

        glm::mat4 t = glm::translate(glm::mat4(1.0f), e.pos);

        glm::vec3 disp = toPos - e.pos;
        if (glm::length(disp) > 0) 
        {
            glm::vec3 dir = glm::normalize(disp);
            const float speed = 2;
            e.pos = e.pos + dir * speed * deltaTime;

            float angle = glm::orientedAngle({ 0, 0, 1 }, dir, { 0,1,0 });
            
            e.yaw = MathUtil::lerp(e.yaw, angle, deltaTime * 5);
            t = glm::rotate(t, e.yaw, { 0,1,0 });
        }
        
        enemiesBuffer_[index] = { t };

        ++index;
    }
}

void LevelMap::RegisterCollisionCallbacks(LevelCollisionComponent* col) {
    col->OnPickupOverlap->Attach(OnPickupOverlap);
}

void LevelMap::OnPickupOverlapCallback(LevelTile tile) {
    for (auto& p : pickups_) {
        if (p.x == tile.x && p.y == tile.y) {
            // HACK - Quick way of making the pickup disappear
            p.x = 10000;
            p.y = 10000;
            break;
        }
    }

    UpdatePickups();
}

void LevelMap::UnlockExit()
{
    exitMR_->SetMesh("mesh.level.exit.open");
    exitUnlocked_ = true;
}

LevelMap::~LevelMap() {
    for (auto const& b : buffers_)
        delete[] b.second;

    buffers_.clear();

    delete pickupsBuffer_;
    delete enemiesBuffer_;

    // TODO - Unregister from LevelCollisionComponent
    FREE_OBSERVER(OnPickupOverlap);

}