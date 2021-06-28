#include <vector>

#include "matrix.hpp"

struct Change {
  int x, y;
  char val;

  Change(int x, int y, char val)
  : x(x),
    y(y),
    val(val)
  {}
};

enum class Board_state
{
  play,
  loss,
  win,
};

class Board {
  public:
    Board(int width, int height, int mines);
    char operator()(int x, int y) const;
    void print();
    Board_state open(int x, int y, std::vector<Change>& changes);
    Board_state mark(int x, int y, std::vector<Change>& changes);
    Board_state hint(std::vector<Change>& changes);
  private:
    void add_mines(int x, int y);
    void count_mines();
    int marked_adj(int x, int y);
    Board_state open_adj(int x, int y, std::vector<Change>& changes);
    bool is_win();
    int mWidth;
    int mHeight;
    int mMines;
    Matrix mBoard;
    Matrix mCover;
    int mNOpen = 0;
};

