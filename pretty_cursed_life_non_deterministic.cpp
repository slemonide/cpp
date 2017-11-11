#include <vector>
#include <ncurses.h>
#include <iterator>
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <ctime>

// Curses stuff

int r,c,  // current row and column (upper-left is (0,0))
    nrows,  // number of rows in window
    ncols;  // number of columns in window

WINDOW *wnd;

// Draws dc at (x,y)
void draw(int x, int y, char dc) {
  move(y,x);
  delch();  insch(dc);
}

// ================
// CONSTANTS

// How much larger the world is compared to the screen size
const int SIZE = 1;


// ================
// DATA DEFINITIONS
std::vector<bool> field;
int width = 0;
int height = 0;

// Origin
int x0 = 0;
int y0 = 0;

bool running = true;
int generation = 0;

// UTILS

void clearScreen() {
#ifdef WINDOWS
  std::system("cls");
#else
  // Assume POSIX
  std::system("clear");
#endif
}

// FUNCTIONS
void initField() {
  std::srand(std::time(0));
  for (int i = 0; i < field.size(); i++) {
    field[i] = (std::rand() % 2 == 0);
  }
}

// IO

void renderDelimiter() {
  for (int i = 0; i < 80; i++) {
    std::cout << "#";
  }

  std::cout << "\n";
}

void renderGeneration() {
  std::cout << "Generation: " << generation << "\n";
}

void initCurses() {
  wnd = initscr();  // curses call to initialize window
  if(has_colors() == false) {
    endwin();
    printf("Your terminal does not support color\n");
    exit(1);
  }
  start_color();
  init_pair(1, COLOR_RED, COLOR_GREEN);
  attron(COLOR_PAIR(1));

  cbreak();  // curses call to set no waiting for Enter key
  nodelay(wnd, true);
  //timeout(1); // keyboard support
  noecho();  // curses call to set no echoing
  getmaxyx(wnd,nrows,ncols);  // curses call to find size of window

  width = ncols * SIZE;
  height = nrows * SIZE;
  
  field.resize(width * height);
}


void renderField() {
  erase();

  for (int x = 0; x < ncols; x++) {
    for (int y = 0; y < nrows; y++) {
      int i = ((y + y0) * width + (x + x0)) % field.size();
      if (field.at(i)) {
        draw(x, y, '#');
      }
    }
  }

  refresh();
}

void killCurses() {
  endwin();  // curses call to restore the original window and leave
}

void stopProgram(int signal) {
  running = false;
}

// Field updates

// Produce the number of alive neighbours of cell with the given index
int neighboursSum(int i) {
  int sum = 0;

  if (field[(i + 1) % field.size()]) sum++;
  if (field[(i - 1 + field.size()) % field.size()]) sum++;
  if (field[(i + width) % field.size()]) sum++;  
  if (field[(i + width + 1) % field.size()]) sum++;
  if (field[(i + width - 1) % field.size()]) sum++;
  if (field[(i - width +  field.size()) % field.size()]) sum++;  
  if (field[(i - width + 1 + field.size()) % field.size()]) sum++;
  if (field[(i - width - 1 + field.size()) % field.size()]) sum++;

  if (std::rand() % 997 == 0) {
    sum = sum + 1;
  }

  return sum;
}

// Produce the next state of the cell at the given index
bool generateNextState(int i) {
  return (neighboursSum(i) == 2 && field[i]) || neighboursSum(i) == 3;
}

void updateField() {
  generation++;

  bool newField [field.size()];
  for (int i = 0; i < field.size(); i++) {
    newField[i] = generateNextState(i);
  }

  for (int i = 0; i < field.size(); i++) {
    field.at(i) = newField[i];
  }  
}

void handleInput() {
  char d = getch();

  if (d == 'w') {
    y0--;
  } else if (d == 's') {
    y0++;
  } else if (d == 'd') {
    x0++;
  } else if (d == 'a') {
    x0--;
  }
}

int main() {
  // Stop on SIGINT
  std::signal(SIGINT, stopProgram);

  initCurses(); // needs to go before initField so that we know field size
  initField();

  while (running) {
    renderField();
    updateField();
    handleInput();
  }

  killCurses();
}

