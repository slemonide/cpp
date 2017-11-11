#include <iterator>
#include <cstdlib>
#include <unistd.h>
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <ctime>


// CONSTANTS
const int size = 1000;

// DATA DEFINITIONS
bool field [size * size];
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
  for (int i = 0; i < size * size; i++) {
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

void renderField() {
  renderDelimiter();
  clearScreen();
  renderGeneration();
  renderDelimiter();

  for (int i = 0; i < size * size; i++) {
    if (field[i]) {
      std::cout << "* ";
    } else {
      std::cout << "  ";
    }

    if (i % size == 0) {
      std::cout << "\n";
    }
  }

  std::cout << "\n";
}

void stopProgram(int signal) {
  running = false;
}

// Field updates

// Produce the number of alive neighbours of cell with the given index
int neighboursSum(int i) {
  int sum = 0;

  if (field[(i + 1) % (size * size)]) sum++;
  if (field[(i - 1 + size * size) % (size * size)]) sum++;
  if (field[(i + size) % (size * size)]) sum++;  
  if (field[(i + size + 1) % (size * size)]) sum++;
  if (field[(i + size - 1) % (size * size)]) sum++;
  if (field[(i - size + size * size) % (size * size)]) sum++;  
  if (field[(i - size + 1 + size * size) % (size * size)]) sum++;
  if (field[(i - size - 1 + size * size) % (size * size)]) sum++;

  return sum;
}

// Produce the next state of the cell at the given index
bool generateNextState(int i) {
  return (neighboursSum(i) == 2 && field[i]) || neighboursSum(i) == 3;
}

void updateField() {
  generation++;

  bool newField [size * size];
  for (int i = 0; i < size * size; i++) {
    newField[i] = generateNextState(i);
  }

  for (int i = 0; i < size * size; i++) {
    field[i] = newField[i];
  }  
}


int main() {
  // Stop on SIGINT
  std::signal(SIGINT, stopProgram);

  initField();

  while (running) {
    renderField();
    updateField();
    usleep(500000);
  }
}

