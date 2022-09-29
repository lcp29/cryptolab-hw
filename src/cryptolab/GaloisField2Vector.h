//
// Created by helmholtz on 2022/9/28.
//

#ifndef CRYPTOLAB_GALOISFIELD2VECTOR_H
#define CRYPTOLAB_GALOISFIELD2VECTOR_H

#include "GaloisField2DenseMatrix.h"

namespace cryptolab
{
    template<int W, typename T, int N>
    class GaloisField2Vector : public GaloisField2DenseMatrix<W, T, 1, N>
    {
    public:
        GaloisField2Vector<W, T, N>() = default;

        GaloisField2Vector<W, T, N>(const GaloisField2Vector<W, T, N> &b) : GaloisField2DenseMatrix<W, T, 1, N>(b) {}

        GaloisField2Vector<W, T, N>(const GaloisField2DenseMatrix<W, T, 1, N> &b) : GaloisField2DenseMatrix<W, T, 1, N>(b) {}

        GaloisField2Vector<W, T, N>(const std::initializer_list<GaloisField2<W, T>> &b) :

        GaloisField2DenseMatrix<W, T, 1, N>(b) {}

        GaloisField2<W, T>& operator[](unsigned int idx);
        const GaloisField2<W, T>& operator[](unsigned int idx) const;
    };

    template<int W, typename T, int N>
    GaloisField2<W, T> &GaloisField2Vector<W, T, N>::operator[](unsigned int idx)
    {
        return this->dat[0][idx];
    }

    template<int W, typename T, int N>
    const GaloisField2<W, T> &GaloisField2Vector<W, T, N>::operator[](unsigned int idx) const
    {
        return this->dat[0][idx];
    }

    template<int W, typename T, int N>
    GaloisField2Vector<W, T, N> operator*(const GaloisField2Vector<W, T, N> &a,
                                          const GaloisField2Vector<W, T, N> &b)
    {
        GaloisField2Vector <W, T, N> ret;
        for (int n = 0; n < N; ++n)
            ret[n] = a[n] * b[n];
        return ret;
    }

} // cryptolab

#endif //CRYPTOLAB_GALOISFIELD2VECTOR_H
