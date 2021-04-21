#pragma once
#include <memory>
#include "bytearray.h"

namespace TinyServer
{

class Stream
{
public:
    virtual ~Stream() { };

    virtual int read(void* buffer, size_t length) = 0;
    virtual int read(Ref<ByteArray> ba, size_t length) = 0;
    virtual int readFixSize(void* buffer, size_t length);
    virtual int readFixSize(Ref<ByteArray> ba, size_t length);
    virtual int write(const void* buffer, size_t length) = 0;
    virtual int write(Ref<ByteArray> ba, size_t length) = 0;
    virtual int writeFixSize(const void* buffer, size_t length);
    virtual int writeFixSize(Ref<ByteArray> ba, size_t length);
    virtual void close() = 0;

private:
};











}