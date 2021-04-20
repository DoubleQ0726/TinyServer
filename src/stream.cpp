#include "stream.h"

namespace TinyServer
{
int Stream::readFixSize(void* buffer, size_t length)
{
    size_t offset = 0;
    size_t left = length;
    while (left > 0)
    {
        int len = read((char*)buffer + offset, left);
        if (len <= 0)
            return len;
        offset += len;
        left -= len;
    }
    return length;
}

int Stream::readFixSize(Ref<ByteArray> ba, size_t length)
{
    size_t left = length;
    while (left > 0)
    {
        int len = read(ba, left);
        if (len <= 0)
            return len;
        left -= len;
    }
    return length;
}

int Stream::writeFixSize(const void* buffer, size_t length)
{
    size_t offset = 0;
    size_t left = length;
    while (left > 0)
    {
        int len = write((const char*)buffer + offset, left);
        if (len <= 0)
            return len;
        offset += len;
        left -= len;
    }
    return length;
}

int Stream::writeFixSize(Ref<ByteArray> ba, size_t length)
{
    size_t left = length;
    while (left > 0)
    {
        int len = write(ba, left);
        if (len <= 0)
            return len;
        left -= len;
    }
    return length;
}











}