//
// Created by helmholtz on 2022/9/28.
//

#ifndef CRYPTOLAB_GALOISFIELD2DENSEMATRIX_H
#define CRYPTOLAB_GALOISFIELD2DENSEMATRIX_H

#include "GaloisField2.h"

namespace cryptolab
{
    template<int W, typename T, int M, int N>
    class GaloisField2DenseMatrix
    {
    protected:
        GaloisField2<W, T> dat[M][N];
    public:
        GaloisField2DenseMatrix<W, T, M, N>() = default;

        GaloisField2DenseMatrix<W, T, M, N>(const GaloisField2DenseMatrix<W, T, M, N> &b)
        {
            for (int m = 0; m < M; ++m)
                for (int n = 0; n < N; ++n)
                    dat[m][n] = b[m][n];
        }

        GaloisField2DenseMatrix<W, T, M, N>(const std::initializer_list<GaloisField2<W, T>> &b)
        {
            int m = 0, n = 0;
            for (auto i: b)
            {
                dat[m][n++] = i;
                if (n >= N)
                {
                    n = 0;
                    if (++m >= M)
                        break;
                }
            }
        }

        virtual GaloisField2<W, T> *operator[](unsigned long idx);

        virtual const GaloisField2<W, T> *operator[](unsigned long idx) const;

        GaloisField2DenseMatrix<W, T, M, N> operator+(const GaloisField2DenseMatrix<W, T, M, N> &b) const;

        void clear();
    };

    template<int W, typename T, int M, int N>
    GaloisField2<W, T> *GaloisField2DenseMatrix<W, T, M, N>::operator[](unsigned long idx)
    {
        return dat[idx];
    }

    template<int W, typename T, int M, int N>
    GaloisField2DenseMatrix<W, T, M, N>
    GaloisField2DenseMatrix<W, T, M, N>::operator+(const GaloisField2DenseMatrix<W, T, M, N> &b) const
    {
        GaloisField2DenseMatrix<W, T, M, N> ret;
        for (int m = 0; m < M; ++m)
            for (int n = 0; n < N; ++n)
                ret[m][n] = dat[m][n] + b[m][n];
        return ret;
    }

    template<int W, typename T, int M, int N>
    const GaloisField2<W, T> *GaloisField2DenseMatrix<W, T, M, N>::operator[](unsigned long idx) const
    {
        return dat[idx];
    }

    template<int W, typename T, int M, int N>
    void GaloisField2DenseMatrix<W, T, M, N>::clear()
    {
        for (int m = 0; m < M; ++m)
            for (int n = 0; n < N; ++n)
                dat[m][n] = 0;
    }

    template<int W, typename T, int M, int N, int R>
    GaloisField2DenseMatrix<W, T, M, N> operator*(const GaloisField2DenseMatrix<W, T, M, R> &a,
                                                  const GaloisField2DenseMatrix<W, T, R, N> &b)
    {
        GaloisField2DenseMatrix<W, T, M, N> ret;
        for (int m = 0; m < M; ++m)
        {
            for (int n = 0; n < N; ++n)
            {
                ret[m][n] = 0;
                for (int r = 0; r < R; ++r)
                {
                    ret[m][n] = ret[m][n] + a[m][r] * b[r][n];
                }
            }
        }
        return ret;
    }

    template<int W, typename T, int M, int N>
    std::ostream &operator<<(std::ostream &os, const GaloisField2DenseMatrix<W, T, M, N> &a)
    {
        for (int m = 0; m < M; ++m)
        {
            for (int n = 0; n < N; ++n)
            {
                os << a[m][n] << " ";
            }
            os << "\n";
        }
        return os;
    }

    using aes128key = GaloisField2DenseMatrix<8, unsigned char, 4, 4>;
    using gf2m44 = GaloisField2DenseMatrix<8, unsigned char, 4, 4>;

} // cryptolab

#endif //CRYPTOLAB_GALOISFIELD2DENSEMATRIX_H
