//
// Created by helmholtz on 2022/9/26.
//

#include "AES128.h"

namespace cryptolab
{
    void AES128::setInitKey(const aes128key &b)
    {
        subKeys[0] = b;
        rng.seed(time(nullptr));
        keyExpand();
    }

    void AES128::setInitKey(const gf28 *b)
    {
        for (int i = 0, n = 0; n < 4; ++n)
            for (int m = 0; m < 4; ++m, ++i)
                subKeys[0][m][n] = b[i];
        rng.seed(time(nullptr));
        keyExpand();
    }

    void AES128::setInitKey(const char *b)
    {
        for (int i = 0, n = 0; n < 4; ++n)
            for (int m = 0; m < 4; ++m, ++i)
                subKeys[0][m][n] = b[i];
        rng.seed(time(nullptr));
        keyExpand();
    }

    void AES128::keyExpand()
    {
        keyW W[44];

        W[0] = {subKeys[0][0][0], subKeys[0][1][0], subKeys[0][2][0], subKeys[0][3][0]};
        W[1] = {subKeys[0][0][1], subKeys[0][1][1], subKeys[0][2][1], subKeys[0][3][1]};
        W[2] = {subKeys[0][0][2], subKeys[0][1][2], subKeys[0][2][2], subKeys[0][3][2]};
        W[3] = {subKeys[0][0][3], subKeys[0][1][3], subKeys[0][2][3], subKeys[0][3][3]};

        for (int i = 4; i < 44; ++i)
        {
            int j = i / 4 - 1;
            keyW temp = W[i - 1];
            if (i % 4 == 0)
                temp = substitution(sBox, keyW({temp[1], temp[2], temp[3], temp[0]})) + rCon[j];
            W[i] = W[i - 4] + temp;
        }

        for (int i = 1; i < 11; ++i)
            for (int n = 0; n < 4; ++n)
                for (int m = 0; m < 4; ++m)
                    subKeys[i][m][n] = W[i * 4 + n][m];
    }

    template<int M, int N>
    GaloisField2DenseMatrix<8, unsigned char, M, N>
    AES128::substitution(const gf28 s[][16], const GaloisField2DenseMatrix<8, unsigned char, M, N> &b)
    {
        GaloisField2DenseMatrix<8, unsigned char, M, N> ret;
        for (int m = 0; m < M; ++m)
            for (int n = 0; n < N; ++n)
                ret[m][n] = s[b[m][n].getC() >> 4][b[m][n].getC() & 0x0f];
        return ret;
    }

    gf2m44 AES128::encRound(const gf2m44 &d, int j)
    {
        // 第0轮，只做轮密钥加
        if (j == 0)
            return d + subKeys[0];
        gf2m44 ret;
        // 字节替换
        ret = substitution(sBox, d);
        // 行移位
        ret = {ret[0][0], ret[0][1], ret[0][2], ret[0][3],
               ret[1][1], ret[1][2], ret[1][3], ret[1][0],
               ret[2][2], ret[2][3], ret[2][0], ret[2][1],
               ret[3][3], ret[3][0], ret[3][1], ret[3][2]};
        // 列混淆
        if (j != 10)
            ret = colM * ret;
        // 轮密钥加
        ret = ret + subKeys[j];
        return ret;
    }

    ByteBuffer AES128::encryptNoPadding(const ByteBuffer &b, WorkingMode mode)
    {
        ByteBuffer ret;
        ret.resize(b.size());
        for (int k = 0; (k << 4) < b.size(); ++k)
        {
            gf2m44 mData, iv = initVec;
            // 填充明文矩阵
            for (int n = 0; n < 4; ++n)
                for (int m = 0; m < 4; ++m)
                    mData[m][n] = b[(k << 4) + (n << 2) + m];
            // 判断工作模式
            switch (mode)
            {
                case WorkingMode::ECB:
                    break;
                case WorkingMode::CBC:
                    mData = mData + iv;
                    break;
            }
            // 加密
            for (int j = 0; j <= 10; ++j)
                mData = encRound(mData, j);
            // 更新向量
            if (mode == WorkingMode::CBC)
                iv = mData;
            // 填充输出缓冲区
            for (int n = 0; n < 4; ++n)
                for (int m = 0; m < 4; ++m)
                    ret[(k << 4) + (n << 2) + m] = mData[m][n].getC();
        }
        return ret;
    }

    ByteBuffer AES128::decryptNoPadding(const ByteBuffer &b, WorkingMode mode)
    {
        ByteBuffer ret;
        ret.resize(b.size());
        for (int k = 0; (k << 4) < b.size(); ++k)
        {
            gf2m44 mData, iv = initVec;
            // 填充密文矩阵
            for (int n = 0; n < 4; ++n)
                for (int m = 0; m < 4; ++m)
                    mData[m][n] = b[(k << 4) + (n << 2) + m];
            // 备份密文
            gf2m44 cData = mData;
            // 解密
            for (int j = 10; j >= 0; --j)
                mData = decRound(mData, j);
            // 判断模式
            switch (mode)
            {
                case WorkingMode::ECB:
                    break;
                case WorkingMode::CBC:
                    mData = mData + iv;
                    iv = cData;
                    break;
            }
            // 填充输出缓冲区
            for (int n = 0; n < 4; ++n)
                for (int m = 0; m < 4; ++m)
                    ret[(k << 4) + (n << 2) + m] = mData[m][n].getC();
        }
        return ret;
    }

    gf2m44 AES128::decRound(const gf2m44 &d, int j)
    {
        // 逆轮密钥加
        gf2m44 ret = d + subKeys[j];
        // 第0轮加密，直接返回
        if (j == 0)
            return ret;
        // 逆列混淆
        if (j != 10)
            ret = invColM * ret;
        // 逆行移位
        ret = {ret[0][0], ret[0][1], ret[0][2], ret[0][3],
               ret[1][3], ret[1][0], ret[1][1], ret[1][2],
               ret[2][2], ret[2][3], ret[2][0], ret[2][1],
               ret[3][1], ret[3][2], ret[3][3], ret[3][0]};
        // 逆字节替换
        ret = substitution(invSBox, ret);
        return ret;
    }

    ByteBuffer AES128::encrypt(const ByteBuffer &b, PaddingType p, WorkingMode mode)
    {
        ByteBuffer padded = b;
        unsigned char padNum = 16 - (padded.size() % 16);
        switch (p)
        {
            case PaddingType::NOPADDING:
                assert(padNum == 16);
                break;
            case PaddingType::ZEROS:
                while (b.size() % 16 != 0)
                    padded.pushBack(0);
                break;
            case PaddingType::ISO10126:
                for (int i = 0; i < padNum - 1; ++i)
                    padded.pushBack((rng() % 256));
                padded.pushBack(padNum);
                break;
            case PaddingType::PKCS5:
                for (int i = 0; i < padNum; ++i)
                    padded.pushBack(padNum);
                break;
        }
        return encryptNoPadding(padded, mode);
    }

    ByteBuffer AES128::decrypt(const ByteBuffer &b, AES128::PaddingType p, WorkingMode mode)
    {
        ByteBuffer ret = decryptNoPadding(b, mode);
        switch (p)
        {
            case PaddingType::ZEROS:
                // 清除掉所有的末尾0，但是也许会多清掉数据
                while (ret[ret.size() - 1] == 0)
                    ret.popBack();
                break;
            case PaddingType::NOPADDING:
                assert(b.size() % 16 == 0);
                break;
            case PaddingType::ISO10126:
            case PaddingType::PKCS5:
                unsigned char padNum = ret[ret.size() - 1];
                ret.resize(ret.size() - padNum);
                break;
        }
        return ret;
    }

    void AES128::setInitVec(const gf2m44 &b)
    {
        initVec = b;
    }

    void AES128::setInitVec(const gf28 *b)
    {
        for (int i = 0, n = 0; n < 4; ++n)
            for (int m = 0; m < 4; ++m, ++i)
                initVec[m][n] = b[i];
    }

    void AES128::setInitVec(const char *b)
    {
        for (int i = 0, n = 0; n < 4; ++n)
            for (int m = 0; m < 4; ++m, ++i)
                initVec[m][n] = b[i];
    }
} // cryptolab