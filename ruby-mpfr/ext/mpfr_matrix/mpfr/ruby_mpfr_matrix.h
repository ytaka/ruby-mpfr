#ifndef RUBY_GMP_MATRIX
#define RUBY_GMP_MATRIX

#include "func_mpfr_matrix.h"

VALUE r_mpfr_matrix, r_mpfr_square_matrix, r_mpfr_col_vector, r_mpfr_row_vector, r_mpfr_vector_module;

void r_mpfr_matrix_free(void *ptr);

void r_mpfr_matrix_suitable_matrix_init (VALUE *other, MPFRMatrix **ptr_other, int row, int column);
VALUE r_mpfr_matrix_robj(MPFRMatrix *x);

#endif
