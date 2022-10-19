//
// Created by helmholtz on 2022/10/11.
//

#ifndef CRYPTOLAB_BIGINTEGER_H
#define CRYPTOLAB_BIGINTEGER_H

#include <cstdint>
#include <random>
#include <cstdlib>

namespace cryptolab
{
    // 无符号整数，占用4N digit
    template<int N>
    class BigInteger
    {
    private:
        uint32_t dat[N];
    public:
        BigInteger<N>() = default;

//        BigInteger<N>(int32_t digit)
//        {
//            assert(N > 0);
//            dat[0] = digit;
//            int fill = digit < 0 ? -1 : 0;
//            memset(dat + 1, fill, (N - 1) * 4);
//        }

        BigInteger<N>(uint64_t b)
        {
            dat[0] = b;
            if (N > 1)
            {
                dat[1] = b >> 32;
                memset(dat + 2, 0, (N - 2) * 4);
            }
        }

        BigInteger<N>(const BigInteger<N> &b)
        {
            memcpy(dat, b.dat, N * 4);
        }

        BigInteger<N> operator+(const BigInteger<N> &b) const;

        BigInteger<N> &operator+=(const BigInteger<N> &b);

        BigInteger<N> operator++();

        BigInteger<N> operator-() const;

        BigInteger<N> operator-(const BigInteger<N> &b) const;

        BigInteger<N> &operator-=(const BigInteger<N> &b);

        BigInteger<N> operator--();

        BigInteger<N> operator*(const BigInteger<N> &b) const;

        BigInteger<N> &operator*=(const BigInteger<N> &b);

        BigInteger<N> operator/(const BigInteger<N> &b) const;

        BigInteger<N> &operator/=(const BigInteger<N> &b);

        BigInteger<N> operator%(const BigInteger<N> &b) const;

        BigInteger<N> &operator%=(const BigInteger<N> &b);

        BigInteger<N> operator<<(int b) const;

        BigInteger<N> &operator<<=(int b);

        BigInteger<N> operator>>(int b) const;

        BigInteger<N> &operator>>=(int b);

        bool operator>(const BigInteger<N> &b) const;

        bool operator>=(const BigInteger<N> &b) const;

        bool operator<(const BigInteger<N> &b) const;

        bool operator<=(const BigInteger<N> &b) const;

        bool operator==(const BigInteger<N> &b) const;

        bool operator!=(const BigInteger<N> &b) const;

        BigInteger<N> uint32mul(uint32_t b) const;

        BigInteger<N> powMod(BigInteger<N> p, BigInteger<N> m);

        uint32_t operator[](int idx) const;

        uint32_t &operator[](int idx);

        static BigInteger<N> exp2(int e);

        static BigInteger<N> random(std::default_random_engine &rndEng, int start_idx = 0, int end_idx = N);
    };

    template<int N>
    BigInteger<N> BigInteger<N>::operator+(const BigInteger<N> &b) const
    {
        BigInteger<N> ret;
        uint64_t c = 0;
        for (int i = 0; i < N; ++i)
        {
            uint64_t longMidResult = (uint64_t) dat[i] + (uint64_t) b[i] + c;
            ret[i] = longMidResult;
            c = longMidResult >> 32;
        }
        return ret;
    }

    template<int N>
    uint32_t BigInteger<N>::operator[](int idx) const
    {
        return dat[idx];
    }

    template<int N>
    uint32_t &BigInteger<N>::operator[](int idx)
    {
        return dat[idx];
    }

    template<int N>
    BigInteger<N> BigInteger<N>::operator-() const
    {
        BigInteger<N> ret;
        int i = 0;
        for (; dat[i] == 0 && i < N; ++i);
        if (i != N)
        {
            ret[i] = -(*(int32_t*) &dat[i]);
            for (++i; i < N; ++i)
                ret[i] ^= 0xFFFFFFFF;
        }
        return ret;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::operator-(const BigInteger<N> &b) const
    {
        BigInteger<N> ret;
        uint64_t c = 0;
        for (int i = 0; i < N; ++i)
        {
            uint64_t longMidResult = (uint64_t) dat[i] - (uint64_t) b[i] + c;
            ret[i] = longMidResult;
            c = *(int64_t*)&longMidResult >> 32;
        }
        return ret;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::operator*(const BigInteger<N> &b) const
    {
        BigInteger<N> ret(0);
        for (int i = 0; i < N; ++i)
        {
            ret <<= 1;
            ret += this->uint32mul(b[N - i - 1]);
        }
        return ret;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::uint32mul(uint32_t b) const
    {
        BigInteger<N> ret(0);
        for (int i = 0; i < N; ++i)
        {
            BigInteger<N> midRes = (uint64_t) dat[i] * (uint64_t) b;
            midRes <<= i;
            ret += midRes;
        }
        return ret;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::operator<<(int b) const
    {
        BigInteger<N> ret;
        memcpy(ret.dat + b, dat, 4 * (N - b));
        for (int i = 0; i < b; ++i)
            ret[i] = 0;
        return ret;
    }

    template<int N>
    BigInteger<N> &BigInteger<N>::operator+=(const BigInteger<N> &b)
    {
        uint64_t c = 0;
        for (int i = 0; i < N; ++i)
        {
            uint64_t longMidResult = (uint64_t) dat[i] + (uint64_t) b[i] + c;
            dat[i] = longMidResult;
            c = longMidResult >> 32;
        }
        return *this;
    }

    template<int N>
    BigInteger<N> &BigInteger<N>::operator<<=(int b)
    {
        memcpy(dat + b, dat, 4 * (N - b));
        for (int i = 0; i < b; ++i)
            dat[i] = 0;
        return *this;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::operator/(const BigInteger<N> &b) const
    {
        BigInteger<N> ret(0);
        BigInteger<N> rem(*this);
        if (b == BigInteger<N>(0))
            return BigInteger<N>(0);
        while (rem >= b)
        {
            int remLnz = N - 1, bLnz = N - 1;
            for (; b[bLnz] == 0; --bLnz);
            for (; rem[remLnz] == 0; --remLnz);
            if (rem[remLnz] <= b[bLnz])
                if(--remLnz < bLnz)
                {
                    if (rem >= b)
                    {
                        ++ret;
                        break;
                    }
                }
            // 分母
            uint64_t u = rem[remLnz];
            if (remLnz != N - 1)
                u += ((uint64_t) rem[remLnz + 1]) << 32;
            // 分子
            uint64_t v = b[bLnz] + 1;
            // 计算部分商
            uint64_t q = u / v;
            // 移位补齐
            BigInteger<N> mid(q);
            mid <<= remLnz - bLnz;
            // 余数减去已经除掉的部分
            rem -= mid * b;
            // 商加上部分商
            ret += mid;
        }
        return ret;
    }

    template<int N>
    BigInteger<N> &BigInteger<N>::operator-=(const BigInteger<N> &b)
    {
        uint64_t c = 0;
        for (int i = 0; i < N; ++i)
        {
            uint64_t longMidResult = (uint64_t) dat[i] - (uint64_t) b[i] + c;
            dat[i] = longMidResult;
            c = *(int64_t*)&longMidResult >> 32;
        }
        return *this;
    }

    template<int N>
    BigInteger<N> &BigInteger<N>::operator*=(const BigInteger<N> &b)
    {
        *this = *this * b;
        return *this;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::operator>>(int b) const
    {
        BigInteger<N> ret;
        uint32_t fill = dat[N - 1] & 0x80000000 ? 0xFFFFFFFF : 0;
        memcpy(ret, dat + b, (N - b) * 4);
        for (int i = N - b; i < N; ++i)
            ret[i] = fill;
        return ret;
    }

    template<int N>
    BigInteger<N> &BigInteger<N>::operator>>=(int b)
    {
        uint32_t fill = dat[N - 1] & 0x80000000 ? 0xFFFFFFFF : 0;
        memcpy(dat, dat + b, (N - b) * 4);
        for (int i = N - b; i < N; ++i)
            dat[i] = fill;
        return *this;
    }

    template<int N>
    bool BigInteger<N>::operator>(const BigInteger<N> &b) const
    {
        for (int i = N - 1; i >= 0; --i)
        {
            if (dat[i] > b[i])
                return true;
            else if (dat[i] < b[i])
                return false;
        }
        return false;
    }

    template<int N>
    bool BigInteger<N>::operator>=(const BigInteger<N> &b) const
    {
        for (int i = N - 1; i >= 0; --i)
        {
            if (dat[i] > b[i])
                return true;
            else if (dat[i] < b[i])
                return false;
        }
        return true;
    }

    template<int N>
    bool BigInteger<N>::operator<(const BigInteger<N> &b) const
    {
        return b > *this;
    }

    template<int N>
    bool BigInteger<N>::operator<=(const BigInteger<N> &b) const
    {
        return b >= *this;
    }

    template<int N>
    bool BigInteger<N>::operator==(const BigInteger<N> &b) const
    {
        for (int i = 0; i < N; ++i)
            if (dat[i] != b[i])
                return false;
        return true;
    }

    template<int N>
    BigInteger<N> &BigInteger<N>::operator/=(const BigInteger<N> &b)
    {
        *this = *this / b;
        return *this;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::operator%(const BigInteger<N> &b) const
    {
        BigInteger<N> rem(*this);
        if (b == BigInteger<N>(0))
            return BigInteger<N>(0);
        while (rem >= b)
        {
            int remLnz = N - 1, bLnz = N - 1;
            for (; b[bLnz] == 0; --bLnz);
            for (; rem[remLnz] == 0; --remLnz);
            if (rem[remLnz] <= b[bLnz])
                if(--remLnz < bLnz)
                {
                    if (rem >= b)
                    {
                        rem -= b;
                        break;
                    }
                }
            // 分母
            uint64_t u = rem[remLnz];
            if (remLnz != N - 1)
                u += ((uint64_t) rem[remLnz + 1]) << 32;
            // 分子，+1防止溢出
            uint64_t v = b[bLnz] + 1;
            // 计算部分商
            uint64_t q = u / v;
            // 移位补齐
            BigInteger<N> mid(q);
            mid <<= remLnz - bLnz;
            // 余数减去已经除掉的部分
            rem -= mid * b;
        }
        return rem;
    }

    template<int N>
    BigInteger<N> &BigInteger<N>::operator%=(const BigInteger<N> &b)
    {
        *this = *this % b;
        return *this;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::random(std::default_random_engine &rndEng, int start_idx, int end_idx)
    {
        BigInteger<N> ret(0);
        for (int i = start_idx; i < end_idx; ++i)
            ret[i] = rndEng();
        return ret;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::operator++()
    {
        auto ret = *this;
        *this += 1;
        return ret;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::operator--()
    {
        auto ret = *this;
        *this -= 1;
        return ret;
    }

    template<int N>
    BigInteger<N> BigInteger<N>::powMod(BigInteger<N> p, BigInteger<N> m)
    {
        BigInteger<N> ret = 1;
        BigInteger<N> t = *this;
        int h = N - 1;
        for (; p[h] == 0 && h >= 0; --h);
        for (int i = 0; i <= h; ++i)
        {
            for (int j = 0; j < 32; ++j)
            {
                if ((p[i] >> j) & 1)
                    ret = (ret * t) % m;
                t = (t * t) % m;
            }
        }
        return ret;
    }

    template<int N>
    bool BigInteger<N>::operator!=(const BigInteger<N> &b) const
    {
        return !(*this == b);
    }

    template<int N>
    BigInteger<N> BigInteger<N>::exp2(int e)
    {
        BigInteger<N> ret(0);
        if (e < 0 || e > 32 * N - 1)
            return ret;
        ret[e / 32] = ((uint64_t) 1) << (e % 32);
        return ret;
    }

    template<int N>
    BigInteger<N> &operator++(BigInteger<N> &a)
    {
        a += 1;
        return a;
    }

    template<int N>
    BigInteger<N> &operator--(BigInteger<N> &a)
    {
        a -= 1;
        return a;
    }

} // cryptolab

#endif //CRYPTOLAB_BIGINTEGER_H
