#pragma once

class FrameGraph;
class Window;
class SceneResourceDesc;
class SceneResourcesDesc;

class BackEndRenderer
{
public:

	virtual void Render(const FrameGraph* frameGraph) = 0;
	virtual void Initialize(const Window& window) = 0;
	virtual void Shutdown() = 0;
	virtual void LoadResources(const SceneResourcesDesc& sceneRes) = 0;
	virtual ~BackEndRenderer() {};
};