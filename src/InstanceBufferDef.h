#pragma once
#include <string>

struct InstanceBufferDef {
    std::string id;
    int instanceCount;
    int size;
    void* ptr;
};
