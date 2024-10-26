#include "dijkstra.h"
#include "raylib.h"
#include <stdio.h>

typedef struct {
  const int width;
  const int height;
  const int scale;
  const int fps;
  const char *title;
} Window;

typedef struct {
  Pos pos;
} Enemy;
void movement_logic(Pos *pos) {
  switch (GetKeyPressed()) {
  case KEY_D:
    pos->y++;
    break;
  case KEY_A:
    pos->y--;
    break;
  case KEY_W:
    pos->x--;
    break;
  case KEY_S:
    pos->x++;
    break;
  }
}
int main(void) {
  static const Window window = {400, 400, 40, 60, "pac-man"};
  InitWindow(window.width, window.height, window.title);
  SetTargetFPS(window.fps);

  static const int row = window.width / window.scale;
  static const int col = window.height / window.scale;

  static Enemy enemy = {0, 0};

  int *grid = (int *)malloc((row * col) * sizeof(int));
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      grid[(i * row + j)] = 1;
    }
  }

  static const int new_row = row * col;
  static const int new_col = row * col;

  int *adjacency_matrix = (int *)calloc((new_col * new_row), sizeof(int));
  convert_to_adjacent(grid, adjacency_matrix, row, col);
  // print_arr(adjacency_matrix, new_row, new_col);

  int *djcomp_matrix = (int *)calloc((new_col * new_row), sizeof(int));
  // const Pos source = {0, 0};
  Pos pos[new_col];
  int previous[row * col];
  int count = 0;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      pos[count] = (Pos){i, j};
      count++;
    }
  }
  dijkstra(djcomp_matrix, row, col, adjacency_matrix, &enemy.pos, pos,
           previous);
  // print_arr(djcomp_matrix, new_row, new_col);
  Pos destination = {8, 0};
  Pos *return_pos = (Pos *)malloc(sizeof(Pos) * new_col);

  int return_index;
  return_pos =
      path(new_col, pos, &destination, previous, &enemy.pos, &return_index);
  for (int i = 0; i < return_index; i++) {
    printf("(%d %d)", return_pos[i].x, return_pos[i].y);
  }
  printf("\n");
  int a = 0;
  float enemyMovementTimer = 0.0f;
  float enemyMovementDelay = 0.2f;
  Pos po = destination;
  while (!WindowShouldClose()) {
    // variable update
    float deltaTime = GetFrameTime();
    enemyMovementTimer += deltaTime;
    switch (GetKeyPressed()) {
    case KEY_D:
      destination.y++;
      break;
    case KEY_A:
      destination.y--;
      break;
    case KEY_W:
      destination.x--;
      break;
    case KEY_S:
      destination.x++;
      break;
    }
    if (enemyMovementTimer >= enemyMovementDelay && a < return_index) {
      enemy.pos.x = return_pos[a].x;
      enemy.pos.y = return_pos[a].y;
      a++;
      enemyMovementTimer = 0.0f;
    }
    if (enemy.pos.x == po.x && enemy.pos.y == po.y) {
        a = 0;
        dijkstra(djcomp_matrix, row, col, adjacency_matrix, &enemy.pos, pos,
                 previous);
        return_pos =
            path(new_col, pos, &destination, previous, &enemy.pos, &return_index);
        po = destination;
    }
    BeginDrawing();
    ClearBackground(BLACK);

    for (size_t y = 0; y < col; y++) {
      for (size_t x = 0; x < row; x++) {
        DrawRectangle(x * window.scale, y * window.scale, window.scale - 1,
                      window.scale - 1, RAYWHITE);
      }
    }
    DrawRectangle(enemy.pos.y * window.scale, enemy.pos.x * window.scale,
                  window.scale - 1, window.scale - 1, PINK);
    DrawRectangle(destination.y * window.scale, destination.x * window.scale,
                  window.scale - 1, window.scale - 1, GREEN);

    EndDrawing();
  }

  free(grid);
  free(djcomp_matrix);
  free(adjacency_matrix);
  CloseWindow();
  return 0;
}
