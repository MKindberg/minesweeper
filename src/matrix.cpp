#include "matrix.hpp"

Matrix::Matrix(int cols, int rows)
: mCols(cols),
  mRows(rows),
  mData(cols*rows, 0)
{
}

int& Matrix::operator()(int x, int y)
{
  return mData[x*mRows+y];
}
int Matrix::operator()(int x, int y) const
{
  return mData[x*mRows+y];
}
