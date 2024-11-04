#include "dijkstra.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>

#ifdef DBG
#define log(fmt, args...)                                                      \
  fprintf(stderr, "INFO: %s:%d: " fmt, __FILE_NAME__, __LINE__, ##args)
#define log_(fmt, args...) fprintf(stderr, fmt, ##args)
#else
#define log(fmt, args...)
#define log_(fmt, args...)
#endif

typedef struct {
  const int width;
  const int height;
  const int scale;
  const int fps;
  const char *title;
  const int font_size;
} Window;

typedef enum GameState { GAME_OVER, GAME_PLAY, GAME_PAUSE } GameState;

typedef struct {
  Pos pos;
  Vector2 velocity;
} Entity;

static Pos position_generator(const int row, const int col) {
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

static bool wall_collision(const int row, const int col, Entity *entity) {
  if (entity->velocity.y == 1) {
    if (entity->pos.y >= col - 1) {
      log("wall collision y(%d %d) (%.1f %.1f) row: %d col:%d\n", entity->pos.x,
          entity->pos.y, entity->velocity.x, entity->velocity.y, row, col);
      return true;
    }
  }
  if (entity->velocity.y == -1) {
    if (entity->pos.y <= 0) {
      log("wall collision -y(%d %d) (%.1f %.1f) row: %d col:%d\n", entity->pos.x,
          entity->pos.y, entity->velocity.x, entity->velocity.y, row, col);
      return true;
    }
  }
  if (entity->velocity.x == 1) {
    if (entity->pos.x >= col - 2) {
      log("wall collided -x(%d %d) (%.1f %.1f) row: %d col:%d\n", entity->pos.x,
          entity->pos.y, entity->velocity.x, entity->velocity.y, row, col);
      return true;
    }
  }
  if (entity->velocity.x == -1) {
    if (entity->pos.x <= 0) {
      log("wall collision x(%d %d) (%.1f %f) row: %d col:%.1d\n", entity->pos.x,
          entity->pos.y, entity->velocity.x, entity->velocity.y, row, col);
      return true;
    }
  }

  return false;
}

static bool food_collision(Pos *food, Entity *entity) {
  if (food->x == entity->pos.x && food->y == entity->pos.y) {
    log("collision with food at:(%d, %d) (%d, %d)\n", food->x, food->y,
        entity->pos.x, entity->pos.y);
    return true;
  }
  return false;
}

static bool entity_collision(Entity *entity, Pos *entity2) {
  if (entity->pos.x == entity2->x && entity->pos.y == entity2->y) {
    log("entities collided at:(%d %d) (%d %d) game over\n", entity->pos.x,
        entity->pos.y, entity2->x, entity2->y);
    return true;
  }
  return false;
}

int main(void) {
  const Window window = {800, 800, 40, 60, "track-man", 25};
  InitWindow(window.width, window.height, window.title);
  SetTargetFPS(window.fps);

  const int row = window.width / window.scale;
  const int col = window.height / window.scale;

  const int new_row = row * col;
  const int new_col = row * col;

  Pos pos[new_col];
  Pos enemy = {0, 0};
  int grid[row * col];

  log("window details:\n\t resoultion: %dx%d, scale: %d, fps: %d, title: %s, "
      "font-size: %d, row: %d, col: %d, new-row: %d, new-col: %d\n",
      window.width, window.height, window.scale, window.fps, window.title,
      window.font_size, row, col, new_row, new_col);

  for (int i = 0, count = 0; i < row; i++) {
    for (int j = 0; j < col; j++, count++) {
      grid[(i * row + j)] = 1;
      pos[count] = (Pos){i, j};
    }
  }

  int *adjacency_matrix = (int *)calloc(sizeof(int), new_col * new_row);
  if (adjacency_matrix == NULL) {
    log("error creating adjacency matrix\n");
  }
  convert_to_adjacent(grid, adjacency_matrix, row, col);

  int djcomp_matrix[new_col * new_row];
  int previous[row * col];
  dijkstra(djcomp_matrix, row, col, adjacency_matrix, &enemy, pos, previous);

  Entity destination = {{8, 8}, {0, 0}};

  int return_index;
  Pos *return_pos =
      path(new_col, pos, &destination.pos, previous, &enemy, &return_index);
  if (return_pos == NULL) {
    log("error creating return positions array\n");
  }

  int enemy_arr_index = 0;
  float enemy_movement_timer = 0.0f;
  float enemy_movement_delay = 0.051f;

  Pos previous_position_destination = destination.pos;
  int frames = 0;
  int score = 0;

  const char instruction[] = " pause: space  movement: (w, a, s, d)";
  const size_t size_instruction = sizeof(instruction) / sizeof(char);
  const int differ_row =
      size_instruction /
      3; // as one scale can contain 3 letter (dont forget this reason)

  Pos food = {5, 5};

  GameState game_state = GAME_PAUSE;

  while (!WindowShouldClose()) {

    // variable update
    if (game_state == GAME_PLAY) {
      if (IsKeyPressed(KEY_SPACE)) {
        game_state = GAME_PAUSE;
      }

      float delta_time = GetFrameTime();
      frames++;
      enemy_movement_timer += delta_time;

      movement_logic(&destination);

      if (wall_collision(row, col, &destination)) {
        destination.velocity = (Vector2){0, 0};
      }

      if (entity_collision(&destination, &enemy)) {
        game_state = GAME_OVER;
      }

      if (food_collision(&food, &destination)) {
        food = position_generator(row, col);
        score++;
      }

      if (enemy_movement_timer >= enemy_movement_delay &&
          enemy_arr_index < return_index) {
        enemy = return_pos[enemy_arr_index];
        enemy_arr_index++;
        enemy_movement_timer = 0.0f;
      }

      if (frames % 5 == 0) {
        destination.pos.x += destination.velocity.x;
        destination.pos.y += destination.velocity.y;
      }

      if (enemy.x == previous_position_destination.x &&
          enemy.y == previous_position_destination.y) {
        enemy_arr_index = 0;
        dijkstra(djcomp_matrix, row, col, adjacency_matrix, &enemy, pos,
                 previous);
        free(return_pos);
        return_pos = path(new_col, pos, &destination.pos, previous, &enemy,
                          &return_index);
        previous_position_destination = destination.pos;
      }
    } else if (game_state == GAME_OVER) {
      if (IsKeyPressed(KEY_ENTER)) {
        enemy = position_generator(row, col);
        dijkstra(djcomp_matrix, row, col, adjacency_matrix, &enemy, pos,
                 previous);
        destination.pos = position_generator(row, col);
        destination.velocity = (Vector2){0, 0};
        free(return_pos);
        return_pos = path(new_col, pos, &destination.pos, previous, &enemy,
                          &return_index);
        previous_position_destination = destination.pos;
        food = position_generator(row, col);
        frames = 0;
        score = 0;
        game_state = GAME_PAUSE;
        enemy_movement_timer = 0.0f;
        enemy_arr_index = 0;
      }
    } else {
      if (IsKeyPressed(KEY_SPACE)) {
        game_state = GAME_PLAY;
      }
    }

    BeginDrawing();
    ClearBackground(BLACK);

    if (game_state == GAME_PLAY || game_state == GAME_PAUSE) {
      DrawRectangle(0, 0, window.width, window.height, RAYWHITE);
      DrawRectangle(food.y * window.scale, food.x * window.scale,
                    window.scale - 1, window.scale - 1, RED);
      DrawRectangle(enemy.y * window.scale, enemy.x * window.scale,
                    window.scale - 1, window.scale - 1, PINK);
      DrawRectangle(destination.pos.y * window.scale,
                    destination.pos.x * window.scale, window.scale - 1,
                    window.scale - 1, GREEN);
      DrawRectangle(0, (col - 1) * window.scale, window.width, window.scale,
                    BLACK);

      DrawText(TextFormat("Score: %d", score), 0, (col - 1) * window.scale,
               window.font_size, RAYWHITE);

      if (game_state == GAME_PAUSE) {
        DrawRectangle((row / 2) * window.scale, (col / 2 - 2) * window.scale,
                      window.scale - 10, window.scale * 2, BLACK);
        DrawRectangle((row / 2 + 1) * window.scale,
                      (col / 2 - 2) * window.scale, window.scale - 10,
                      window.scale * 2, BLACK);
      }

      DrawText(instruction, (row - differ_row) * window.scale,
               (col - 1) * window.scale, window.font_size, RAYWHITE);
    } else {
      DrawRectangle(0, 0, window.width, window.height, GREEN);
      DrawText("GAME OVER", (row / 2 - 3) * window.scale,
               (col / 2 - 3) * window.scale, 50, DARKGREEN);
      DrawText(TextFormat("SCORE: %d", score), (row / 2) * window.scale,
               (col / 2) * window.scale, 20, DARKGREEN);
      DrawText("PRESS ENTER TO RETRY", (row / 2 - 2) * window.scale,
               (col / 2 - 1) * window.scale, 20, DARKGREEN);
    }

    EndDrawing();
  }

  free(adjacency_matrix);
  free(return_pos);

  CloseWindow();
  return 0;
}
