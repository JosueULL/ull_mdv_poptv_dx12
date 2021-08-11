#pragma once

#define SAFE_FREE(ptr) if (ptr!=nullptr) { delete ptr; ptr = nullptr; }
#define SAFE_FREE_ARRAY(ptr) if (ptr!=nullptr) { delete[] ptr; ptr = nullptr; }