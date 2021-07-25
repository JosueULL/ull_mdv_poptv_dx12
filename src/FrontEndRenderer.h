#pragma once

#include <memory>

class Scene;
class FrameGraph;

class FrontEndRenderer {

private:
	std::unique_ptr<FrameGraph> frameGraph_;

public:
	FrontEndRenderer();
	const FrameGraph* GetSceneFrameGraph(Scene* scene);
};