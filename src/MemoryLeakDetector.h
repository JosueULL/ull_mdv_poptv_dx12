#pragma once

#include "windows.h"
#define _CRTDBG_MAP_ALLOC //to get more details
#include <crtdbg.h>   //for malloc and free

class MemoryLeakDetector 
{
private:
	_CrtMemState sOld;
	_CrtMemState sNew;
	_CrtMemState sDiff;

public:
	
	void Start() {
		_CrtMemCheckpoint(&sOld); //take a snapchot
	}

	void Stop() {
		_CrtMemCheckpoint(&sNew); //take a snapchot 
		if (_CrtMemDifference(&sDiff, &sOld, &sNew)) // if there is a difference
		{
			OutputDebugString(L"-----------_CrtMemDumpStatistics ---------");
			_CrtMemDumpStatistics(&sDiff);
			OutputDebugString(L"-----------_CrtMemDumpAllObjectsSince ---------");
			_CrtMemDumpAllObjectsSince(&sOld);
			OutputDebugString(L"-----------_CrtDumpMemoryLeaks ---------");
			_CrtDumpMemoryLeaks();
			MessageBox(NULL, (LPCWSTR)L"Memory leaks detected. Check the console for more details", (LPCWSTR)L"Memory Leaks Detected!", MB_OK);
		}
	}
};