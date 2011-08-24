#include <stdio.h>
#include <ruby.h>
#include <mpfr.h>
#include "ruby_mpfr.h"

typedef struct __MPFRMatrix{
  int row, column, size;
  MPFR *data;
} MPFRMatrix;

/*
  1 2 3
  4 5 6

  row = 2
  column = 3
  size = 6
  MPFR *data 1 4 2 5 3 6

  (i, j) element => data + i + row * j
  0 <= i < 2, 0 <= j < 3
*/

#define r_mpfr_make_matrix_struct(ruby_var, c_var) { ruby_var = Data_Make_Struct(r_mpfr_matrix, MPFRMatrix, 0, r_mpfr_matrix_free, c_var); }
#define r_mpfr_get_matrix_struct(c_var, ruby_var) { Data_Get_Struct(ruby_var, MPFRMatrix, c_var); }

#define r_mpfr_make_square_matrix_struct(ruby_var, c_var) { ruby_var = Data_Make_Struct(r_mpfr_square_matrix, MPFRMatrix, 0, r_mpfr_matrix_free, c_var); }
#define r_mpfr_make_col_vector_struct(ruby_var, c_var) { ruby_var = Data_Make_Struct(r_mpfr_col_vector, MPFRMatrix, 0, r_mpfr_matrix_free, c_var); }
#define r_mpfr_make_row_vector_struct(ruby_var, c_var) { ruby_var = Data_Make_Struct(r_mpfr_row_vector, MPFRMatrix, 0, r_mpfr_matrix_free, c_var); }

#define r_mpfr_matrix_temp_alloc_init(c_var, i, j) { c_var = ALLOC_N(MPFRMatrix, 1); mpfr_matrix_init(c_var, i, j); }
#define r_mpfr_matrix_temp_free(c_var) { mpfr_matrix_clear(c_var); free(c_var); }

#define mpfr_matrix_get_ptr(matrix, i) (matrix->data + i)
#define mpfr_matrix_get_element(matrix, i, j) (matrix->data + i + matrix->row * j)

void mpfr_matrix_clear(MPFRMatrix *mat);
void mpfr_matrix_init(MPFRMatrix *mat, int row, int column);
void mpfr_matrix_set_zeros(MPFRMatrix *mat);
void mpfr_matrix_set_element(MPFRMatrix *mat, int row, int col, MPFR *a);
void mpfr_matrix_set(MPFRMatrix *new, MPFRMatrix *x);
void mpfr_matrix_swap(MPFRMatrix *x, MPFRMatrix *y);

void mpfr_matrix_row(MPFRMatrix *new, MPFRMatrix *x, int row);
void mpfr_matrix_column(MPFRMatrix *new, MPFRMatrix *x, int column);
void mpfr_matrix_transpose(MPFRMatrix *new, MPFRMatrix *x);
void mpfr_matrix_neg(MPFRMatrix *new, MPFRMatrix *x);

int mpfr_matrix_equal_p(MPFRMatrix *x, MPFRMatrix *y);
void mpfr_matrix_add(MPFRMatrix *new, MPFRMatrix *x, MPFRMatrix *y);
void mpfr_matrix_sub(MPFRMatrix *new, MPFRMatrix *x, MPFRMatrix *y);
void mpfr_matrix_mul(MPFRMatrix *new, MPFRMatrix *x, MPFRMatrix *y);
void mpfr_matrix_mul_scalar(MPFRMatrix *new, MPFRMatrix *x, MPFR *scalar);
void mpfr_matrix_div_scalar(MPFRMatrix *new, MPFRMatrix *x, MPFR *scalar);
void mpfr_matrix_inner_product(MPFR *pr, MPFRMatrix *x, MPFRMatrix *y);
void mpfr_matrix_vector_distance(MPFR *distance, MPFRMatrix *x, MPFRMatrix *y);
void mpfr_matrix_vector_norm(MPFR *norm, MPFRMatrix *x);
void mpfr_matrix_max_norm(MPFR *norm, MPFRMatrix *x);

void mpfr_col_vector_init(MPFRMatrix *mat, int row);
void mpfr_row_vector_init(MPFRMatrix *mat, int column);
int mpfr_vector_normalize(MPFRMatrix *new, MPFRMatrix *x);
int mpfr_vector_set_length(MPFRMatrix *new, MPFRMatrix *x, MPFR *l);
void mpfr_vector_midpoint(MPFRMatrix *new, MPFRMatrix *x, MPFRMatrix *y);
void mpfr_vector_dividing_point(MPFRMatrix *new, MPFRMatrix *x, MPFRMatrix *y, MPFR *div);

int mpfr_square_matrix_lu_decomp (MPFRMatrix *ret, int *indx, MPFRMatrix *x);
void mpfr_square_matrix_determinant(MPFR *det, MPFRMatrix *x);
void mpfr_square_matrix_qr_decomp(MPFRMatrix *q, MPFRMatrix *r, MPFRMatrix *x);
void mpfr_square_matrix_identity(MPFRMatrix *id);


