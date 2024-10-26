#include "dijkstra.h"

static int min_index(int *djcomp_matrix, int *visited, int row,
              const int iter_value) {
  int col, min = INT_MAX;
  for (int j = 0; j < iter_value; j++) {
    if (visited[j] == 0 && *(djcomp_matrix + (row * iter_value + j)) < min) {
      min = *(djcomp_matrix + (row * iter_value + j));
      col = j;
    }
  }
  return col;
}

void print_arr(int *arr, int col, int row) {
  printf("\n");
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      if (*(arr + (i * col + j)) == INT_MAX) {
        printf("-  ");
      } else {
        printf("%d  ", *(arr + (i * col + j)));
      }
    }
    printf("\n");
  }
}

void convert_to_adjacent(int *matrix, int *adjacency_matrix, int col, int row) {
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      int index = i * col + j;

      if (*(matrix + (i * col + j))) {
        if (j + 1 < col) {
          int right_index = i * col + (j + 1);
          *(adjacency_matrix + (index * (col * row) + right_index)) = 1;
          *(adjacency_matrix + (right_index * (col * row) + index)) = 1;
        }

        if (i + 1 < row) {
          int bottom_index = (i + 1) * col + j;
          *(adjacency_matrix + (index * (col * row) + bottom_index)) = 1;
          *(adjacency_matrix + (bottom_index * (col * row) + index)) = 1;
        }
      }
    }
  }
}

void dijkstra(int *djcomp_matrix, const int o_row, const int o_col, int *adjacency_matrix,
              const Pos *source,Pos *pos) {
  int *visited= (int *)calloc((o_col* o_row), sizeof(int));
  int row, col, min;
  for (int i = 0; i < o_col * o_row * o_col * o_row; i++) {
    djcomp_matrix[i] = INT_MAX;
  }

  for (int i = 0; i < o_col * o_row; i++) {
    if (source->x == pos[i].x && source->y == pos[i].y) {
      row = i;
      break;
    }
  }
  *(djcomp_matrix + row) = 0;

  for (int k = 0; k < o_col * o_row - 1; k++) {
    col = min_index(djcomp_matrix, visited, k, o_col * o_row);
    min = djcomp_matrix [ k * (o_col * o_row) + col];
    visited[col] = 1;

    for (int i = 0; i < o_col * o_row; i++) {
      if (visited[i] == 0) {
        djcomp_matrix[(k + 1) * (o_col * o_row) + i] = djcomp_matrix[k * (o_col * o_row) + i];
        if (adjacency_matrix[col * o_col * o_row + i] != 0) {
          int temp = adjacency_matrix[col * o_col * o_row + i] + min;
          if (temp < djcomp_matrix[(k + 1) * (o_col * o_row) + i] ) {
            djcomp_matrix[(k + 1) * (o_col * o_row) + i] = temp;
          }
        }
      }
    }
  }
}
