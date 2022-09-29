//
// Created by helmholtz on 2022/9/28.
//

#include "ByteBuffer.h"

namespace cryptolab
{
    ByteBuffer::ByteBuffer(const std::initializer_list<unsigned char> &b)
    {
        for (auto &i: b)
            dat.push_back(i);
    }

    const unsigned char &ByteBuffer::operator[](unsigned long idx) const
    {
        return dat[idx];
    }

    unsigned char &ByteBuffer::operator[](unsigned long idx)
    {
        return dat[idx];
    }

    unsigned long ByteBuffer::size() const
    {
        return dat.size();
    }

    void ByteBuffer::resize(unsigned long s)
    {
        dat.resize(s);
    }

    ByteBuffer::ByteBuffer(const std::string &b)
    {
        for (auto &i : b)
            dat.push_back(i);
    }

    std::string ByteBuffer::toString() const
    {
        std::string ret;
        for (auto &i : dat)
            ret.push_back(i);
        return ret;
    }

    std::ostream &operator<<(std::ostream &os, const ByteBuffer &b)
    {
        for (auto &i : b.dat)
            os << std::setw(2) << std::setfill('0') << +i;
        return os;
    }

    void ByteBuffer::pushBack(unsigned char c)
    {
        dat.push_back(c);
    }

    unsigned char ByteBuffer::popBack()
    {
        unsigned char ret = dat[dat.size() - 1];
        dat.pop_back();
        return ret;
    }

    unsigned char *ByteBuffer::data()
    {
        return dat.data();
    }

    const unsigned char *ByteBuffer::data() const
    {
        return dat.data();
    }

    ByteBuffer ByteBuffer::fromHexStr(const char *s)
    {
        ByteBuffer ret;
        for (int i = 0; s[i + 1] != '\0'; i += 2)
        {
            char bStr[3] = {s[i], s[i + 1], 0};
            ret.dat.push_back(strtol(bStr, nullptr, 16));
        }
        return ret;
    }
} // cryptolab