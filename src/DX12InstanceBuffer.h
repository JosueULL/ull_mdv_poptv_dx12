#pragma once

#include "DX12Buffer.h"

struct DX12InstanceBuffer
{
    DX12Buffer buffer;
    int instanceCount;
};