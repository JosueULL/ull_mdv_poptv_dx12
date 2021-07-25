#pragma once

#include <memory>
#include "Singleton.h"
#include "MemoryLeakDetector.h"

class Scene;
class Window;
class Keyboard;
class DX12Renderer;
class FrontEndRenderer;

class System : public Singleton<System>
{
public:
	System(token);
	~System();
	void Init(HINSTANCE hInstance);
	void Run();

	Window* GetWindow() { return window_.get(); }
	Keyboard* GetKeyboard() { return keyboard_.get(); }

private:
	void Shutdown();
	void ProcessMessageQueue();

	std::unique_ptr<FrontEndRenderer> frontEndRenderer_;
	std::unique_ptr<DX12Renderer> backEndRenderer_;
	std::unique_ptr<Window> window_;
	std::unique_ptr<Scene> scene_;
	std::unique_ptr<Keyboard> keyboard_;

	HINSTANCE hInstance_;
	bool quit_;

	MemoryLeakDetector mLeakDetector_;
};

