#include <vector>

template <class T>
class Matrix {
  public:
    Matrix(int cols, int rows)
    : mCols(cols),
      mRows(rows),
      mData(cols*rows, 0)
    {
    }

  T& operator()(int x, int y)
  {
    return mData[x*mRows+y];
  }

  T operator()(int x, int y) const
  {
    return mData[x*mRows+y];
  }

  private:
    int mCols;
    int mRows;
    std::vector<T> mData;
};


