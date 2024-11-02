#include "dijkstra.h"
#include <limits.h>
#include <stdio.h>

#ifdef DBG
#define log(fmt, args...)                                                      \
  fprintf(stderr, "INFO: %s:%d: " fmt, __FILE_NAME__, __LINE__, ##args)
#define log_(fmt, args...) fprintf(stderr, fmt, ##args)
#else
#define log_(fmt, args...)
#define log(fmt, args...)
#endif


static int min_index(int *djcomp_matrix, int *visited, int row,
                     const int iter_value) {
  int col, min = INT_MAX;
  for (int j = 0; j < iter_value; j++) {
    if (visited[j] == 0 && djcomp_matrix[(row * iter_value + j)] < min) {
      min = djcomp_matrix[(row * iter_value + j)];
      col = j;
    }
  }
  return col;
}

void print_arr(int *arr, int col, int row) {
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      if (arr[(i * col + j)] == INT_MAX) {
        log_("-  ");
      } else {
        log_("%d  ", arr[(i * col + j)]);
      }
    }
    log_("\n");
  }
  log_("\n");
}

void convert_to_adjacent(int *matrix, int *adjacency_matrix, int col, int row) {
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      int index = i * col + j;

      if (matrix[(i * col + j)]) {
        if (j + 1 < col) {
          int right_index = i * col + (j + 1);
          adjacency_matrix[(index * (col * row) + right_index)] = 1;
          adjacency_matrix[(right_index * (col * row) + index)] = 1;
        }

        if (i + 1 < row) {
          int bottom_index = (i + 1) * col + j;
          adjacency_matrix[(index * (col * row) + bottom_index)] = 1;
          adjacency_matrix[(bottom_index * (col * row) + index)] = 1;
        }
      }
    }
  }
}

void dijkstra(int *djcomp_matrix, const int o_row, const int o_col,
              int *adjacency_matrix, const Pos *source, Pos *pos,
              int *previous) {
  int *visited = (int *)calloc((o_col * o_row), sizeof(int));
  if (visited == NULL) {
    log("error creating visited array\n");
    return;
  }
  int row = 0, col, min;
  const int max = o_col * o_row;
  for (int i = 0; i < max; i++) {
    for (int j = 0; j < max; j++) {
      djcomp_matrix[i * max + j] = INT_MAX;
    }
    previous[i] = -1;
  }
  for (int i = 0; i < max; i++) {
    if (source->x == pos[i].x && source->y == pos[i].y) {
      row = i;
      break;
    }
  }
  djcomp_matrix[row] = 0;

  for (int k = 0; k < max - 1; k++) {
    col = min_index(djcomp_matrix, visited, k, max);
    min = djcomp_matrix[k * max + col];
    visited[col] = 1;

    for (int i = 0; i < max; i++) {
      if (visited[i] == 0) {
        djcomp_matrix[(k + 1) * max + i] = djcomp_matrix[k * max + i];
        if (adjacency_matrix[col * max + i] != 0) {
          int temp = adjacency_matrix[col * max + i] + min;
          if (temp < djcomp_matrix[(k + 1) * max + i]) {
            djcomp_matrix[(k + 1) * max + i] = temp;
            previous[i] = col;
          }
        }
      }
    }
  }
  free(visited);
}

Pos *path(const int iter_value, Pos *vertex, const Pos *dest_vertex,
          int *previous, const Pos *source __attribute__((unused)),
          int *return_index) {

  int dest_index = -1;
  Pos *return_pos = (Pos *)malloc(sizeof(Pos) * iter_value);
  if (return_pos == NULL) {
    log("error creating return position array\n");
  }
  for (int i = 0; i < iter_value; i++) {
    if (vertex[i].x == dest_vertex->x && vertex[i].y == dest_vertex->y) {
      dest_index = i;
      break;
    }
  }

  if (dest_index == -1) {
    log("destination not found destination_index = %d\n", dest_index);
    return NULL;
  }

  int current = dest_index;
  log("shortest path from source: (%d, %d) to destination: (%d, %d) : \n",
      source->x, source->y, dest_vertex->x, dest_vertex->y);
  if (previous[current] == -1) {
    log("already at the source as no path found.\n");
  } else {
    int path[iter_value], path_index = 0;
    while (current != -1) {
      path[path_index++] = current;
      current = previous[current];
    }
    log("return position array elements: ");
    for (int i = path_index - 1, j = 0; i >= 0; i--, j++) {
      log_("(%d, %d) ", vertex[path[i]].x, vertex[path[i]].y);
      return_pos[j].y = vertex[path[i]].y;
      return_pos[j].x = vertex[path[i]].x;
    }
    log_("\n");
    *return_index = path_index;
    log("return positions array length: %d\n", *return_index);
  }
  return return_pos;
}
