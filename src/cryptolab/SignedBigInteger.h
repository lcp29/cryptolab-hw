//
// Created by helmholtz on 2022/10/15.
//

#ifndef CRYPTOLAB_SIGNEDBITINTEGER_H
#define CRYPTOLAB_SIGNEDBITINTEGER_H

#include "BigInteger.h"

namespace cryptolab
{
    template<int N>
    class SignedBigInteger
    {
    public:
        // 全部public开摆
        BigInteger<N> digit;
        bool sign;

        SignedBigInteger() : digit(0), sign(false)
        {};

        SignedBigInteger<N>(uint64_t b)
        {
            digit = b;
            sign = false;
        }

        SignedBigInteger(const BigInteger<N> &a) : digit(a), sign(false)
        {};

        SignedBigInteger(const BigInteger<N> &a, uint8_t s) : digit(a), sign(s)
        {};

        SignedBigInteger(const SignedBigInteger<N> &a)
        {
            digit = a.digit;
            sign = a.sign;
        }

        SignedBigInteger<N> operator+(const SignedBigInteger<N> &b) const;

        SignedBigInteger<N> operator-() const;

        SignedBigInteger<N> operator-(const SignedBigInteger<N> &b) const;

        SignedBigInteger<N> operator*(const SignedBigInteger<N> &b) const;

        SignedBigInteger<N> operator/(const SignedBigInteger<N> &b) const;

        SignedBigInteger<N> operator%(const SignedBigInteger<N> &b) const;
    };

    template<int N>
    SignedBigInteger<N> SignedBigInteger<N>::operator+(const SignedBigInteger<N> &b) const
    {
        SignedBigInteger<N> ret;
        if (sign == b.sign)
        {
            ret.digit = digit + b.digit;
            ret.sign = sign;
        } else
        {
            if (digit > b.digit)
            {
                ret.sign = sign;
                ret.digit = digit - b.digit;
            } else if (digit < b.digit)
            {
                ret.sign = b.sign;
                ret.digit = b.digit - digit;
            } else
                ret = 0;
        }
        return ret;
    }

    template<int N>
    SignedBigInteger<N> SignedBigInteger<N>::operator-() const
    {
        SignedBigInteger<N> ret(*this);
        ret.sign = !ret.sign;
        return ret;
    }

    template<int N>
    SignedBigInteger<N> SignedBigInteger<N>::operator-(const SignedBigInteger<N> &b) const
    {
        return *this + (-b);
    }

    template<int N>
    SignedBigInteger<N> SignedBigInteger<N>::operator*(const SignedBigInteger<N> &b) const
    {
        SignedBigInteger<N> ret;
        ret.sign = sign ^ b.sign;
        ret.digit = digit * b.digit;
        return ret;
    }

    template<int N>
    SignedBigInteger<N> SignedBigInteger<N>::operator/(const SignedBigInteger<N> &b) const
    {
        SignedBigInteger<N> ret;
        ret.sign = sign ^ b.sign;
        ret.digit = digit / b.digit;
        return ret;
    }

    template<int N>
    SignedBigInteger<N> SignedBigInteger<N>::operator%(const SignedBigInteger<N> &b) const
    {
        if (!sign)
            return digit % b.digit;
        else
            return b.digit - digit % b.digit;
    }
} // cryptolab

#endif //CRYPTOLAB_SIGNEDBITINTEGER_H
