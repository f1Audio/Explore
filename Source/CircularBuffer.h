#pragma once
#include <memory>

class CircularBuffer
{
public:

    void init(unsigned int _bufferLength);

    void flush();

    void write(float input);

    float read(int delaySamples);
    
    float read(double delayInFractionalSamples);
    inline double doLinearInterpolation(double y1, double y2, double fractional_X);

private:
    std::unique_ptr<float[]> buffer = nullptr;
    unsigned int writeIndex = 0;
    unsigned int bufferLength = 1024;    // must be nearest power of 2
    unsigned int wrapMask = 1023;        // must be (bufferLength - 1)
};
