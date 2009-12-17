#ifndef RUBY_MPFI_MATRIX
#define RUBY_MPFI_MATRIX

#include "func_mpfi_matrix.h"

VALUE r_mpfi_matrix, r_mpfi_square_matrix, r_mpfi_col_vector, r_mpfi_row_vector, r_mpfi_vector_module;

void r_mpfi_matrix_free(void *ptr);

void r_mpfi_matrix_suitable_matrix_init (VALUE *other, MPFIMatrix **ptr_other, int row, int column);
VALUE r_mpfi_matrix_robj(MPFIMatrix *x);

#endif
