//
// Created by helmholtz on 2022/9/26.
//

#ifndef CRYPTOLAB_GALOISFIELD2_H
#define CRYPTOLAB_GALOISFIELD2_H

#include <iostream>
#include <vector>

namespace cryptolab
{
    // W大于此值就不设置乘法表了
    constexpr unsigned long mulLookUpTableSizeLimit = 8;

    template<int W, typename T>    // GF(2^W)的有限域，每个数字使用T类型存储
    class GaloisField2
    {
    private:
        T c;                                            // 多项式的系数
        static T xwom;                                  // x^W / m(x)，其中m(x)是素多项式
        static constexpr bool TgW = (sizeof(T) > W);    // T的长度大于W，需要考虑最高位
        static std::vector<std::vector<T>> lookUpTable; // W不是很大的时候，我们当然可以查表
        static bool lookUpTableValid;
    public:
        GaloisField2<W, T>() : c(0) {};
        /**
         * 使用整数i初始化GF(2^n)中的多项式
         * @param i 要初始化的整数，不要大于GF(2^n)对应的整数范围
         */
        GaloisField2<W, T>(T i) : c(i) {};

        static void setMx(T m);

        static void setXwom(T m);

        static T getMx();

        static void initLookUpTable();

        T getC() const;

        void self_double();

        GaloisField2<W, T> mul2();

        GaloisField2<W, T> operator+(const GaloisField2<W, T> &b) const;

        GaloisField2<W, T> operator*(const GaloisField2<W, T> &b) const;
    };

    template<int W, typename T>
    std::ostream &operator<<(std::ostream &os, const GaloisField2<W, T> &gf)
    {
        os << +gf.getC();   // '+'保证输出成数值而不是字符，因为常用的GF(2^8)用unsigned char存储，所以很有用
        return os;
    }

    template<int W, typename T>
    GaloisField2<W, T> operator*(T a, const GaloisField2<W, T> &b)
    {
        return GaloisField2<W, T>(a) * b;
    }

    template<int W, typename T>
    GaloisField2<W, T> operator*(int a, const GaloisField2<W, T> &b)
    {
        return GaloisField2<W, T>(a) * b;
    }

    template<int W, typename T>
    GaloisField2<W, T> operator+(T a, const GaloisField2<W, T> &b)
    {
        return GaloisField2<W, T>(a) + b;
    }

    template<int W, typename T>
    GaloisField2<W, T> operator+(int a, const GaloisField2<W, T> &b)
    {
        return GaloisField2<W, T>(a) + b;
    }

    template<int W, typename T>
    T GaloisField2<W, T>::xwom = 0;

    template<int W, typename T>
    bool GaloisField2<W, T>::lookUpTableValid = false;

    template<int W, typename T>
    std::vector<std::vector<T>> GaloisField2<W, T>::lookUpTable = std::vector<std::vector<T>>();

    /**
     * 设置GF(2^W)中的不可约多项式m(x)，该多项式对每个模板类型W是唯一的，而且是静态变量，所以只用设置一次
     * m(x)的次数必须为W，且最高位需要省略，例：m(x)=x^4+x+1，则T一定为4且参数m为0b11.
     * @tparam W GF(2^W)
     * @tparam T 系数存储的类型
     * @param m 不可约多项式m(x)的按位表示，x^W项省略!!!
     */
    template<int W, typename T>
    void GaloisField2<W, T>::setMx(T m)
    {
        // 计算x^W / m(x)的余数，因为最高位省略了，所以只赋值就可以了
        xwom = m;
        // 重算（也许是第一次算）查找表
        initLookUpTable();
    }

    /**
     * 获取x^W / m(x)的余多项式
     * @tparam W GF(2^W)
     * @tparam T 系数存储的类型
     * @return x^W / m(x)的余多项式
     */
    template<int W, typename T>
    T GaloisField2<W, T>::getMx()
    {
        return xwom;
    }

    /**
     * 直接设置x^W / m(x)的余多项式
     * @tparam W GF(2^W)
     * @param m 余多项式的按位表示
     */
    template<int W, typename T>
    void GaloisField2<W, T>::setXwom(T m)
    {
        xwom = m;
        initLookUpTable();
    }

    template<int W, typename T>
    GaloisField2<W, T> GaloisField2<W, T>::mul2()
    {
        if (lookUpTableValid)
            return GaloisField2<W, T>(lookUpTable[this->c][2]);
        T retc = c;
        // 如果最高位为1
        if (retc & (1 << (W - 1)))
            retc = (retc << 1) ^ xwom;
        else
            retc <<= 1;
        // 如果最高位大于W就抹掉
        if (TgW)
            retc ^= (1 << W);
        return GaloisField2<W, T>(retc);
    }

    template<int W, typename T>
    GaloisField2<W, T> GaloisField2<W, T>::operator+(const GaloisField2<W, T> &b) const
    {
        return GaloisField2<W, T>(c ^ b.c);
    }

    template<int W, typename T>
    GaloisField2<W, T> GaloisField2<W, T>::operator*(const GaloisField2<W, T> &b) const
    {
        if (lookUpTableValid)
            return GaloisField2<W, T>(lookUpTable[this->c][b.c]);
        // 循环乘
        GaloisField2<W, T> ret(0);
        T bc = b.c;
        constexpr T highestBitMask = 1 << (W - 1);
        for (int i = 0; i < W; ++i)
        {
            ret.self_double();
            // 从最高位开始循环乘
            if (bc & highestBitMask)
                ret = ret + c;
            bc <<= 1;
        }
        return ret;
    }

    /**
     * 自乘2
     * @tparam W GF(2^W)
     */
    template<int W, typename T>
    void GaloisField2<W, T>::self_double()
    {
        // 最高位为1
        if (c & (1 << (W - 1)))
            c = (c << 1) ^ xwom;
        else
            c <<= 1;
        // 去掉最高位
        if (TgW)
        {
            c ^= (1 << W);
        }
    }

    template<int W, typename T>
    T GaloisField2<W, T>::getC() const
    {
        return c;
    }

    template<int W, typename T>
    void GaloisField2<W, T>::initLookUpTable()
    {
        // 初始化查找表
        if (W <= mulLookUpTableSizeLimit)
        {
            lookUpTable.resize(1 << W);
            for (int tbi = 0; tbi < (1 << W); ++tbi)
            {
                lookUpTable[tbi].resize(1 << W);
                for (int tbj = 0; tbj < (1 << W); ++tbj)
                {
                    lookUpTable[tbi][tbj] = (GaloisField2<W, T>(tbi) * GaloisField2<W, T>(tbj)).getC();
                }
            }
            lookUpTableValid = true;
        }
    }

    using gf28 = GaloisField2<8, unsigned char>;

} // cryptolab

#endif //CRYPTOLAB_GALOISFIELD2_H
