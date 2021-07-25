#pragma once

#include <memory>
#include "DX12Renderer.h"
#include "MemoryLeakDetector.h"
#include "Singleton.h"

class Scene;
class Keyboard;
class FrontEndRenderer;

class System : public Singleton<System>
{
public:
	System(token);
	~System();
	void Init(HINSTANCE hInstance);
	void Run();

	AMD::Window* GetWindow() { return window_.get(); }
	Keyboard* GetKeyboard() { return keyboard_.get(); }

private:
	void Shutdown();
	void ProcessMessageQueue();

	std::unique_ptr<FrontEndRenderer> frontEndRenderer_;
	std::unique_ptr<AMD::DX12Renderer> backEndRenderer_;
	std::unique_ptr<AMD::Window> window_;
	std::unique_ptr<Scene> scene_;
	std::unique_ptr<Keyboard> keyboard_;

	HINSTANCE hInstance_;
	bool quit_;

	MemoryLeakDetector mLeakDetector_;
};

