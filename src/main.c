#include "dijkstra.h"
#include "raylib.h"
#include <stdbool.h>

typedef struct {
  const int width;
  const int height;
  const int scale;
  const int fps;
  const char *title;
  const int font_size;
} Window;

typedef enum GameState { GAME_OVER, GAME_PLAY } GameState;

typedef struct {
  Pos pos;
  Vector2 velocity;
} Entity;

static Pos food_position_generator(const int row, const int col) {
  int x = GetRandomValue(0, row - 2);
  int y = GetRandomValue(0, col - 2);

  return (Pos){x, y};
}
static void movement_logic(Entity *pos) {
  switch (GetKeyPressed()) {
  case KEY_D:
    pos->velocity = (Vector2){0, 1};
    break;
  case KEY_A:
    pos->velocity = (Vector2){0, -1};
    break;
  case KEY_W:
    pos->velocity = (Vector2){-1, 0};
    break;
  case KEY_S:
    pos->velocity = (Vector2){1, 0};
    break;
  }
}

static bool collide(const int row, const int col, Entity *entity,
                    Entity *entity2) {
  if (entity->velocity.y == 1) {
    if (entity->pos.y >= col - 1) {
      printf("collide y(%d %d) (%f %f) row: %d col:%d\n", entity->pos.x,
             entity->pos.y, entity->velocity.x, entity->velocity.y, row, col);
      return true;
    }
  }
  if (entity->velocity.y == -1) {
    if (entity->pos.y <= 0) {
      printf("collide y(%d %d) (%f %f) row: %d col:%d\n", entity->pos.x,
             entity->pos.y, entity->velocity.x, entity->velocity.y, row, col);
      return true;
    }
  }
  if (entity->velocity.x == 1) {
    if (entity->pos.x >= col - 2) {
      printf("collide x(%d %d) (%f %f) row: %d col:%d\n", entity->pos.x,
             entity->pos.y, entity->velocity.x, entity->velocity.y, row, col);
      return true;
    }
  }
  if (entity->velocity.x == -1) {
    if (entity->pos.x <= 0) {
      printf("collide x(%d %d) (%f %f) row: %d col:%d\n", entity->pos.x,
             entity->pos.y, entity->velocity.x, entity->velocity.y, row, col);
      return true;
    }
  }
  if (entity->pos.x == entity2->pos.x && entity->pos.y == entity2->pos.y) {
    printf("collide entities game over(%d %d) (%d %d)\n", entity->pos.x,
           entity->pos.y, entity2->pos.x, entity2->pos.y);
    return true;
  }
  return false;
}
static bool food_collision(Pos *food, Entity *entity) {
  if (food->x == entity->pos.x && food->y == entity->pos.y) {
    return true;
  }
  return false;
}
int main(void) {
  static const Window window = {600, 600, 40, 60, "pac-man", 25};
  InitWindow(window.width, window.height, window.title);
  SetTargetFPS(window.fps);

  static const int row = window.width / window.scale;
  static const int col = window.height / window.scale;

  static Entity enemy = {{0, 0}, {0, 0}};

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
  static Pos pos[new_col];
  static int previous[row * col];
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
  Entity destination = {{8, 8}, {0, 0}};

  int return_index;
  Pos *return_pos =
      path(new_col, pos, &destination.pos, previous, &enemy.pos, &return_index);
  // for (int i = 0; i < return_index; i++) {
  //   printf("(%d %d)", return_pos[i].x, return_pos[i].y);
  // }
  // printf("\n");
  int enemy_arr_index = 0;
  float enemy_movement_timer = 0.0f;
  float enemy_movement_delay = 0.051f;

  Pos previous_position_destination = destination.pos;
  int frames = 0;
  static int score = 0;

  const char instruction[] = "movement: (w, a, s, d)";
  const size_t size_instruction = sizeof(instruction) / sizeof(char);
  static const int differ_row =
      size_instruction /
      3; // as one scale can contain 3 letter (dont forget this reason)

  Pos food = {5, 5};

  while (!WindowShouldClose()) {
    // variable update
    float delta_time = GetFrameTime();
    frames++;

    enemy_movement_timer += delta_time;
    movement_logic(&destination);

    if (collide(row, col, &destination, &enemy)) {
      destination.velocity = (Vector2){0, 0};
    }
    if (food_collision(&food, &destination)) {
      food = food_position_generator(row, col);
      score++;
    }

    if (enemy_movement_timer >= enemy_movement_delay &&
        enemy_arr_index < return_index) {
      enemy.pos = return_pos[enemy_arr_index];
      enemy_arr_index++;
      enemy_movement_timer = 0.0f;
    }

    if (frames % 5 == 0) {
      destination.pos.x += destination.velocity.x;
      destination.pos.y += destination.velocity.y;
    }

    if (enemy.pos.x == previous_position_destination.x &&
        enemy.pos.y == previous_position_destination.y) {
      enemy_arr_index = 0;
      dijkstra(djcomp_matrix, row, col, adjacency_matrix, &enemy.pos, pos,
               previous);
      return_pos = path(new_col, pos, &destination.pos, previous, &enemy.pos,
                        &return_index);
      previous_position_destination = destination.pos;
    }

    BeginDrawing();
    ClearBackground(BLACK);

    for (size_t y = 0; y < col - 1; y++) {
      for (size_t x = 0; x < row; x++) {
        DrawRectangle(x * window.scale, y * window.scale, window.scale - 1,
                      window.scale - 1, RAYWHITE);
      }
    }
    DrawRectangle(food.y * window.scale, food.x * window.scale,
                  window.scale - 1, window.scale - 1, RED);
    DrawRectangle(enemy.pos.y * window.scale, enemy.pos.x * window.scale,
                  window.scale - 1, window.scale - 1, PINK);
    DrawRectangle(destination.pos.y * window.scale,
                  destination.pos.x * window.scale, window.scale - 1,
                  window.scale - 1, GREEN);

    DrawText(TextFormat("Score: %d", score), 0, (col - 1) * window.scale,
             window.font_size, RAYWHITE);
    DrawText("movement: (w, a, s, d)", (row - differ_row) * window.scale,
             (col - 1) * window.scale, window.font_size, RAYWHITE);
    EndDrawing();
  }

  free(grid);
  free(djcomp_matrix);
  free(adjacency_matrix);
  free(return_pos);

  CloseWindow();
  return 0;
}
