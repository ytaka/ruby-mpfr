#include <stdio.h>
#include <ruby.h>
#include <mpfr.h>
#include <mpfi.h>
#include <mpfi_io.h>
#include "ruby_mpfr_matrix.h"
#include "ruby_mpfi.h"
#include "ruby_mpfi_complex.h"

typedef struct __MPFIMatrix{
  int row, column, size;
  MPFI *data;
} MPFIMatrix;

/*
  1 2 3
  4 5 6

  row = 2
  column = 3
  size = 6
  MPFI *data 1 4 2 5 3 6

  (i, j) element => data + i + row * j
  0 <= i < 2, 0 <= j < 3
*/

#define r_mpfi_make_matrix_struct(ruby_var, mpfi_var) { ruby_var = Data_Make_Struct(r_mpfi_matrix, MPFIMatrix, 0, r_mpfi_matrix_free, mpfi_var); }
#define r_mpfi_get_matrix_struct(mpfi_var, ruby_var) { Data_Get_Struct(ruby_var, MPFIMatrix, mpfi_var); }

#define r_mpfi_make_square_matrix_struct(ruby_var, mpfi_var) { ruby_var = Data_Make_Struct(r_mpfi_square_matrix, MPFIMatrix, 0, r_mpfi_matrix_free, mpfi_var); }
#define r_mpfi_make_col_vector_struct(ruby_var, mpfi_var) { ruby_var = Data_Make_Struct(r_mpfi_col_vector, MPFIMatrix, 0, r_mpfi_matrix_free, mpfi_var); }
#define r_mpfi_make_row_vector_struct(ruby_var, mpfi_var) { ruby_var = Data_Make_Struct(r_mpfi_row_vector, MPFIMatrix, 0, r_mpfi_matrix_free, mpfi_var); }

#define r_mpfi_matrix_temp_alloc_init(mpfi_var, i, j) { mpfi_var = ALLOC_N(MPFIMatrix, 1); mpfi_matrix_init(mpfi_var, i, j); }
#define r_mpfi_matrix_temp_free(mpfi_var) { mpfi_matrix_clear(mpfi_var); free(mpfi_var); }

#define mpfi_matrix_get_ptr(matrix, i) (matrix->data + i)
#define mpfi_matrix_get_element(matrix, i, j) (matrix->data + i + matrix->row * j)


/* Returned value may be same pointer as an argument which has always same size. */
/* But if returned value may have different size from that of an argument */
/* (for example, mpfi_matrix_transpose), */
/* you must use differnt pointer for returned value from that of an argument. */

void mpfi_matrix_clear(MPFIMatrix *mat);
void mpfi_matrix_init(MPFIMatrix *mat, int row, int column);
void mpfi_matrix_set_zeros(MPFIMatrix *mat);
void mpfi_matrix_set_element(MPFIMatrix *mat, int row, int col, MPFI *a);
void mpfi_matrix_set(MPFIMatrix *new, MPFIMatrix *x);
void mpfi_matrix_swap(MPFIMatrix *x, MPFIMatrix *y);

void mpfi_matrix_row(MPFIMatrix *new, MPFIMatrix *x, int row);
void mpfi_matrix_column(MPFIMatrix *new, MPFIMatrix *x, int column);
void mpfi_matrix_transpose(MPFIMatrix *new, MPFIMatrix *x);
void mpfi_matrix_neg(MPFIMatrix *new, MPFIMatrix *x);

int mpfi_matrix_equal_p(MPFIMatrix *x, MPFIMatrix *y);
void mpfi_matrix_add(MPFIMatrix *new, MPFIMatrix *x, MPFIMatrix *y);
void mpfi_matrix_add_fr(MPFIMatrix *new, MPFIMatrix *x, MPFRMatrix *y);
void mpfi_matrix_sub(MPFIMatrix *new, MPFIMatrix *x, MPFIMatrix *y);
void mpfi_matrix_sub_fr(MPFIMatrix *new, MPFIMatrix *x, MPFRMatrix *y);
void mpfi_matrix_mul(MPFIMatrix *new, MPFIMatrix *x, MPFIMatrix *y);
void mpfi_matrix_mul_fr(MPFIMatrix *new, MPFIMatrix *x, MPFRMatrix *y);
void mpfi_matrix_mul_scalar(MPFIMatrix *new, MPFIMatrix *x, MPFI *scalar);
void mpfi_matrix_div_scalar(MPFIMatrix *new, MPFIMatrix *x, MPFI *scalar);

int mpfi_matrix_include_p(MPFIMatrix *x, MPFIMatrix *y);
int mpfi_matrix_include_fr_p(MPFIMatrix *x, MPFRMatrix *y);
int mpfi_matrix_strictly_include_p(MPFIMatrix *x, MPFIMatrix *y);
int mpfi_matrix_bounded_p(MPFIMatrix *x);
void mpfi_matrix_mid(MPFRMatrix *ret, MPFIMatrix *x);
void mpfi_matrix_mid_interval(MPFIMatrix *ret, MPFIMatrix *x);
void mpfi_matrix_from_mpfr_matrix(MPFIMatrix *ret, MPFRMatrix *x);
int mpfi_matrix_intersect(MPFIMatrix *ret, MPFIMatrix *x, MPFIMatrix *y);
int mpfi_matrix_union(MPFIMatrix *z, MPFIMatrix *x, MPFIMatrix *y);

void mpfi_matrix_inner_product(MPFI *pr, MPFIMatrix *x, MPFIMatrix *y);
void mpfi_matrix_vector_distance(MPFI *distance, MPFIMatrix *x, MPFIMatrix *y);
void mpfi_matrix_vector_distance_center_pts(MPFR *distance, MPFIMatrix *x, MPFIMatrix *y);
void mpfi_matrix_vector_norm(MPFI *norm, MPFIMatrix *x);
void mpfi_matrix_max_norm(MPFI *norm, MPFIMatrix *x);
void mpfi_matrix_max_diam_abs(MPFR *diam, MPFIMatrix *x);

/* ----------------- vector ---------------- */
void mpfi_col_vector_init(MPFIMatrix *mat, int row);
void mpfi_row_vector_init(MPFIMatrix *mat, int column);
int mpfi_vector_normalize(MPFIMatrix *new, MPFIMatrix *x);
void mpfi_vector_midpoint(MPFIMatrix *new, MPFIMatrix *x, MPFIMatrix *y);
int mpfi_vector_set_length(MPFIMatrix *new, MPFIMatrix *x, MPFR *l);

/* ----------------- square matrix ----------------- */
int mpfi_square_matrix_lu_decomp (MPFIMatrix *ret, int *indx, MPFIMatrix *x);
void mpfi_2d_square_matrix_determinant(MPFI *det, MPFIMatrix *x);
void mpfi_3d_square_matrix_determinant(MPFI *det, MPFIMatrix *x);
void mpfi_square_matrix_determinant(MPFI *det, MPFIMatrix *x);
void mpfi_square_matrix_qr_decomp(MPFIMatrix *q, MPFIMatrix *r, MPFIMatrix *x);
int mpfi_2d_square_matrix_inverse_matrix(MPFIMatrix *inv, MPFIMatrix *x);
int mpfi_2d_square_matrix_eigenvalue(MPFI *val1, MPFI *val2, MPFIMatrix *x);
void mpfi_2d_square_matrix_real_eigenvector(MPFIMatrix *vec, MPFIMatrix *x, MPFI *eigenval);
void mpfi_square_matrix_identity(MPFIMatrix *id);

