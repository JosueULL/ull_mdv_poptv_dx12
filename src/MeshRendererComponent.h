#pragma once

#include "Component.h"
#include "Material.h"

#include <string>
#include <memory>

class MeshRendererComponent : public Component {
private:
	std::string meshId_;
    std::shared_ptr<Material> material_;

public:

    MeshRendererComponent() = delete;
    MeshRendererComponent(const MeshRendererComponent&) = delete;


    explicit MeshRendererComponent(SceneObject* owner) : Component(owner),
        meshId_()
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
        material_.reset(material);
    }

    Material* GetMaterial() {
        return material_.get();
    }
};