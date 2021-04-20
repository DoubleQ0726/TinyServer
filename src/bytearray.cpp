#include "bytearray.h"
#include "myendian.h"
#include "string.h"
#include <fstream>
#include <sstream>
#include <iomanip>

namespace TinyServer
{
static Ref<Logger> logger = TINY_LOG_NAME("system");

ByteArray::Node::Node(size_t s)
    : size(s), ptr(new char[s]), next(nullptr)
{

}

ByteArray::Node::Node()
    : size(0), ptr(nullptr), next(nullptr)
{

}

ByteArray::Node::~Node()
{
    if (ptr)
    {
        delete[] ptr;
    }
}

ByteArray::ByteArray(size_t base_size)
    : m_baseSize(base_size), m_position(0), m_capacity(base_size), m_size(0), m_endian(TINY_BIG_ENDIAN), m_root(new Node(base_size)), m_cur(m_root)
{

}

ByteArray::~ByteArray()
{
    Node* tmp = m_root;
    while (tmp)
    {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
}

bool ByteArray::isLittleEndian() const
{
    return m_endian == TINY_LITTLE_ENDIAN;
}
void ByteArray::setIsLittleEndian(bool val)
{
    if (val)
        m_endian = TINY_LITTLE_ENDIAN;
    else
        m_endian = TINY_BIG_ENDIAN;
}

//write
void ByteArray::writeFint8(int8_t value)
{
    write(&value, sizeof(value));
}
void ByteArray::writeFuint8(uint8_t value)
{
    write(&value, sizeof(value));
}

void ByteArray::writeFint16(int16_t value)
{
    if (m_endian != TINY_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint16(uint16_t value)
{
    if (m_endian != TINY_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFint32(int32_t value)
{
    if (m_endian != TINY_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint32(uint32_t value)
{
    if (m_endian != TINY_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFint64(int64_t value)
{
    if (m_endian != TINY_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

void ByteArray::writeFuint64(uint64_t value)
{
    if (m_endian != TINY_BYTE_ORDER)
    {
        value = byteswap(value);
    }
    write(&value, sizeof(value));
}

static uint32_t EncodeZigzag32(const int32_t& value)
{
    if (value < 0)
        return ((uint32_t)(-value) * 2 - 1);
    else
        return 2 * value; 
}

static uint64_t EncodeZigzag64(const int64_t& value)
{
    if (value < 0)
        return ((uint64_t)(-value) * 2 - 1);
    else
        return 2 * value; 
}

static int32_t DecodeZigzag32(const uint32_t& value)
{
    return (value >> 1) ^ -(value & 1);
}

static int64_t DecodeZigzag64(const uint64_t& value)
{
    return (value >> 1) ^ -(value & 1);
}

void ByteArray::writeInt32(int32_t value)
{
    writeUint32(EncodeZigzag32(value));
}

void ByteArray::writeUint32(uint32_t value)
{
    uint8_t tmp[5];
    uint8_t i = 0;
    while (value >= 0x80)
    {
        tmp[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    tmp[i++] = value;
    write(tmp, i);
}

void ByteArray::writeInt64(int64_t value)
{
    writeUint64(EncodeZigzag32(value));
}


void ByteArray::writeUint64(uint64_t value)
{
    uint8_t tmp[10];
    uint8_t i = 0;
    while (value >= 0x80)
    {
        tmp[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    tmp[i++] = value;
    write(tmp, i);
}

void ByteArray::writeFloat(float value)
{
    uint32_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint32(v);
}

void ByteArray::writeDouble(double value)
{
    uint64_t v;
    memcpy(&v, &value, sizeof(value));
    writeFuint64(v);
}


//length:int16, data
void ByteArray::writeStringF16(const std::string& value)
{
    writeFuint16(value.size());
    write(value.c_str(), value.size());
}

//length:int32, data
void ByteArray::writeStringF32(const std::string& value)
{
    writeFuint32(value.size());
    write(value.c_str(), value.size());
}
//length:int64, data
void ByteArray::writeStringF64(const std::string& value)
{
    writeFuint64(value.size());
    write(value.c_str(), value.size());
}

//length:varint, data
void ByteArray::writeStringVint(const std::string& value)
{
    writeUint64(value.size());
    write(value.c_str(), value.size());
}

//data
void ByteArray::writeStringWithoutLength(const std::string& value)
{
    write(value.c_str(), value.size());
}

int8_t ByteArray::readFint8()
{
    int8_t v;
    read(&v, sizeof(v));
    return v;
}

uint8_t ByteArray::readFuint8()
{
    uint8_t v;
    read(&v, sizeof(v));
    return v;
}

#define XX(type) \
    type v; \
    read(&v, sizeof(v)); \
    if (m_endian == TINY_BYTE_ORDER) \
    { \
        return v; \
    } \
    else \
    { \
        return byteswap(v); \
    }

int16_t ByteArray::readFint16()
{
    XX(int16_t);
}

uint16_t ByteArray::readFuint16()
{
    XX(uint16_t);
}

int32_t ByteArray::readFint32()
{
    XX(int32_t);
}

uint32_t ByteArray::readFuint32()
{
    XX(uint32_t);
}

int64_t ByteArray::readFint64()
{
    XX(int64_t);
}

uint64_t ByteArray::readFuint64()
{
    XX(uint64_t);
}
#undef XX

int32_t ByteArray::readInt32()
{
    return DecodeZigzag32(readUint32());
}

uint32_t ByteArray::readUint32()
{
    uint32_t result = 0;
    for (int i = 0; i < 32; i += 7)
    {
        uint8_t b = readFuint8();
        if (b < 0x80)
        {
            result |= ((uint32_t)b) << i;
            break;
        }
        else
        {
            result |= (((uint32_t)(b & 0x7f)) << i);
        }
    }
    return result;
}


int64_t ByteArray::readInt64()
{
    return DecodeZigzag64(readUint64());
}

uint64_t ByteArray::readUint64()
{
    uint32_t result = 0;
    for (int i = 0; i < 64; i += 7)
    {
        uint8_t b = readFuint8();
        if (b < 0x80)
        {
            result |= ((uint64_t)b) << i;
            break;
        }
        else
        {
            result |= (((uint64_t)(b & 0x7f)) << i);
        }
    }
    return result;
}

float ByteArray::readFloat()
{
    uint32_t v = readFuint32();
    float value;
    memcpy(&value, &v, sizeof(v));
    return value;
}

double ByteArray::readDouble()
{
    uint64_t v = readFuint32();
    float value;
    memcpy(&value, &v, sizeof(v));
    return value;
}
//length:int16, data
std::string ByteArray::readStringF16()
{
    uint16_t len = readFuint16();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

//length:int32, data
std::string ByteArray::readStringF32()
{
    uint32_t len = readFuint32();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

//length:int64, data
std::string ByteArray::readStringF64()
{
    uint64_t len = readFuint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;
}

//data
std::string ByteArray::readStringVint()
{
    uint64_t len = readFuint64();
    std::string buff;
    buff.resize(len);
    read(&buff[0], len);
    return buff;  
}

void ByteArray::clear()
{
    m_position = m_size = 0;
    m_capacity = m_baseSize;
    Node* tmp = m_root->next;
    while(tmp)
    {
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
    m_cur = m_root;
    m_root->next = nullptr;
}

void ByteArray::write(const void* buf, size_t size)
{
    if (size == 0)
        return;
    addCapacity(size);
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;   //当前节点的剩余容量
    size_t bpos = 0;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy(m_cur->ptr + npos, (const char*)buf + bpos, size);
            if (m_cur->size == npos + size)
            {
                m_cur = m_cur->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        }
        else
        {
            memcpy(m_cur->ptr + npos, (const char*)buf + bpos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }
    if (m_position > m_size)
        m_size = m_position;
}

void ByteArray::read(void* buf, size_t size)
{
    if (size > getReadSize())
        throw std::out_of_range("not enough len");
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    size_t bpos = 0;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy((char*)buf + bpos, m_cur->ptr + npos, size);
            if (m_cur->size == npos + size)
            {
                m_cur = m_cur->next;
            }
            m_position += size;
            bpos += size;
            size = 0;
        }
        else
        {
            memcpy((char *)buf + bpos, m_cur->ptr + npos, ncap);
            m_position += ncap;
            bpos += ncap;
            size -= ncap;
            m_cur = m_cur->next;
            ncap = m_cur->size;
            npos = 0;
        }
    }
}

void ByteArray::read(void* buf, size_t size, size_t position) const
{
    if (size > getReadSize())
        throw std::out_of_range("not enough len");
    Node* cur = m_cur;
    size_t npos = position % m_baseSize;
    size_t ncap = cur->size - npos;
    size_t bpos = 0;
    while (size > 0)
    {
        if (ncap >= size)
        {
            memcpy((char*)buf + bpos, cur->ptr + npos, size);
            if (cur->size == npos + size)
            {
                cur = cur->next;
            }
            position += size;
            bpos += size;
            size = 0;
        }
        else
        {
            memcpy((char*)buf + bpos, cur->ptr + npos, ncap);
            position += ncap;
            bpos += ncap;
            size -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
    }
}

void ByteArray::setPosition(size_t pos)
{
    if (pos > m_capacity)
        throw std::out_of_range("set_position out of range");
    m_position = pos;
    if (m_position > m_size)
        m_size = m_position;
    m_cur = m_root;
    while (pos > m_cur->size)
    {
        pos -= m_cur->size;
        m_cur = m_cur->next;
    }
    if (pos == m_cur->size)
        m_cur = m_cur->next;
}

bool ByteArray::writeToFile(const std::string& name) const
{
    std::fstream ofs;
    ofs.open(name, std::ios::trunc | std::ios::binary | std::ios::out);
    if (!ofs)
    {
        TINY_LOG_ERROR(logger) << "writeToFile name = " << name << " error, errno = "
            << errno << " errstr = " << strerror(errno);
        return false;
    }
    int64_t read_size = getReadSize();
    int64_t pos = m_position;
    Node* cur = m_cur;
    while (read_size > 0)
    {
        int diff = pos % m_baseSize;
        int64_t len = (read_size > (int64_t)m_baseSize ? m_baseSize : read_size) - diff;

        ofs.write(cur->ptr + diff, len);
        cur = cur->next;
        pos += len;
        read_size -= len;
    }
    return true;
}

bool ByteArray::readFromFile(const std::string& name)
{
    std::ifstream ifs;
    ifs.open(name, std::ios::binary | std::ios::in);
    if (!ifs)
    {
        TINY_LOG_ERROR(logger) << "readFromFile name = " << name << " error, errno = "
            << errno << " errstr = " << strerror(errno);
        return false;
    }
    std::shared_ptr<char> buff(new char[m_baseSize], [](char *ptr){
        delete[] ptr;
    });
    while (!ifs.eof())
    {
        ifs.read(buff.get(), m_baseSize);
        write(buff.get(), ifs.gcount());
    }
    return true;
}

void ByteArray::addCapacity(size_t size)
{
    if (size == 0)
        return;
    size_t old_cap = getCurCapacity();
    if (old_cap >= size)
        return;
    
    size = size - old_cap;
    size_t count = (size / m_baseSize) + (((size % m_baseSize) > old_cap) ? 1 : 0);
    Node* tmp = m_root;
    while (tmp->next)
    {
        tmp = tmp->next;
    }
    Node* first = nullptr;
    for (size_t i = 0; i < count; ++i)
    {
        tmp->next = new Node(m_baseSize);
        if (first == nullptr)
            first = tmp->next;
        tmp = tmp->next;
        m_capacity += m_baseSize;
    }

    if (old_cap == 0)
        m_cur = first;
}

std::string ByteArray::toString() const
{
    std::string str;
    str.resize(getReadSize());
    if (str.empty())
        return str;
    read(&str[0], str.size(), m_position);
    return str;
}

std::string ByteArray::toHexString() const
{
    std::string str = toString();
    std::stringstream ss;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (i > 0 && i % 32 == 0)
            ss << std::endl;
        ss << std::setw(2) << std::setfill('0') << std::hex
           << (int)(uint8_t)str[i] << " "; 
    }
    return ss.str();
}

uint64_t ByteArray::getReadBuffers(std::vector<iovec>& buffers, uint64_t len) const
{
    len = len > getReadSize() ? getReadSize() : len;
    if (len == 0)
        return 0;
    uint64_t size = len;
    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    struct iovec iov;
    Node* cur = m_cur;
    while (len > 0)
    {
        if (ncap >= len)
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        }
        else
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

uint64_t ByteArray::getReadBuffers(std::vector<iovec>& buffers, uint64_t len, uint64_t position) const
{
    len = len > getReadSize() ? getReadSize() : len;
    if (len == 0)
        return 0;
    uint64_t size = len;
    size_t npos = position % m_baseSize;
    size_t count = position / m_baseSize;
    Node* cur = m_root;
    while (count > 0)
    {
        cur = cur->next;
        --count;
    }
    size_t ncap = m_cur->size - npos;
    struct iovec iov;
    while (len > 0)
    {
        if (ncap >= len)
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        }
        else
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}
uint64_t ByteArray::getWriteBuffers(std::vector<iovec>& buffers, uint64_t len)
{
    if (len == 0)
        return 0;
    addCapacity(len);
    uint64_t size = len;

    size_t npos = m_position % m_baseSize;
    size_t ncap = m_cur->size - npos;
    struct iovec iov;
    Node* cur = m_cur;
    while (len > 0)
    {
        if (ncap >= len)
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = len;
            len = 0;
        }
        else
        {
            iov.iov_base = cur->ptr + npos;
            iov.iov_len = ncap;
            len -= ncap;
            cur = cur->next;
            ncap = cur->size;
            npos = 0;
        }
        buffers.push_back(iov);
    }
    return size;
}

}