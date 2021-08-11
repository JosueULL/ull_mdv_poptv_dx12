#pragma once

#include <windows.h>

class SystemTime
{
public:
    SystemTime()
    {
        LARGE_INTEGER li;
        QueryPerformanceFrequency(&li);

        // seconds
        //timerFrequency_ = double(li.QuadPart);

        // milliseconds
        timerFrequency_ = double(li.QuadPart) / 1000.0;

        // microseconds
        //timerFrequency = double(li.QuadPart) / 1000000.0;

        QueryPerformanceCounter(&li);
        lastFrameTime_ = li.QuadPart;
    }

    double UpdateFrameDelta()
    {
        LARGE_INTEGER li;
        QueryPerformanceCounter(&li);
        frameDelta_ = double(li.QuadPart - lastFrameTime_) / timerFrequency_;
        elapsedTime_ += frameDelta_ / 1000;
        if (frameDelta_ > 0)
            fps_ = (int)(1000 / frameDelta_);
        lastFrameTime_ = li.QuadPart;
        return frameDelta_;
    }

    double GetElapsedTime() const { return elapsedTime_; }

private:
    double timerFrequency_ = 0.0;
    long long lastFrameTime_ = 0;
    long long lastSecond_ = 0;
    double frameDelta_ = 0;
    double elapsedTime_ = 0;
    int fps_ = 0;

};
