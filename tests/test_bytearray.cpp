#include "TinyServer.h"
#include "bytearray.h"
using namespace TinyServer;
static Ref<Logger> logger = TINY_LOG_ROOT;
void test()
{
#define XX(type, len, write_fun, read_fun, base_len) {\
    Ref<ByteArray> ba(new ByteArray(base_len)); \
    std::vector<type> vec; \
    for (int i = 0; i < len; ++i) \
    { \
        vec.push_back(rand()); \
    } \
    for (auto& item : vec) \
    { \
        ba->write_fun(item); \
    } \
    ba->setPosition(0); \
    for (size_t i = 0; i < vec.size(); ++i) \
    { \
        type v = ba->read_fun(); \
        TINY_ASSERT(v == vec[i]); \
    } \
    TINY_ASSERT(ba->getReadSize() == 0); \
    TINY_LOG_INFO(logger) << #write_fun "/" #read_fun " (" #type ") len = " \
        << len << " base_len = " << base_len << " size = " << ba->getSize(); \
}
    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t, 100, writeFint16, readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t, 100, writeFint32, readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t, 100, writeFint64, readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t, 100, writeInt32, readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t, 100, writeInt64, readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);

#undef XX

#define XX(type, len, write_fun, read_fun, base_len) {\
    Ref<ByteArray> ba(new ByteArray(base_len)); \
    std::vector<type> vec; \
    for (int i = 0; i < len; ++i) \
    { \
        vec.push_back(rand()); \
    } \
    for (auto& item : vec) \
    { \
        ba->write_fun(item); \
    } \
    ba->setPosition(0); \
    for (size_t i = 0; i < vec.size(); ++i) \
    { \
        type v = ba->read_fun(); \
        TINY_ASSERT(v == vec[i]); \
    } \
    TINY_ASSERT(ba->getReadSize() == 0); \
    TINY_LOG_INFO(logger) << #write_fun "/" #read_fun " (" #type ") len = " \
        << len << " base_len = " << base_len << " size = " << ba->getSize(); \
    ba->setPosition(0); \
    TINY_ASSERT(ba->writeToFile("/tmp/" #type "_" #len "-" #read_fun ".dat")) \
    Ref<ByteArray> ba2(new ByteArray(base_len * 2)); \
    TINY_ASSERT(ba2->readFromFile("/tmp/" #type "_" #len "-" #read_fun ".dat")) \
    ba2->setPosition(0); \
    TINY_ASSERT(ba->toString() == ba2->toString()); \
    TINY_ASSERT(ba->getPosition() == 0); \
    TINY_ASSERT(ba->getPosition() == 0); \
}
    XX(int8_t, 100, writeFint8, readFint8, 1);
    XX(uint8_t, 100, writeFuint8, readFuint8, 1);
    XX(int16_t, 100, writeFint16, readFint16, 1);
    XX(uint16_t, 100, writeFuint16, readFuint16, 1);
    XX(int32_t, 100, writeFint32, readFint32, 1);
    XX(uint32_t, 100, writeFuint32, readFuint32, 1);
    XX(int64_t, 100, writeFint64, readFint64, 1);
    XX(uint64_t, 100, writeFuint64, readFuint64, 1);

    XX(int32_t, 100, writeInt32, readInt32, 1);
    XX(uint32_t, 100, writeUint32, readUint32, 1);
    XX(int64_t, 100, writeInt64, readInt64, 1);
    XX(uint64_t, 100, writeUint64, readUint64, 1);
#undef XX

}


int main()
{
    test();
    return 0;
}