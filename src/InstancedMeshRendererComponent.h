#pragma once

#include "Component.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "Material.h"

class InstancedMeshRendererComponent : public Component
{

public:
	struct InstanceData {
		glm::mat4 transform;
	};

	COMPONENT_BASE(InstancedMeshRendererComponent)

	void SetInstanceData(const std::vector<InstanceData>& data) {
		data_ = data;
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

private:
    std::string meshId_;
    std::shared_ptr<Material> material_;
	std::vector<InstanceData> data_;
};