#pragma once

#include <memory>
#include <string>
#include "Singleton.h"
#include "MemoryLeakDetector.h"

class Scene;
class Window;
class Keyboard;
class BackEndRenderer;
class FrontEndRenderer;
class SystemTime;

class System : public Singleton<System>
{
public:
	System(token);
	~System();
	void Init(HINSTANCE hInstance, std::string name, int w, int h);
	void LoadScene(Scene* startScene);
	void Run();

	Window* GetWindow() const { return window_.get(); }
	Keyboard* GetKeyboard() const { return keyboard_.get(); }
	SystemTime* GetTime() const { return time_.get(); }

private:
	void Shutdown();
	void ProcessMessageQueue();

	std::unique_ptr<FrontEndRenderer> frontEndRenderer_;
	std::unique_ptr<BackEndRenderer> backEndRenderer_;
	std::unique_ptr<Window> window_;
	std::unique_ptr<Scene> scene_;
	std::unique_ptr<Keyboard> keyboard_;
	std::unique_ptr<SystemTime> time_;

	HINSTANCE hInstance_;
	bool quit_;

	MemoryLeakDetector mLeakDetector_;
};

