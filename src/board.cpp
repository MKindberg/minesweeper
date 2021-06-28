#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>

#include "board.hpp"

Board::Board(int width = 20, int height = 20, int mines = 10)
: mWidth(width),
  mHeight(height),
  mMines(mines),
  mBoard(width, height),
  mCover(width, height),
  mNOpen(0),
  mNMarked(0)
{
}

char Board::operator()(int x, int y) const
{
  if(mCover(x, y) == 2)
    return 'O';
  else if(mCover(x, y) == 0)
    return ' ';
  else if(mBoard(x, y) == -1)
    return 'M';
  return mBoard(x, y) + '0';
}

bool Board::is_win()
{
  return mMines + mNOpen == mWidth * mHeight;
}

int Board::mines_left()
{
  return mMines - mNMarked;
}

Board_state Board::open(int x, int y, std::vector<Change>& changes)
{
  if(mNOpen == 0) {
    add_mines(x, y);
    print();
  }
  if(mCover(x, y) == 2) {
    changes.emplace_back(x, y, 'O');
    return Board_state::play;
  } else if(mCover(x, y) == 1) {
    changes.emplace_back(x, y, mBoard(x, y) + '0');
    if(marked_adj(x, y) == mBoard(x, y))
      return open_adj(x, y, changes);
    return Board_state::play;
  }
  mCover(x, y) = 1;
  ++mNOpen;
  if(mBoard(x, y) == -1) {
    changes.emplace_back(x, y, 'M');
    return Board_state::loss;
  } else if(is_win()) {
    changes.emplace_back(x, y, mBoard(x, y) + '0');
    return Board_state::win;
  } else {
    changes.emplace_back(x, y, mBoard(x, y) + '0');
    // Open all neighbours when a 0 is opened
    if(mBoard(x, y) == 0) {
      return open_adj(x, y, changes);
    }
    return Board_state::play;
  }
}

Board_state Board::mark(int x, int y, std::vector<Change>& changes)
{
  if(mCover(x, y) == 1) {
    return open(x, y, changes);
  }
  else if(mCover(x, y) == 2) {
    mCover(x, y) = 0;
    --mNMarked;
    changes.emplace_back(x, y, ' ');
  } else {
    mCover(x, y) = 2;
    ++mNMarked;
    changes.emplace_back(x, y, 'O');
    if(is_win())
      return Board_state::win;
  }
  return Board_state::play;
}

Board_state Board::hint(std::vector<Change>& changes)
{
  srand((unsigned) time(NULL));
  int x, y;
  do {
    x = rand() % mWidth;
    y = rand() % mHeight;
  } while(mCover(x, y) != 0);

  if(mBoard(x, y) == -1)
    return mark(x, y, changes);
  return open(x, y, changes);
}

/*************** Private ***************/

int Board::marked_adj(int x, int y)
{
  int marked = 0;
  for(int xx = -1; xx < 2; ++xx)
    for(int yy = -1; yy < 2; ++yy)
      if(!(xx == 0 && yy == 0)
            && x+xx >= 0
            && y+yy >= 0
            && x+xx < mWidth
            && y+yy < mHeight)
        if(mCover(x+xx, y+yy) == 2)
          ++marked;
  return marked;
}
Board_state Board::open_adj(int x, int y, std::vector<Change>& changes)
{
  for(int xx = -1; xx < 2; ++xx)
    for(int yy = -1; yy < 2; ++yy)
      if(x+xx >= 0
            && y+yy >= 0
            && x+xx < mWidth
            && y+yy < mHeight
            && mCover(x+xx, y+yy) == 0) {
        Board_state b = open(x+xx, y+yy, changes);
        if(b != Board_state::play)
          return b;
      }
  return Board_state::play;
}


void Board::add_mines(int x, int y)
{
  srand((unsigned) time(NULL));

  for(int i = 0; i < mMines; ++i) {
    int xx = rand() % mWidth;
    int yy = rand() % mHeight;
    if(mBoard(xx, yy) == 0 && !(x == xx && y == yy))
      mBoard(xx, yy) = -1;
    else
      --i;
  }

  count_mines();
}

void Board::count_mines()
{
  for(int x = 0; x < mWidth; ++x)
    for(int y = 0; y < mHeight; ++y)
      if(mBoard(x, y) != -1)
        for(int i = std::max(0, x-1); i < std::min(mWidth, x+2); ++i)
          for(int j = std::max(0, y-1); j < std::min(mHeight, y+2); ++j)
            if(mBoard(i, j) == -1)
              ++mBoard(x, y);
}
void Board::print()
{
  for(int i = 0; i < mHeight; ++i) {
    for(int j = 0; j < mWidth; ++j)
      printf("%2d ", mBoard(j, i));
    printf("\n");
  }
}

