#pragma once

#include "Component.h"
#include "Material.h"
#include "InstanceBufferDef.h"

#include <string>
#include <memory>



class MeshRendererComponent : public Component {

public:
    
    MeshRendererComponent() = delete;
    MeshRendererComponent(const MeshRendererComponent&) = delete;


    explicit MeshRendererComponent(SceneObject* owner) : Component(owner),
        meshId_(),
        material_(nullptr),
        instanceBufferDef_(nullptr)
    {
    }

    ~MeshRendererComponent()
    {
    }

    void SetMesh(std::string meshId) {
        meshId_ = meshId;
    }
    
    std::string GetMesh() {
        return meshId_;
    }

    void SetMaterial(Material* material) {
        material_ = material;
    }

    Material* GetMaterial() {
        return material_;
    }

    void SetInstanceBuffer(InstanceBufferDef* buffer) {
        instanceBufferDef_ = buffer;
    }

    InstanceBufferDef* GetInstanceBuffer() {
        return instanceBufferDef_;
    }

private:
    std::string meshId_;
    Material* material_;
    InstanceBufferDef* instanceBufferDef_;
};