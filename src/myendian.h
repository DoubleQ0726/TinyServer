#pragma once
#define TINY_LITTLE_ENDIAN 1
#define TINY_BIG_ENDIAN 2

#include <byteswap.h>
#include <stdint.h>
#include <type_traits>

namespace TinyServer
{

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type
byteswap(T value)
{
    return (T)bswap_64((uint64_t)value);
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type
byteswap(T value)
{
    return (T)bswap_32((uint32_t)value);
}

template<typename T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type
byteswap(T value)
{
    return (T)bswap_16((uint16_t)value);
}

#if BYTE_ORDER == BIG_ENDIAN
#define TINY_BYTE_ORDER TINY_BIG_ENDIAN
#else
#define TINY_BYTE_ORDER TINY_LITTLE_ENDIAN
#endif

#if TINY_BYTE_ORDER == TINY_BIG_ENDIAN
template<typename T>
T byteswapOnLittleEndian(T t)
{
    return t;
}

template<typename T>
T byteswapOnBigEndian(T t)
{
    return byteswap(t);
}
#else
/* 只在小端机器上执行byteswap, 在大端机器上什么都不做 */
template<typename T>
T byteswapOnLittleEndian(T t)
{
    return byteswap(t);
}

template<typename T>
T byteswapOnBigEndian(T t)
{
    return t;
}
#endif

}