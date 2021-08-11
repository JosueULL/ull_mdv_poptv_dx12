#include "SceneResourcesDesc.h"

#include "Mesh.h"
#include "Material.h"
#include "ConstantBufferDef.h"
#include "InstanceBufferDef.h"
#include "Texture.h"
#include "RenderStateDef.h"

void SceneResourcesDesc::ReleaseAll() {
	Release<Mesh>(Meshes);
	Release<Material>(Materials);
	Release<ConstantBufferDef>(ConstantBuffers);
	Release<InstanceBufferDef>(InstanceBuffers);
	Release<Texture>(Textures);
	Release<RenderStateDef>(RenderStates);
}