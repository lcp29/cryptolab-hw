//
// Created by helmholtz on 2022/10/10.
//
#include <iostream>
#include <ctime>
#include "RSA.h"
#include "BigInteger.h"

using namespace std;
using namespace cryptolab;

int main()
{
    BigInteger<32> a((uint64_t)0x5af3107a4000), b((uint64_t)29);
    RSA<16> rsa;
    cout << ((-SignedBigInteger<1>(17)) % SignedBigInteger<1>(19)).digit[0] << endl;
    cout << RSA<16>::inverseP(5, 7)[0] << endl;
    rsa.genKey();
    return 0;
}
