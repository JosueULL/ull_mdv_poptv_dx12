#include "FrontEndRenderer.h"
#include "FrameGraph.h"
#include "Scene.h"
#include "Camera.h"
#include "MeshRendererComponent.h"
#include "InstancedMeshRendererComponent.h"
#include "SceneObject.h"

FrontEndRenderer::FrontEndRenderer() :
    frameGraph_(std::make_unique<FrameGraph>()) 
{

}

const FrameGraph* FrontEndRenderer::GetSceneFrameGraph(Scene* scene) {
    FrameGraph* fg = frameGraph_.get();
    fg->Clear();
    
    Camera* cam = scene->GetMainCamera();
    FrameGraphCamera fgCam;
    fgCam.CBuffer.RootIndex = 1;
    fgCam.CBuffer.Id = "mainCam";
    fgCam.CBuffer.Data = static_cast<Camera::ConstantBuffer*>(cam->GetCBuffer());
    fg->RenderCam = fgCam;

    
    // Gather all mesh renderers
    std::vector<MeshRendererComponent*> renderers = scene->GetAllComponents<MeshRendererComponent>();
    for (MeshRendererComponent* renderer : renderers) {
        SceneObject* sceneObj = renderer->GetSceneObject();
        FrameGraphMesh fGraphMesh;
        fGraphMesh.Id = renderer->GetMesh();
        fGraphMesh.TextureBinds = renderer->GetMaterial()->TextureBinds;

        InstanceBufferDef* instanceBufferDef = renderer->GetInstanceBuffer();
        if (instanceBufferDef != nullptr) {
            fGraphMesh.CBuffer.RootIndex = 3;
            fGraphMesh.CBuffer.Data = instanceBufferDef->ptr;
            fGraphMesh.CBuffer.Id = instanceBufferDef->id;
            fg->InstancedMeshes.push_back(fGraphMesh);
        }
        else 
        {
            fGraphMesh.CBuffer.RootIndex = 2;
            fGraphMesh.CBuffer.Data = sceneObj->GetCBuffer();
            fGraphMesh.CBuffer.Id = sceneObj->GetId();
            fg->Meshes.push_back(fGraphMesh);
        }
    }

     return fg;
}