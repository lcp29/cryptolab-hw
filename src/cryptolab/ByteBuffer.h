//
// Created by helmholtz on 2022/9/28.
//

#ifndef CRYPTOLAB_BYTEBUFFER_H
#define CRYPTOLAB_BYTEBUFFER_H

#include <string>
#include <vector>
#include <ostream>
#include <iomanip>

namespace cryptolab
{

    class ByteBuffer
    {
    private:
        std::vector<unsigned char> dat;
    public:
        ByteBuffer() = default;

        ByteBuffer(const ByteBuffer &b);

        ByteBuffer(const std::initializer_list<unsigned char> &b);

        ByteBuffer(const std::string &b);

        unsigned char &operator[](unsigned long idx);

        const unsigned char &operator[](unsigned long idx) const;

        unsigned long size() const;

        std::string toString() const;

        void resize(unsigned long s);

        void pushBack(unsigned char c);

        unsigned char popBack();

        unsigned char *data();

        const unsigned char *data() const;

        static ByteBuffer fromHexStr(const char *s);

        friend std::ostream &operator<<(std::ostream &os, const ByteBuffer &b);
    };

    std::ostream &operator<<(std::ostream &os, const ByteBuffer &b);
} // cryptolab

#endif //CRYPTOLAB_BYTEBUFFER_H
