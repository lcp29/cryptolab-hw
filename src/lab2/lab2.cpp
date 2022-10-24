//
// Created by helmholtz on 2022/10/10.
//
#include <iostream>
#include <string>
#include <chrono>
#include "RSA.h"
#include "ByteBuffer.h"
#include "BigInteger.h"

using namespace std;
using namespace cryptolab;

ByteBuffer str2buf(const string &o, int blockSize);

string buf2str(const ByteBuffer &b);


int main()
{
    // BigInteger<32> a("19b308bf8317e42f54b262b901d8088b639a379aff331c884f644fcd507eaa189ef3447e2c9e23d09b5634b3f2f7c0a8f8cfb998432167a128d0febfffffffffe8935e6b8fb20f4b8eaa6dc906c093c699656b017fb8c91ddae409f9fe9b7bac7a14ef16f4618133dd53190655e4a1283d3b5d8fec85c191f0422adabe749cc0");
    // BigInteger<32> b("7cfbf54651025d7f62efbaaf10185e6651e73bd12ff5261421d836b70ee895af");
    // cout << a % b ;
    // 直接将明文硬编码
    string originalText = "2002 A.M. TURING AWARD. RSA, an acronym for Rivest, Shamir and Adleman, uses algorithmic number theory to provide an efficient realization of a public-key cryptosystem, a concept first envisioned theoretically by Whitfield Diffie, Martin Hellman and Ralph Merkle. RSA is now the most widely used encryption method, with applications throughout the Internet to secure on-line transactions. It has also inspired breakthrough work in both theoretical computer science and mathematics.";
    ByteBuffer cBuf = str2buf(originalText, 63);
    // rsa512 (16 x 32b)
    RSA<16> rsa;
    cout << "正在生成密钥...";
    auto startTime = chrono::system_clock::now();
    rsa.genKey();
    auto endTime = chrono::system_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(endTime - startTime);
    cout << "已生成，用时" << (double) duration.count() * chrono::microseconds::period::num / chrono::microseconds::period::den << "秒." << endl;
    cout << "p:   0x" << rsa.getKeyP1() <<
         "\nq:   0x" << rsa.getKeyP2() <<
         "\nn:   0x" << rsa.getKeyN() <<
         "\ne:   0x" << rsa.getKeyE() <<
         "\nd:   0x" << rsa.getKeyD() <<
         "\nphi: 0x" << rsa.getKeyPhi() << endl;
    auto c = rsa.encrypt(cBuf, 63);
    cout << "密文：\n" << c << endl;
    auto m = rsa.decrypt(c, 63);
    cout << "明文：\n" << buf2str(m) << endl;
    return 0;
}

// 4个明文字符分成一组进行加密
// 例：对于明文tuvna，分组为
// 第一组
// | 0..7 | 8..15 | 16..23 | 24..31
// | t    | u     | v      | n
// 第二组
// | 0..7 | 8..15 | 16..23 | 24..31
// | a    | \0    | \0     | \0

ByteBuffer str2buf(const string &o, int blockSize)
{
    unsigned long blockNum = (o.length() + blockSize - 1) / blockSize;
    ByteBuffer ret;
    ret.resize(blockNum * blockSize);
    for (int i = 0; i < blockNum * blockSize; ++i)
        ret[i] = i >= o.length() ? 0 : o[i];
    return ret;
}

string buf2str(const ByteBuffer &b)
{
    string ret;
    for (int i = 0; i < b.size() && b[i] != '\0'; ++i)
        ret.push_back(b[i]);
    return ret;
}
