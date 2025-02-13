// #include "snake.h"
#include <iostream> // required for cout and cin
// Unix
#include <cstdlib>   // for system()
#include <fcntl.h>   // Include this header for fcntl() and related macros
#include <fstream>   // Stream class to both read and write from/to files.
#include <termios.h> // for _kbhit and _getch
#include <unistd.h>  // <-- if use unix
// WINDOWS
// #include <windows.h> // <-- if use windows
// #include <conio.h> // required for _kbhit() and _getch()

void clear_screen1() {
  printf("\033[2J"     // clear the screen
         "\033[1;1H"); // move cursor home
}

void clear_screen2() {
  system("clear"); // Clears the screen in Unix-like systems
}

void print(std::string text) { std::cout << text << std::endl; }

void hello() {
  clear_screen2();
  print("# --- HELLO IN: --- #");
  print("");
  sleep(1);
  print("   _____ _   _          _  ________  ");
  print("  / ____| \\ | |   /\\   | |/ /  ____| ");
  print(" | (___ |  \\| |  /  \\  | ' /| |__    ");
  print("  \\___ \\| . ` | / /\\ \\ |  < |  __|   ");
  print("  ____) | |\\  |/ ____ \\| . \\| |____  ");
  print(" |_____/|_| \\_/_/    \\_\\_|\\_\\______| ");
  sleep(1); // Sleep for 1 second
  print("");
  print("Skip into y/n");
  std::string answer;
  std::cin >> answer;
  if (answer != "y") {
    print("go up 'k' or 'w'");
    sleep(2);
    print("");
    sleep(1);
    print("go down 'j' or 's'");
    sleep(2);
    print("");
    sleep(1);
    print("go left 'h' or 'a'");
    sleep(2);
    print("");
    sleep(1);
    print("go right 'l' or 'd'");
    sleep(2);
    print("");
    sleep(1);
  }
}

int _kbhit() {
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF) {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

// Function to get a single character
int _getch() {
  struct termios oldt, newt;
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
}

bool gameOver;
bool vim_keybind;
const int height = 20;
const int width = 20;

int x, y, fruitX, fruitY, score, record, last_score;

int tailX[100], tailY[100], nTail;

enum eDirection {
  STOP = 0,
  UP,
  DOWN,
  LEFT,
  RIGHT
}; // user-defined type that consists of a set of named integral constants
eDirection dir;

void last_score_load() {
  // Read value
  std::ifstream file("./data/last-score.txt");
  file >> last_score;
  file.close();
}

void last_score_save() {
  std::ofstream file("./data/last-score.txt");
  file << score;
  file.close();
}

void record_load() {
  // Read value
  std::ifstream file("./data/record.txt");
  file >> record;
  file.close();
  last_score_load();
}

void record_check() {
  // Write value
  if (score > record) {
    std::ofstream file("./data/record.txt");
    file << score << std::endl;
    file.close();
  }
  last_score_save();
}

void Setup() {
  gameOver = false;
  x = width / 2;
  y = height / 2;
  fruitX = rand() % width;
  fruitY = rand() % height;
  dir = STOP;
  vim_keybind = true;
}
void Draw() {
  // clear_screen1();

  clear_screen2();
  for (int i = 0; i < width; i++) {
    std::cout << "#";
  }
  std::cout << std::endl;

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      if (j == 0 || j == width - 1) {
        std::cout << "#";
      }
      // Snake
      else if (i == y && j == x) {
        std::cout << "O";
      }
      // Fruit
      else if (i == fruitY && j == fruitX) {
        std::cout << "F";
      } else {
        bool print = false;

        for (int k = 0; k < nTail; k++) {
          if (i == tailY[k] && j == tailX[k]) {
            std::cout << "o";
            print = true;
          }
        }
        if (!print)
          std::cout << " ";
      }
    }
    std::cout << std::endl;
  }

  for (int i = 0; i < width; i++) {
    std::cout << "#";
  }
  std::cout << std::endl;
  std::cout << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "Score: " << score << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "Last Score: " << last_score << std::endl;
  std::cout << "-------------------------" << std::endl;
  std::cout << "Record: " << record << std::endl;
  std::cout << "-------------------------" << std::endl;
}
void Input() {
  if (_kbhit()) {
    switch (_getch()) {
    case 'w':
    case 'k':
    case 65:
      dir = UP;
      break;
    case 's':
    case 'j':
    case 66:
      dir = DOWN;
      break;
    case 'a':
    case 'h':
    case 68:
      dir = LEFT;
      break;
    case 'd':
    case 'l':
    case 67:
      dir = RIGHT;
      break;
    case 'q':
      gameOver = true;
    default:
      break;
    }
  }
}
void Logic() {

  int prevX = tailX[0];
  int prevY = tailY[0];
  tailX[0] = x;
  tailY[0] = y;
  int prev2X, prev2Y;

  for (int i = 1; i < nTail; i++) {
    prev2X = tailX[i];
    prev2Y = tailY[i];
    tailX[i] = prevX;
    tailY[i] = prevY;
    prevX = prev2X;
    prevY = prev2Y;
  }

  switch (dir) {
  case UP:
    y--;
    break;
  case DOWN:
    y++;
    break;
  case LEFT:
    x--;
    break;
  case RIGHT:
    x++;
    break;
  default:
    break;
  }

  if (x < 0 || x > width || y < 0 || y > height) {
    gameOver = true;
  }

  // Go on tail = kill
  for (int i = 0; i < nTail; i++) {
    if (x == tailX[i] && y == tailY[i]) {
      gameOver = true;
    }
  }

  if (x == fruitX && y == fruitY) {
    score += 10;
    fruitX = rand() % (width - 1) + 1;
    fruitY = rand() % (height - 1) + 1;
    nTail++;
  }
}

void test() {
  std::cout << "" << std::endl;
  std::string buja = "f";
}

int main() {
  hello();
  Setup();
  record_load();
  while (!gameOver) {
    Draw();
    Input();
    Logic();
    // sleep(1); // for seconds
    usleep(200000); // for microseconds
  }
  record_check();
  return 0;
}
