#include "dijkstra.h"
#include <limits.h>

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
  printf("\n");
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      if (arr[(i * col + j)] == INT_MAX) {
        printf("-  ");
      } else {
        printf("%d  ", arr[(i * col + j)]);
      }
    }
    printf("\n");
  }
  printf("\n");
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
  int row = 0, col, min;
  // for (int i = 0; i < o_col * o_row * o_col * o_row; i++) {
  //   djcomp_matrix[i] = INT_MAX;
  // }
  for (int i = 0; i < o_col * o_row; i++) {
    for (int j = 0; j < o_col * o_row; j++) {
      djcomp_matrix[i * o_col * o_row + j] = INT_MAX;
    }
    previous[i] = -1;
  }
  for (int i = 0; i < o_col * o_row; i++) {
    if (source->x == pos[i].x && source->y == pos[i].y) {
      row = i;
      break;
    }
  }
  djcomp_matrix[row] = 0;

  for (int k = 0; k < o_col * o_row - 1; k++) {
    col = min_index(djcomp_matrix, visited, k, o_col * o_row);
    min = djcomp_matrix[k * (o_col * o_row) + col];
    visited[col] = 1;

    for (int i = 0; i < o_col * o_row; i++) {
      if (visited[i] == 0) {
        djcomp_matrix[(k + 1) * (o_col * o_row) + i] =
            djcomp_matrix[k * (o_col * o_row) + i];
        if (adjacency_matrix[col * o_col * o_row + i] != 0) {
          int temp = adjacency_matrix[col * o_col * o_row + i] + min;
          if (temp < djcomp_matrix[(k + 1) * (o_col * o_row) + i]) {
            djcomp_matrix[(k + 1) * (o_col * o_row) + i] = temp;
            previous[i] = col;
          }
        }
      }
    }
  }
  free(visited);
}

Pos *path(const int iter_value, Pos *vertex, const Pos *dest_vertex,
          int *previous, const Pos *source,int *ind) {

  int dest_index = -1;
  Pos *return_pos = (Pos*)malloc(sizeof(Pos)*iter_value);
  for (int i = 0; i < iter_value; i++) {
    if (vertex[i].x == dest_vertex->x && vertex[i].y == dest_vertex->y) {
      dest_index = i;
      break;
    }
  }

  if (dest_index == -1) {
    printf("Destination not found.\n");
    return NULL;
  }

  int current = dest_index;
  printf("Shortest path from (%d, %d) to (%d, %d) : ", source->x, source->y,
         dest_vertex->x, dest_vertex->y);
  if (previous[current] == -1) {
    printf("No path found.\n");
  } else {
    int path[iter_value], path_index = 0;
    while (current != -1) {
      path[path_index++] = current;
      current = previous[current];
    }
    for (int i = path_index - 1,in=0; i >= 0; i--,in++) {
      printf("(%d, %d) ", vertex[path[i]].x, vertex[path[i]].y);
      return_pos[in].y = vertex[path[i]].y;
      return_pos[in].x = vertex[path[i]].x;
      // DrawRectangle(vertex[path[i]].y*scale, vertex[path[i]].x*scale,
      // scale-1, scale-1, color);
    }
    *ind = path_index;
    printf("\n");
    // for (int i = 0; i < *ind; i++) {
    //   printf("(%d %d)",return_pos[i].x,return_pos[i].y);
    // }
    // printf("\n");
  }
  return return_pos;
}
