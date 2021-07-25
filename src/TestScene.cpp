#include "TestScene.h"

#include "System.h"
#include "Keyboard.h"
#include "Material.h"
#include "SceneObject.h"
#include "MeshRendererComponent.h"
#include <Windows.h>

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

	so2 = AddObject("obj2");
	so2->GetTransform()->SetLocalScale(glm::vec3(0.5f));
	mrc = so2->AddComponent<MeshRendererComponent>();
	mrc->SetMesh(BuiltInRes::Mesh::Cube);
	mrc->SetMaterial(mat2);

	AddCamera("mainCam");
}

void TestScene::Update()
{
	// TEST ----
	static int counter = 0;
	counter++;
	float scale = std::sin(static_cast<float> (counter) / 128.0f);
	//so1->GetTransform()->SetLocalScale(glm::vec3(scale));
	so1->GetTransform()->SetLocalPosition(glm::vec3(scale, 0, 1));
	so1->GetTransform()->SetLocalRotation(glm::vec3(scale * 360.0f, scale * 360.0f, scale * 360.0f));
	//so1->GetTransform()->SetLocalRotation(glm::vec3(-scale * 90.0f, 0, 0));

	//so2->GetTransform()->SetLocalScale(glm::vec3(scale));

	static float x = 0;
	
	so2->GetTransform()->SetLocalPosition(glm::vec3(x, 0, 1));
	so2->GetTransform()->SetLocalRotation(glm::vec3(0, scale * 360.0f, 0));
	//so2->GetTransform()->SetLocalRotation(glm::vec3(0, 0, -scale * 180.0f));
	// END TEST ----


	Scene::Update();
}
