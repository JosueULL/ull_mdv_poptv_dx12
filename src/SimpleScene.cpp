#include "SimpleScene.h"

SimpleScene::SimpleScene() : Scene()
{
	AddTexture("tex.checker", "Assets/Textures/checker.jpg");	// Adds a new Texture to SceneResourcesDesc
	Material* mat = AddMaterial("mat.checker", "Assets/Shaders/baseTexture", false, false); // Adds a new Shader & Material to SceneResourcesDesc
	mat->SetTexture(0, "tex.checker"); 
	AddMesh("mesh.cube", "Assets/Meshes/cube.obj"); // Adds a new Mesh to SceneResourcesDesc
	SceneObject* obj = AddObject("myCube");	 // Adds a new CBuffer to SceneResourcesDesc
	MeshRendererComponent* mr = obj->AddComponent<MeshRendererComponent>();
	mr->SetMesh("mesh.cube");
	mr->SetMaterial(mat);
	obj->GetTransform()->SetLocalPosition({ 0,0,5 });
	obj->GetTransform()->SetLocalRotation({ 0,45,0 });
}