#include "CircularBuffer.h"

void CircularBuffer::init(unsigned int _bufferLength) {
    // set write index to 0
    writeIndex = 0;
    // find nearest power of 2 for buffer length, thus we will be able to use wrap mask for wrapping
    bufferLength = pow(2, ceil(log(_bufferLength) / log(2)));
    // set wrap mask as (bufferLength - 1) for wrapping
    wrapMask = bufferLength - 1;
    // create new buffer
    buffer.reset(new float[bufferLength]);
    // flush buffer
    flush();
}


inline double CircularBuffer::doLinearInterpolation(double y1, double y2, double fractional_X)
{
    // --- check invalid condition
    if (fractional_X >= 1.0) return y2;

    // --- use weighted sum method of interpolating
    return fractional_X*y2 + (1.0 - fractional_X)*y1;
}
void CircularBuffer::flush() {
    memset(&buffer[0], 0, bufferLength * sizeof(float));
}

float CircularBuffer::read(int delaySamples) {
    // calculate read index as offset from write index
    int readIndex = (writeIndex - 1) - delaySamples;
    // check and wrap backwards if the index is negative
    readIndex &= wrapMask;

    return buffer[readIndex];
}

float CircularBuffer::read(double delayInFractionalSamples)
{
    // --- truncate delayInFractionalSamples and read the int part
    float y1 = read((int)delayInFractionalSamples);

    

    // --- else do interpolation
    //
    // --- read the sample at n+1 (one sample OLDER)
    float y2 = read((int)delayInFractionalSamples + 1);

    // --- get fractional part
    double fraction = delayInFractionalSamples - (int)delayInFractionalSamples;

    // --- do the interpolation (you could try different types here)
    return doLinearInterpolation(y1, y2, fraction);
}

void CircularBuffer::write(float input) {
    // write and increment index
    buffer[writeIndex++] = input;
    // wrap write index if exceeding buffer length
    writeIndex &= wrapMask;
}
