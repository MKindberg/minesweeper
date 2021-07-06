#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <iostream>

#include "board.hpp"

Board::Board(int width, int height, int mines)
: mWidth(width),
  mHeight(height),
  mMines(mines),
  mBoard(width, height),
  mCover(width, height),
  mNOpen(0),
  mNMarked(0)
{
}

/**
 * \brief Check the value of a square
 *
 * \param[in]   x         The column of the square to check.
 * \param[in]   y         The row of the square to check.
 *
 * \return    The value of of the requested square
 *
 * \retval    'O'   The square is marked as a mine.
 * \retval    ' '   The square is not opened.
 * \retval    'M'   The square contains an opened mine.
 * \retval    <int> The square is adjacent to <int> mines.
 */
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

/**
 * \brief Check how many mines are left.
 *
 * \return The number of mines left to mark.
 */
int Board::mines_left()
{
  return mMines - mNMarked;
}

/**
 * \brief Try to open a square on the board
 *
 * \details Open a square if it's not marked as a mine. If the clicked square
 *          doesn't border a mine all (unmarked) neighbours are opened as well.
 *
 * \note    The mines are added on the first call to open so it will always open a 0.
 *
 * \param[in]   x         The column of the square to open.
 * \param[in]   y         The row of the square to open.
 * \param[out]  changes   A vector of changes that was made to the board by this call.
 *
 * \return The state of the board after this call.
 *
 * \retval Board_state::play  Everything is ok, a new move can be made.
 * \retval Board_state::win   All squares without mines are opened.
 * \retval Board_state::loss  A mine was discovered.
 */
Board_state Board::open(int x, int y, std::vector<Change>& changes)
{
  if(mNOpen == 0) {
    add_mines(x, y);
  }
  if(mCover(x, y) == 2) {
    changes.emplace_back(x, y, 'O');
    return Board_state::play;
  }
  else if(mCover(x, y) == 1) {
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
  }
  else if(is_win()) {
    changes.emplace_back(x, y, mBoard(x, y) + '0');
    return Board_state::win;
  }
  else {
    changes.emplace_back(x, y, mBoard(x, y) + '0');
    // Open all neighbours when a 0 is opened
    if(mBoard(x, y) == 0) {
      return open_adj(x, y, changes);
    }
    return Board_state::play;
  }
}

/**
 * \brief Mark/unmark a square as containing a mine. Open neighbours if square is open.
 *
 * \details If the selected square is open and have the correct number of marked neighbours
 *          is correct all non-marked neighbours will be opened.
 *
 * \param[in]   x         The column of the square to mark/unmark.
 * \param[in]   y         The row of the square to mark/unmark.
 * \param[out]  changes   A vector of changes that was made to the board by this call.
 *
 * \return The state of the board after this call.
 *
 * \retval Board_state::play  Everything is ok, a new move can be made.
 * \retval Board_state::win   All squares without mines are opened.
 * \retval Board_state::loss  A mine was discovered.
 */
Board_state Board::mark(int x, int y, std::vector<Change>& changes)
{
  if(mCover(x, y) == 1) {
    return open(x, y, changes);
  }
  else if(mCover(x, y) == 2) {
    mCover(x, y) = 0;
    --mNMarked;
    changes.emplace_back(x, y, ' ');
  }
  else {
    mCover(x, y) = 2;
    ++mNMarked;
    changes.emplace_back(x, y, 'O');
    if(is_win())
      return Board_state::win;
  }
  return Board_state::play;
}

/**
 * \brief Open/mark a random square.
 *
 * \details Select a random square and mark or open it depending on if it contains a mine or not.
 *
 * \param[out]  changes   A vector of changes that was made to the board by this call.
 *
 * \return The state of the board after this call.
 *
 * \retval Board_state::play  Everything is ok, a new move can be made.
 * \retval Board_state::win   All squares without mines are opened.
 */
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

/**
* Check if all non-mine squares have been opened.
*/
bool Board::is_win()
{
  return mMines + mNOpen == mWidth * mHeight;
}

/**
* Count number of marked adjacent squares.
*/
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

/**
* Open all non-marked adjacent squares.
*/
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

/**
* Add mMines mine to the board, the square (x, y) is guaranteed to be a 0.
*/
void Board::add_mines(int x, int y)
{
  srand((unsigned) time(NULL));

  for(int i = 0; i < mMines; ++i) {
    int xx = rand() % mWidth;
    int yy = rand() % mHeight;
    if(mBoard(xx, yy) == 0 && !(abs(x-xx) < 2 && abs(y-yy) < 2))
      mBoard(xx, yy) = -1;
    else
      --i;
  }

  count_mines();
}

/**
* Count number of mines currently placed on the board.
*/
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

/**
* Print an opened version of the board, mines are -1.
*/
void Board::print()
{
  for(int i = 0; i < mHeight; ++i) {
    for(int j = 0; j < mWidth; ++j)
      printf("%2d ", mBoard(j, i));
    printf("\n");
  }
}

