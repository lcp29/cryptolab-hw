//
// Created by helmholtz on 2022/10/14.
//

#ifndef CRYPTOLAB_RSA_H
#define CRYPTOLAB_RSA_H

#include "BigInteger.h"
#include "SignedBigInteger.h"
#include "ByteBuffer.h"
#include <random>
#include <chrono>

namespace cryptolab
{
    template<int N>
    class RSA
    {
    private:
        std::default_random_engine eng;
        BigInteger<N> phi;
        BigInteger<N> n;
        BigInteger<N> d;
        BigInteger<N> e;
        BigInteger<N> p1;
        BigInteger<N> p2;
    public:
        RSA<N>()
        {
            eng.seed(std::chrono::system_clock::now().time_since_epoch().count());
        }

        BigInteger<N> genPrime(int aNum = 10);

        static bool checkCoPrime(const BigInteger<N> &a, const BigInteger<N> &b);

        static BigInteger<N> inverseP(const BigInteger<N> &a, const BigInteger<N> &p);

        void genKey();

        const BigInteger<N> &getKeyN() const
        { return n; };

        const BigInteger<N> &getKeyD() const
        { return d; };

        const BigInteger<N> &getKeyE() const
        { return e; };

        const BigInteger<N> &getKeyPhi() const
        { return phi; }

        const BigInteger<N> &getKeyP1() const
        { return p1; }

        const BigInteger<N> &getKeyP2() const
        { return p2; }

        BigInteger<N> encryptElement(const BigInteger<N> &c) const;

        BigInteger<N> decryptElement(const BigInteger<N> &m) const;

        /**
         * 加密一个ByteBuffer块
         * @param c 待加密Buffer
         * @param blockSize 块大小，以字节为单位
         * @return 加密后的Buffer
         */
        ByteBuffer encrypt(const ByteBuffer &c, int blockSize = 2);

        /**
         * 解密一个ByteBuffer
         * @param c 待解密Buffer
         * @param blockSize 块大小，以字节为单位
         * @return 解密后的Buffer
         */
        ByteBuffer decrypt(const ByteBuffer &c, int blockSize = 2);
    };

    template<int N>
    BigInteger<N> RSA<N>::genPrime(int aNum)
    {
        while (true)
        {
            bool isPrime = true;
            auto primeCandidate = BigInteger<N>::random(eng, 0, N / 2);
            primeCandidate[0] |= 1;
            if ((primeCandidate % 3 == 0) ||
                (primeCandidate % 5 == 0) ||
                (primeCandidate % 7 == 0) ||
                (primeCandidate % 11 == 0))
                continue;
            // primeCandidate = BigInteger<N>(63018038201);
            // p - 1 = 2^k * q
            auto q = primeCandidate - 1;
            auto k = BigInteger<N>(0);
            for (; (q[0] & 1) == 0; ++k, q /= 2);
            for (int i = 0; i < aNum; ++i)
            {
                // 1 < a < p - 1
                BigInteger<N> a = BigInteger<N>::random(eng) % (primeCandidate - 2) + 1;
                // 1. a^q % p == 1 ?
                // 2. a^(2^j*q) % p == p - 1 ?
                bool cond1, cond2 = false;
                auto aq = a.powMod(q, primeCandidate);
                cond1 = (aq == 1);
                for (int ik = 0; k > ik; ++ik)
                {
                    if (aq == primeCandidate - 1)
                    {
                        cond2 = true;
                        break;
                    }
                    aq = (aq * aq) % primeCandidate;
                }
                if (!(cond1 || cond2))
                {
                    isPrime = false;
                    break;
                }
            }
            if (isPrime)
                return primeCandidate;
        }
    }

    template<int N>
    void RSA<N>::genKey()
    {
        p1 = genPrime(), p2 = genPrime();
        n = p1 * p2;
        phi = (p1 - 1) * (p2 - 1);
        // generate e
        auto e0 = BigInteger<N>::random(eng) % phi;
        e = e0;
        while(!checkCoPrime(e, phi))
            if(++e >= phi)
                e = BigInteger<N>::random(eng) % e0;
        // generate d
        d = inverseP(e, phi);
    }

    template<int N>
    bool RSA<N>::checkCoPrime(const BigInteger<N> &a, const BigInteger<N> &b)
    {
        BigInteger<N> p(a), q(b), r(a % b);
        while(r != 0)
        {
            p = q;
            q = r;
            r = p % q;
        }
        return q == 1;
    }

    template<int N>
    BigInteger<N> RSA<N>::inverseP(const BigInteger<N> &a, const BigInteger<N> &p)
    {
        BigInteger<2 * N> p2 = p;
        BigInteger<2 * N> a2 = a;
        SignedBigInteger<2 * N> x1 = 1, x2 = 0, x3 = p2;
        SignedBigInteger<2 * N> y1 = 0, y2 = 1, y3 = a2;
        while(true)
        {
            if (y3.digit == BigInteger<2 * N>(0)) return 0;
            if (y3.digit == BigInteger<2 * N>(1)) return y2.digit % p2;
            auto q = x3 / y3;
            auto t1 = ((x1 % p2) - ((q * y1) % p2)) % p2, t2 = ((x2 % p2) - ((q * y2) % p2)) % p2, t3 = ((x3 % p2) - ((q * y3) % p2)) % p2;
            x1 = y1, x2 = y2, x3 = y3;
            y1 = t1, y2 = t2, y3 = t3;
        }
    }

    template<int N>
    BigInteger<N> RSA<N>::encryptElement(const BigInteger<N> &c) const
    {
        return c.powMod(e, n);
    }

    template<int N>
    BigInteger<N> RSA<N>::decryptElement(const BigInteger<N> &m) const
    {
        return m.powMod(d, n);
    }

    template<int N>
    ByteBuffer RSA<N>::encrypt(const ByteBuffer &c, int blockSize)
    {
        assert(blockSize < 4 * N);
        assert(c.size() % blockSize == 0);
        unsigned long blockNum = c.size() / blockSize;
        ByteBuffer ret;
        ret.resize(blockNum * 4 * N);
        for (int i = 0; i < blockNum; ++i)
        {
            BigInteger<N> mBlock(0);
            for (int j = blockSize - 1; j >= 0; --j)
            {
                mBlock *= 0x100;
                mBlock += c[i * blockSize + j];
            }
            BigInteger<N> cBlock = encryptElement(mBlock);
            for (int j = 0; j < 4 * N; ++j)
            {
                ret[i * 4 * N + j] = cBlock[0] & 0xFF;
                cBlock /= 0x100;
            }
        }
        return ret;
    }

    template<int N>
    ByteBuffer RSA<N>::decrypt(const ByteBuffer &c, int blockSize)
    {
        assert(blockSize < 4 * N);
        assert(c.size() % blockSize == 0);
        unsigned long blockNum = c.size() / (4 * N);
        ByteBuffer ret;
        ret.resize(blockNum * blockSize);
        for (int i = 0; i < blockNum; ++i)
        {
            BigInteger<N> cBlock(0);
            for (int j = 4 * N - 1; j >= 0; --j)
            {
                cBlock *= 0x100;
                cBlock += c[i * 4 * N + j];
            }
            BigInteger<N> mBlock = decryptElement(cBlock);
            for (int j = 0; j < blockSize; ++j)
            {
                ret[i * blockSize + j] = mBlock[0] & 0xFF;
                mBlock /= 0x100;
            }
        }
        return ret;
    }

} // cryptolab

#endif //CRYPTOLAB_RSA_H
