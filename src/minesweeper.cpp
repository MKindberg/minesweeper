#include <vector>
#include <iterator>
#include <iostream>
#include <functional>

#include <gtkmm/application.h>
#include <gtkmm/applicationwindow.h>
#include <gtkmm/togglebutton.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <glibmm/ustring.h>

#include "board.hpp"

using std::vector;

class MinesweeperWindow : public Gtk::ApplicationWindow {
  std::vector<Gtk::ToggleButton> btns;
  Gtk::Grid mGrid;
  Gtk::RadioButton mRbMark, mRbOpen;
  Gtk::Button mHint, mReset;
  Gtk::Label mStatus;
  int mHeight;
  int mWidth;
  int mMines;
  Board mBoard;
  bool mDisableClick;
public:
    MinesweeperWindow(int width = 10, int height = 10, int mines = 10)
    : mBoard(width, height, mines),
      mWidth(width),
      mHeight(height),
      mMines(mines),
      mRbMark("Mark"),
      mRbOpen("Open"),
      mHint("Hint"),
      mReset("Reset"),
      mStatus(""),
      mDisableClick(false)
    {

      mRbMark.join_group(mRbOpen);
      mGrid.attach(mRbOpen, 0, 0);
      mGrid.attach(mRbMark, 1, 0);

      mGrid.attach(mHint, 2, 0);
      mHint.signal_clicked().connect([this] {
        std::vector<Change> changes;
        if(mBoard.hint(changes) == Board_state::win)
          win();
        update(changes);
      });

      mGrid.attach(mReset, 3, 0, 2, 1);
      mReset.signal_clicked().connect([this] { reset(); });

      mGrid.attach(mStatus, 5, 0, 2, 1);
      mStatus.set_label(std::to_string(mBoard.mines_left()) + " mines left");

      for(int i = 0; i < width * height; ++i)
        btns.emplace_back(" ");

      for(int row = 0; row < height; ++row)
        for(int col = 0; col < width; ++col) {
          int n = row*width + col;
          mGrid.attach(btns[n], col, row+1);

          btns[n].signal_clicked().connect(on_click(col, row));
        }

      add(mGrid);
      show_all();
    }
  std::function<void()> on_click(int x, int y)
  {
    return [this, x, y] {
      if(mDisableClick)
        return;
      int n = y*mWidth + x;
      std::vector<Change> changes;
      Board_state res;

      if (mRbMark.get_active()) {
        res = mBoard.mark(x, y, changes);
      }
      else {
        res = mBoard.open(x, y, changes);
      }
      update(changes);

      if(res == Board_state::win) {
        win();
      }
      else if(res == Board_state::loss) {
        loss();
      }
    };
  }

  void win()
  {
    mStatus.set_label("Win!!");
    std::cout << "Win!! \n";
    mDisableClick = true;
  }

  void loss()
  {
    mStatus.set_label("Loss!!");
    std::cout << "Loss!! \n";
    mDisableClick = true;
  }

  void update(std::vector<Change> changes)
  {
    for(Change c: changes) {
      int n = c.y*mWidth + c.x;
      if(c.val == 'O' || c.val == ' ') {
        mDisableClick = true;
        btns[n].set_active(false);
        mDisableClick = false;
      }
      else {
        mDisableClick = true;
        btns[n].set_active(true);
        mDisableClick = false;
      }
      btns[n].set_label(Glib::ustring(1, c.val));
    }
    mStatus.set_label(std::to_string(mBoard.mines_left()) + " mines left");
  }
  void reset()
  {
    mDisableClick = true;
    for(int i = 0; i < btns.size(); ++i) {
      btns[i].set_label(" ");
      btns[i].set_active(false);
    }
    mBoard = Board(mWidth, mHeight, mMines);
    mStatus.set_label(std::to_string(mBoard.mines_left()) + " mines left");
    mDisableClick = false;
  }
};


int main(int argc, char* argv[])
{
  auto app = Gtk::Application::create("mkindberg.minesweeper");
  int width = 10, height = 10, mines = 15;
  if(argc == 2) {
    std::cout << argv[0] << ' ' << argv[1] << '\n';
    mines = atoi(argv[1]);
    std::cout << "mines: " << mines << '\n';
  }
  else if(argc == 3) {
    width = atoi(argv[1]);
    height = atoi(argv[2]);
  }
  else if(argc == 4) {
    width = atoi(argv[1]);
    height = atoi(argv[2]);
    mines = atoi(argv[3]);
  }
  MinesweeperWindow mw(width, height, mines);
  return app->run(mw);

  return 0;
}
