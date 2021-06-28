#include <vector>

class Matrix {
  public:
    Matrix(int cols, int rows);
    int& operator()(int x, int y);
    int operator()(int x, int y) const;
    void reset();
  private:
    int mCols;
    int mRows;
    std::vector<int> mData;
};


