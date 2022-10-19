//
// Created by helmholtz on 2022/10/14.
//

#ifndef CRYPTOLAB_RSA_H
#define CRYPTOLAB_RSA_H

#include "BigInteger.h"
#include "SignedBigInteger.h"
#include <random>
#include <chrono>

namespace cryptolab
{
    template<int N>
    class RSA
    {
    private:
        std::default_random_engine eng;
        BigInteger<N> n;
        BigInteger<N> d;
        BigInteger<N> e;
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
        BigInteger<N> p = genPrime(), q = genPrime();
        n = p * q;
        BigInteger<N> phi = (p - 1) * (q - 1);
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
        SignedBigInteger<N> x1 = 1, x2 = 0, x3 = p;
        SignedBigInteger<N> y1 = 0, y2 = 1, y3 = a;
        while(true)
        {
            if (y3.digit == 0) return 0;
            if (y3.digit == 1) return y2.digit % p;
            auto q = x3 / y3;
            auto t1 = (x1 - q * y1) % p, t2 = (x2 - q * y2) % p, t3 = (x3 - q * y3) % p;
            x1 = y1, x2 = y2, x3 = y3;
            y1 = t1, y2 = t2, y3 = t3;
        }
    }

} // cryptolab

#endif //CRYPTOLAB_RSA_H
