#include "SystemTime.h"
#include "System.h"
#include "Window.h"
#include "Keyboard.h"
#include "FrameGraph.h"
#include "TestScene.h"
#include "DX12Renderer.h"
#include "FrontEndRenderer.h"

#define APPNAME "Programación Optimizada : Proyecto final"
#define SCREENW 1280
#define SCREENH 720

System::System(token) :
	frontEndRenderer_(std::make_unique<FrontEndRenderer>()),
	backEndRenderer_(nullptr),
	window_(nullptr),
	scene_(nullptr),
	keyboard_(std::make_unique<Keyboard>()),
	time_(std::make_unique<SystemTime>()),
	hInstance_(),
	quit_(false)
{
#if DEBUG
	mLeakDetector_.Start();
#endif
}

System::~System() 
{
#if DEBUG
	mLeakDetector_.Stop();
#endif
}

void System::Init(HINSTANCE hInstance)
{
	hInstance_ = hInstance;
	window_.reset(new Window(hInstance_, APPNAME, SCREENW, SCREENH));

	backEndRenderer_.reset(new DX12Renderer);
	backEndRenderer_->Initialize(*window_);

	TestScene* scene = new TestScene();
	scene_.reset(scene);

	backEndRenderer_->LoadResources(scene_->GetResourcesDesc());
}

void System::Run() 
{
	while (!quit_) 
	{
		time_->UpdateFrameDelta();

		ProcessMessageQueue();
		keyboard_.get()->Update();
		scene_.get()->Update();

		const FrameGraph* fGraph = frontEndRenderer_.get()->GetSceneFrameGraph(scene_.get());
		
		backEndRenderer_->Render(fGraph);
	}

	Shutdown();
}

void System::ProcessMessageQueue()
{
	MSG message;
	ZeroMemory(&message, sizeof(message));
	if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		switch (message.message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_QUIT:
			quit_ = true;
			break;

		//default:
			//mouse_->Update(message);
		}

		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

void System::Shutdown()
{
	backEndRenderer_->Shutdown();
	
	backEndRenderer_.reset();
	frontEndRenderer_.reset();
	window_.reset();
	scene_.reset();
	keyboard_.reset();
	time_.reset();
}