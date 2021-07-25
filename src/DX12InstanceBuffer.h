#pragma once

class DX12InstanceBuffer
{

public:
    static DX12InstanceBuffer Create()
    {
        /*
        // Create our trees instance buffer
        // Pretty much the same thing as a regular vertex buffer, except that this buffers data
        // will be used per "instance" instead of per "vertex". Each instance of the geometry
        // gets it's own instanceData data, similar to how each vertex of the geometry gets its own
        // Vertex data
        D3D11_BUFFER_DESC instBuffDesc;
        ZeroMemory(&instBuffDesc, sizeof(instBuffDesc));

        instBuffDesc.Usage = D3D11_USAGE_DEFAULT;
        instBuffDesc.ByteWidth = sizeof(InstanceData) * numTrees;
        instBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        instBuffDesc.CPUAccessFlags = 0;
        instBuffDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA instData;
        ZeroMemory(&instData, sizeof(instData));

        instData.pSysMem = &inst[0];
        hr = d3d11Device->CreateBuffer(&instBuffDesc, &instData, &treeInstanceBuff);

        // The tree's world matrix (We will keep it an identity matrix, but we could change their positions without
        // unrealistic effects, since remember that all transformations are done around the point (0,0,0), and we will
        // be applying this world matrix to our trees AFTER they have been individually positioned depending on the
        // instance buffer, which means they will not be centered at the point (0,0,0))
        treeWorld = XMMatrixIdentity();
        */
    }
};