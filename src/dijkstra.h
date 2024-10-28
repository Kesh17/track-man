#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int x;
  int y;
} Pos;

void print_arr(int *arr, int col, int row);
void convert_to_adjacent(int *matrix, int *adjacency_matrix, int col, int row);
void dijkstra(int *djcomp_matrix, int o_row, int o_col, int *adjacency_matrix,
              const Pos *source, Pos *pos,int *previous);
Pos *path(const int iter_value, Pos *vertex, const Pos *dest_vertex,
          int *previous, const Pos *source,int *in) ;

#endif
