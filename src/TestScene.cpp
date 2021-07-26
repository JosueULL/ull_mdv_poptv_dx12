#include "TestScene.h"

#include "System.h"
#include "Keyboard.h"
#include "Material.h"
#include "SceneObject.h"
#include "MeshRendererComponent.h"
#include "InstancedMeshRendererComponent.h"
#include "FPCameraCtrlComponent.h"

TestScene::TestScene() : Scene()
{
	// Resources --------------------------------
	AddTexture("doom", "Assets/Textures/doom.jpg");
	AddTexture("fire", "Assets/Textures/fire.jpg");
	AddTexture("checker", "Assets/Textures/checker.jpg");

	// Materials
	Material* mat1 = new Material();
	mat1->SetTexture(0, "doom");

	Material* mat2 = new Material();
	mat2->SetTexture(0, "checker");

	// Objects --------------------------------
	so1 = AddObject("obj1");
	MeshRendererComponent* mrc = so1->AddComponent<MeshRendererComponent>();
	mrc->SetMesh(BuiltInRes::Mesh::Cube);
	mrc->SetMaterial(mat1);
	so1->GetTransform()->SetLocalScale(glm::vec3(0.25f));

	so2 = AddObject("obj2");
	so2->GetTransform()->SetLocalScale(glm::vec3(0.5f));
	mrc = so2->AddComponent<MeshRendererComponent>();
	mrc->SetMesh(BuiltInRes::Mesh::Cube);
	mrc->SetMaterial(mat2);


	// ------------------------- Instantiated mesh
	struct InstanceBuffer {
		InstancedMeshRendererComponent::InstanceData instanceData[3];
	};

	InstanceBuffer* iBuffer = new InstanceBuffer();
	iBuffer->instanceData[0] = { glm::translate(glm::mat4(1.0f), glm::vec3(1.0f)) };
	iBuffer->instanceData[1] = { glm::translate(glm::mat4(1.0f), glm::vec3(2.0f)) };
	iBuffer->instanceData[2] = { glm::translate(glm::mat4(1.0f), glm::vec3(3.0f)) };
	
	InstanceBufferDef* iBufferDef =AddInstanceBuffer("instanceBuffer", iBuffer, 3, sizeof(InstanceBuffer));

	SceneObject* so3 = AddObject("inst1");
	MeshRendererComponent* imrc = so3->AddComponent<MeshRendererComponent>();
	imrc->SetMesh(BuiltInRes::Mesh::Cube);
	imrc->SetMaterial(mat2);
	imrc->SetInstanceBuffer(iBufferDef);

	// ------------------------ End 
	
	Camera* cam = AddCamera("mainCam");
	cam->GetSceneObject()->AddComponent<FPCameraCtrlComponent>();
}

void TestScene::Update()
{
	// TEST ----
	static int counter = 0;
	counter++;
	float scale = std::sin(static_cast<float> (counter) / 128.0f);
	//so1->GetTransform()->SetLocalScale(glm::vec3(scale));
	//so1->GetTransform()->SetLocalPosition(glm::vec3(scale, 0, 1));
	//so1->GetTransform()->SetLocalRotation(glm::vec3(scale * 360.0f, scale * 360.0f, scale * 360.0f));
	//so1->GetTransform()->SetLocalRotation(glm::vec3(-scale * 90.0f, 0, 0));

	//so2->GetTransform()->SetLocalScale(glm::vec3(scale));

	static float x = 0;
	
	so2->GetTransform()->SetLocalPosition(glm::vec3(x, 0, 1));
	so2->GetTransform()->SetLocalRotation(glm::vec3(0, scale * 360.0f, 0));
	//so2->GetTransform()->SetLocalRotation(glm::vec3(0, 0, -scale * 180.0f));
	// END TEST ----


	Scene::Update();
}
