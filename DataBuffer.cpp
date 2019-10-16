#include "DataBuffer.h"
#include <string.h>

DataBuffer::DataBuffer()
{
    m_Size = 0;
}

DataBuffer::DataBuffer(const char *_source, const size_t _size)
{
    m_BufPtr.reset(new char[_size]);
    memcpy(m_BufPtr.get(),_source,_size);
    m_Size = _size;
}

DataBuffer::DataBuffer(const byte *_source, const size_t _size)
{
    m_BufPtr.reset(new char[_size]);
    memcpy(m_BufPtr.get(),_source,_size);
    m_Size = _size;
}

DataBuffer::DataBuffer(const DataBuffer &_copy)
{
    m_BufPtr.reset(new char[_copy.m_Size]);
    memcpy(m_BufPtr.get(),_copy.m_BufPtr.get(),_copy.m_Size);
    m_Size = _copy.m_Size;
}

bool DataBuffer::operator==(const DataBuffer &_buf) const
{
    if(m_Size != _buf.m_Size)
    {
        return false;
    }

    for(size_t l = 0;l < m_Size;++l)
    {
        if(m_BufPtr[0] != _buf.m_BufPtr[0])
        {
            return false;
        }
    }

    return true;
}

DataBuffer &DataBuffer::operator=(const DataBuffer &_buf)
{
    m_BufPtr.reset(new char[_buf.m_Size]);
    memcpy(m_BufPtr.get(),_buf.m_BufPtr.get(),_buf.m_Size);
    m_Size = _buf.m_Size;

    return *this;
}

DataBuffer DataBuffer::operator+(const DataBuffer &_buf) const
{
    size_t _size = m_Size + _buf.m_Size;
    unique_ptr<char[]> tmp(new char[_size]);
    memset(tmp.get(),0,_size);
    memcpy(tmp.get(),m_BufPtr.get(),m_Size);
    memcpy(tmp.get()+m_Size,_buf.m_BufPtr.get(),_buf.m_Size);

    return DataBuffer(tmp.get(),_size);
}

DataBuffer &DataBuffer::operator+=(const DataBuffer &_buf)
{
    size_t _size = m_Size + _buf.m_Size;
    unique_ptr<char[]> tmp(new char[_size]);
    memset(tmp.get(),0,_size);
    memcpy(tmp.get(),m_BufPtr.get(),m_Size);
    memcpy(tmp.get()+m_Size,_buf.m_BufPtr.get(),_buf.m_Size);

    m_BufPtr.reset(new char[_size]);
    memcpy(m_BufPtr.get(),tmp.get(),_size);
    m_Size = _size;

    return *this;
}

DataBuffer::operator bool() const
{
    if(m_Size == 0)
    {
        return false;
    }

    return true;
}

size_t DataBuffer::GetSize() const
{
    return m_Size;
}

char *DataBuffer::GetBuffer() const
{
    return m_BufPtr.get();
}
