#include "ruby_mpfi_matrix.h"

void mpfi_matrix_init(MPFIMatrix *mat, int row, int column){
  mat->row = row;
  mat->column = column;
  mat->size = row * column;
  /* mat->data = (MPFI *)malloc(sizeof(MPFI) * mat->size); */
  mat->data = ALLOC_N(MPFI, mat->size);
  int i;
  for(i = 0; i < mat->size; i++){
    mpfi_init(mat->data + i);
  }
}

void mpfi_matrix_set_zeros(MPFIMatrix *mat){
  int i;
  for(i = 0; i < mat->size; i++){
    mpfi_set_si(mat->data + i, 0);
  }
}

void mpfi_matrix_clear(MPFIMatrix *mat){
  int i;
  for(i = 0; i < mat->size; i++){
    mpfi_clear(mat->data + i);
  }
  free(mat->data);
}

void mpfi_matrix_set_element(MPFIMatrix *mat, int row, int col, MPFI *a){
  mpfi_set(mpfi_matrix_get_element(mat, row, col), a);
}

void mpfi_matrix_set(MPFIMatrix *new, MPFIMatrix *x){
  int i;
  for(i = 0; i < x->size; i++){
    mpfi_set(new->data + i, x->data + i);
  }
}

void mpfi_matrix_swap(MPFIMatrix *x, MPFIMatrix *y){
  int i;
  for (i = 0; i < x->size; i++) {
    mpfi_swap(mpfi_matrix_get_ptr(x, i), mpfi_matrix_get_ptr(y, i));
  }
}

void mpfi_matrix_row(MPFIMatrix *new, MPFIMatrix *x, int row){
  int i;
  for (i = 0; i < x->column; i++) {
    mpfi_set(new->data + i, mpfi_matrix_get_element(x, row, i));
  }
}

void mpfi_matrix_column(MPFIMatrix *new, MPFIMatrix *x, int column){
  int i;
  for (i = 0; i < x->row; i++) {
    mpfi_set(new->data + i, mpfi_matrix_get_element(x, i, column));
  }
}

void mpfi_matrix_transpose(MPFIMatrix *new, MPFIMatrix *x){
  int i, j, index;
  for(j = 0; j < x->column; j++){
    index = j * x->row;
    for(i = 0; i < x->row; i++){
      mpfi_set(new->data + j + i * new->row, x->data + i + index);
    }
  }
}

void mpfi_matrix_neg(MPFIMatrix *new, MPFIMatrix *x){
  int i;
  for(i = 0; i < x->size; i++){
    mpfi_neg(new->data + i, x->data + i);
  }
}

/* Return 0 if *x and *y has the same elements. Otherwise return 1. */
int mpfi_matrix_equal_p(MPFIMatrix *x, MPFIMatrix *y){
  int i, ret = 0;
  if (x->column == y->column && x->row == y->row) {
    for (i = 0; i < x->size; i++) {
      if (mpfr_equal_p(r_mpfi_left_ptr(x->data + i), r_mpfi_left_ptr(y->data + i)) == 0 ||
	  mpfr_equal_p(r_mpfi_right_ptr(x->data + i), r_mpfi_right_ptr(y->data + i)) == 0) {
	ret = 1;
	break;
      }
    }
  }else{
    ret = 1;
  }
  return ret;
}

void mpfi_matrix_add(MPFIMatrix *new, MPFIMatrix *x, MPFIMatrix *y){
  int i, j, index;
  for(j = 0; j < x->column; j++){
    index = j * x->row;
    for(i = 0; i < x->row; i++){
      mpfi_add(new->data + i + index, x->data + i + index, y->data + i + index);
    }
  }
}

void mpfi_matrix_add_fr(MPFIMatrix *new, MPFIMatrix *x, MPFRMatrix *y){
  int i, j, index;
  for(j = 0; j < x->column; j++){
    index = j * x->row;
    for(i = 0; i < x->row; i++){
      mpfi_add_fr(new->data + i + index, x->data + i + index, y->data + i + index);
    }
  }
}

void mpfi_matrix_sub(MPFIMatrix *new, MPFIMatrix *x, MPFIMatrix *y){
  int i, j, index;
  for(j = 0; j < x->column; j++){
    index = j * x->row;
    for(i = 0; i < x->row; i++){
      mpfi_sub(new->data + i + index, x->data + i + index, y->data + i + index);
    }
  }
}

void mpfi_matrix_sub_fr(MPFIMatrix *new, MPFIMatrix *x, MPFRMatrix *y){
  int i, j, index;
  for(j = 0; j < x->column; j++){
    index = j * x->row;
    for(i = 0; i < x->row; i++){
      mpfi_sub_fr(new->data + i + index, x->data + i + index, y->data + i + index);
    }
  }
}

/* x and new must be different pointer from each other. */
void mpfi_matrix_mul(MPFIMatrix *new, MPFIMatrix *x, MPFIMatrix *y){
  int i, j, k, index, index_y;
  MPFI *tmp;
  r_mpfi_temp_alloc_init(tmp);
  for(i = 0; i < new->size; i++){
    mpfi_set_si(new->data + i, 0);
  }
  for(j = 0; j < y->column; j++){
    for(i = 0; i < x->row; i++){
      index = i + j * new->row;
      index_y = j * y->row;
      for(k = 0; k < x->column; k++){
	mpfi_mul(tmp, x->data + i + k * x->row, y->data + k + index_y);
	mpfi_add(new->data + index, new->data + index, tmp);
      }
    }
  }
  r_mpfi_temp_free(tmp);
}

/* x and new must be different pointer from each other. */
void mpfi_matrix_mul_fr(MPFIMatrix *new, MPFIMatrix *x, MPFRMatrix *y){
  int i, j, k, index, index_y;
  MPFI *tmp;
  r_mpfi_temp_alloc_init(tmp);
  for(i = 0; i < new->size; i++){
    mpfi_set_si(new->data + i, 0);
  }
  for(j = 0; j < y->column; j++){
    for(i = 0; i < x->row; i++){
      index = i + j * new->row;
      index_y = j * y->row;
      for(k = 0; k < x->column; k++){
	mpfi_mul_fr(tmp, x->data + i + k * x->row, y->data + k + index_y);
	mpfi_add(new->data + index, new->data + index, tmp);
      }
    }
  }
  r_mpfi_temp_free(tmp);
}

void mpfi_matrix_mul_scalar(MPFIMatrix *new, MPFIMatrix *x, MPFI *scalar){
  int i;
  for(i = 0; i < x->size; i++){
    mpfi_mul(new->data + i, x->data + i, scalar);
  }
}

void mpfi_matrix_div_scalar(MPFIMatrix *new, MPFIMatrix *x, MPFI *scalar){
  int i;
  for(i = 0; i < x->size; i++){
    mpfi_div(new->data + i, x->data + i, scalar);
  }
}

/* Return 0 if *x includes *y. Otherwise return 1. */
int mpfi_matrix_include_p(MPFIMatrix *x, MPFIMatrix *y){
  int i, ret = 0;
  if (x->column == y->column && x->row == y->row) {
    for (i = 0; i < x->size; i++) {
      if (mpfi_is_inside(y->data + i, x->data + i) <= 0) {
	ret = 1;
	break;
      }
    }
  }else{
    ret = 1;
  }
  return ret;
}

/* Return 0 if *x includes *y. Otherwise return 1. */
int mpfi_matrix_include_fr_p(MPFIMatrix *x, MPFRMatrix *y){
  int i, ret = 0;
  if (x->column == y->column && x->row == y->row) {
    for (i = 0; i < x->size; i++) {
      if (mpfi_is_inside_fr(y->data + i, x->data + i) <= 0) {
	ret = 1;
	break;
      }
    }
  }else{
    ret = 1;
  }
  return ret;
}

/* Return 0 if *x includes *y. Otherwise return 1. */
int mpfi_matrix_strictly_include_p(MPFIMatrix *x, MPFIMatrix *y){
  int i, ret = 0;
  if (x->column == y->column && x->row == y->row) {
    for (i = 0; i < x->size; i++) {
      if (mpfi_is_strictly_inside(y->data + i, x->data + i) <= 0) {
	ret = 1;
	break;
      }
    }
  }else{
    ret = 1;
  }
  return ret;
}

/* Return 0 if *x is bonded. Otherwise return 1. */
int mpfi_matrix_bounded_p(MPFIMatrix *x){
  int ret = 0, i;
  for (i = 0; i < x->size; i++) {
    if (mpfi_bounded_p(x->data + i) == 0) {
      ret = 1;
      break;
    }
  }
  return ret;
}

void mpfi_matrix_mid(MPFRMatrix *ret, MPFIMatrix *x){
  int i;
  for(i = 0; i < x->size; i++){
    mpfi_mid(ret->data + i, x->data + i);
  }
}

void mpfi_matrix_mid_interval(MPFIMatrix *ret, MPFIMatrix *x){
  int i;
  for(i = 0; i < x->size; i++){
    mpfi_mid_interval(ret->data + i, x->data + i);
  }
}

void mpfi_matrix_from_mpfr_matrix(MPFIMatrix *ret, MPFRMatrix *x){
  int i;
  for(i = 0; i < x->size; i++){
    mpfi_set_fr(ret->data + i, x->data + i);
  }
}

/* This function returns 0 if the intersection of two boxes exists. */
/* Otherwise, it returns nonzero. */
int mpfi_matrix_intersect(MPFIMatrix *z, MPFIMatrix *x, MPFIMatrix *y){
  int ret = 0;
  int i;
  if (x->column == y->column && x->row == y->row) {
    for(i = 0; i < x->size; i++){
      mpfi_intersect(z->data + i, x->data + i, y->data + i);
      if(ret == 0 && mpfi_is_empty(z->data + i)){
	ret = 1;
      }
    }
    return ret;
  }else{
    return -1;
  }
}

/* Return -1 if size of matrixies is not same. */
/* Otherwise, return 0 and set convex hull to MPFIMatrix *z. */
int mpfi_matrix_union(MPFIMatrix *z, MPFIMatrix *x, MPFIMatrix *y){
  int ret = 0;
  int i;
  if (x->column == y->column && x->row == y->row) {
    for(i = 0; i < x->size; i++){
      mpfi_union(z->data + i, x->data + i, y->data + i);
    }
    return ret;
  }else{
    return -1;
  }
}

void mpfi_matrix_inner_product(MPFI *pr, MPFIMatrix *x, MPFIMatrix *y){
  MPFI *tmp;
  r_mpfi_temp_alloc_init(tmp);
  int i;
  mpfi_set_si(pr, 0);
  for(i = 0; i < x->size; i++){
    mpfi_mul(tmp, x->data + i, y->data + i);
    mpfi_add(pr, pr, tmp);
  }
  r_mpfi_temp_free(tmp);
}

void mpfi_matrix_vector_distance(MPFI *distance, MPFIMatrix *x, MPFIMatrix *y){
  MPFI *tmp;
  r_mpfi_temp_alloc_init(tmp);
  int i;
  mpfi_set_si(distance, 0);
  for(i = 0; i < x->size; i++){
    mpfi_sub(tmp, x->data + i, y->data + i);
    mpfi_mul(tmp, tmp, tmp);
    mpfi_add(distance, distance, tmp);
  }
  mpfi_sqrt(distance, distance);
  r_mpfi_temp_free(tmp);
}

void mpfi_matrix_vector_distance_center_pts(MPFR *distance, MPFIMatrix *x, MPFIMatrix *y){
  MPFRMatrix *tmp_x, *tmp_y;
  r_mpfr_matrix_temp_alloc_init(tmp_x, x->row, x->column);
  r_mpfr_matrix_temp_alloc_init(tmp_y, y->row, y->column);
  
  mpfi_matrix_mid(tmp_x, x);
  mpfi_matrix_mid(tmp_y, y);
  mpfr_matrix_vector_distance(distance, tmp_x, tmp_y);

  r_mpfr_matrix_temp_free(tmp_x);
  r_mpfr_matrix_temp_free(tmp_y);
}

void mpfi_matrix_vector_norm(MPFI *norm, MPFIMatrix *x){
  MPFI *tmp;
  r_mpfi_temp_alloc_init(tmp);
  int i;
  mpfi_set_si(norm, 0);
  for(i = 0; i < x->size; i++){
    mpfi_mul(tmp, x->data + i, x->data + i);
    mpfi_add(norm, norm, tmp);
  }
  mpfi_sqrt(norm, norm);
  r_mpfi_temp_free(tmp);
}

void mpfi_matrix_max_norm(MPFI *norm, MPFIMatrix *x){
  MPFI *tmp, *abs;
  r_mpfi_temp_alloc_init(tmp);
  r_mpfi_temp_alloc_init(abs);
  int i;
  mpfi_set_si(norm, 0);
  for(i = 0; i < x->size; i++){
    mpfi_abs(abs, x->data + i);
    mpfi_intersect(tmp, abs, norm);
    if(mpfi_is_empty(tmp) > 0){
      if(mpfr_cmp(r_mpfi_right_ptr(abs), r_mpfi_left_ptr(norm)) > 0){
	mpfi_set(norm, abs);
      }
    }else{
      mpfi_union(norm, norm, abs);
    }
  }
  r_mpfi_temp_free(tmp);
  r_mpfi_temp_free(abs);
}

void mpfi_matrix_max_diam_abs(MPFR *diam, MPFIMatrix *x){
  int i;
  MPFR *tmp;
  r_mpfr_temp_alloc_init(tmp);

  mpfr_set_si(diam, 0, GMP_RNDN);
  for (i = 0; i < x->size; i++) {
    mpfi_diam_abs(tmp, x->data + i);
    if(mpfr_cmp(tmp, diam) > 0){
      mpfr_set(diam, tmp, GMP_RNDN);
    }
  }

  r_mpfr_temp_free(tmp);
}

/* ------------------- vector --------------------- */

void mpfi_col_vector_init(MPFIMatrix *mat, int row){
  mat->row = row;
  mat->column = 1;
  mat->size = row;
  /* mat->data = (MPFI *)malloc(sizeof(MPF) * mat->size); */
  mat->data = ALLOC_N(MPFI, mat->size);
  int i;
  for(i = 0; i < mat->size; i++){
    mpfi_init(mat->data + i);
  }
}

void mpfi_row_vector_init(MPFIMatrix *mat, int column){
  mat->row = 1;
  mat->column = column;
  mat->size = column;
  /* mat->data = (MPFI *)malloc(sizeof(MPF) * mat->size); */
  mat->data = ALLOC_N(MPFI, mat->size);
  int i;
  for(i = 0; i < mat->size; i++){
    mpfi_init(mat->data + i);
  }
}

/* If length of MPFIMatrix *x is zero, return 1. Otherwise return 0. */
int mpfi_vector_normalize(MPFIMatrix *new, MPFIMatrix *x){
  MPFRMatrix *fr_mat;
  r_mpfr_matrix_temp_alloc_init(fr_mat, x->row, x->column);
  mpfi_matrix_mid(fr_mat, x);
  MPFR *norm;
  r_mpfr_temp_alloc_init(norm);
  mpfr_matrix_vector_norm(norm, fr_mat);
  int i, j, index, ret = 0;
  if(mpfr_cmp_ui(norm, 0) > 0){
    for(j = 0; j < x->column; j++){
      index = j * x->row;
      for(i = 0; i < x->row; i++){
	mpfi_div_fr(new->data + i + index, x->data + i + index, norm);
      }
    }
  }else{
    ret = 1;
  }
  r_mpfr_matrix_temp_free(fr_mat);
  r_mpfr_temp_free(norm);
  return ret;
}

void mpfi_vector_midpoint(MPFIMatrix *new, MPFIMatrix *x, MPFIMatrix *y){
  int i;
  for (i = 0; i < new->size; i++) {
    mpfi_add(mpfi_matrix_get_ptr(new, i), mpfi_matrix_get_ptr(x, i), mpfi_matrix_get_ptr(y, i));
    mpfi_div_ui(mpfi_matrix_get_ptr(new, i), mpfi_matrix_get_ptr(new, i), 2);
  }
}

/* If length of MPFIMatrix *x is zero, return 1. Otherwise return 0. */
int mpfi_vector_set_length(MPFIMatrix *new, MPFIMatrix *x, MPFR *length){
  MPFI *norm_i;
  MPFR *factor_r;
  r_mpfi_temp_alloc_init(norm_i);
  r_mpfr_temp_alloc_init(factor_r);
  mpfi_matrix_vector_norm(norm_i, x);
  mpfi_mid(factor_r, norm_i);
  int i, j, index, ret = 0;
  if(mpfr_cmp_ui(factor_r, 0) > 0){
    mpfr_ui_div(factor_r, 1, factor_r, GMP_RNDN);
    mpfr_mul(factor_r, factor_r, length, GMP_RNDN);
    for(j = 0; j < x->column; j++){
      index = j * x->row;
      for(i = 0; i < x->row; i++){
	mpfi_mul_fr(new->data + i + index, x->data + i + index, factor_r);
      }
    }
  }else{
    ret = 1;
  }
  r_mpfi_temp_free(norm_i);
  r_mpfr_temp_free(factor_r);
  return ret;
}

/* ---------------------- square matrix ------------------------- */

/* Return 0 if we execute even permutation for matrix, 1 if odd permutation or */
/* -1 if matrix is singular. */
int mpfi_square_matrix_lu_decomp (MPFIMatrix *ret, int *indx, MPFIMatrix *x){
  int i, j, k, imax, ret_val = 0;
  MPFI *big, *sum, *dum, *tmp1, *tmp2;
  MPFIMatrix *vv, *tmp_ret;
  r_mpfi_matrix_temp_alloc_init(tmp_ret, ret->row, ret->column);

  mpfi_matrix_set(tmp_ret, x);

  r_mpfi_temp_alloc_init(big);
  r_mpfi_temp_alloc_init(sum);
  r_mpfi_temp_alloc_init(dum);
  r_mpfi_temp_alloc_init(tmp1);
  r_mpfi_temp_alloc_init(tmp2);
  r_mpfi_matrix_temp_alloc_init(vv, x->size, 1);
  for (i = 0; i < tmp_ret->row; i++) {
    mpfi_set_si(big, 0);
    for (j = 0; j < tmp_ret->column; j++) {
      mpfi_abs(tmp1, mpfi_matrix_get_element(x, i, j));

      mpfi_intersect(tmp2, tmp1, big);
      if(mpfi_is_empty(tmp2) > 0){
	if(mpfr_cmp(r_mpfi_right_ptr(tmp1), r_mpfi_left_ptr(big)) > 0){
	  mpfi_set(big, tmp1);
	}
      }else{
	mpfi_union(big, tmp1, big);
      }
    }
    if (mpfi_has_zero(big) > 0) {
      ret_val = -1;
      break;
    }
    mpfi_ui_div(vv->data + i, 1, big);
  }

  if (ret_val >= 0) {
    for (j = 0; j < tmp_ret->column; j++) {
      for (i = 0; i < j; i++) {
	mpfi_set(sum, mpfi_matrix_get_element(tmp_ret, i, j));
	for (k = 0; k < i; k++) {
	  mpfi_mul(tmp1, mpfi_matrix_get_element(tmp_ret, i, k), mpfi_matrix_get_element(tmp_ret, k, j));
	  mpfi_sub(sum, sum, tmp1);
	}
	mpfi_set(mpfi_matrix_get_element(tmp_ret, i, j), sum);
      }
      mpfi_set_si(big, 0);
      imax = j;
      for (i = j; i < tmp_ret->row; i++) {
        mpfi_set(sum, mpfi_matrix_get_element(tmp_ret, i, j));
	for (k = 0; k < j; k++) {
	  mpfi_mul(tmp1, mpfi_matrix_get_element(tmp_ret, i, k), mpfi_matrix_get_element(tmp_ret, k, j));
	  mpfi_sub(sum, sum, tmp1);
	}
	mpfi_set(mpfi_matrix_get_element(tmp_ret, i, j), sum);
	mpfi_abs(dum, sum);
	mpfi_mul(dum, vv->data + i, dum);

	mpfi_intersect(tmp2, dum, big);
	if(mpfi_is_empty(tmp2) > 0){
	  if(mpfr_cmp(r_mpfi_right_ptr(dum), r_mpfi_left_ptr(big)) > 0){
	    mpfi_set(big, dum);
	    imax = i;
	  }
	}else{
	  mpfi_union(big, dum, big);
	  imax = i;
	}
      }
      if (j != imax) {
	for (k = 0; k < tmp_ret->column; k++) {
	  mpfi_set(dum, mpfi_matrix_get_element(tmp_ret, imax, k));
	  mpfi_set(mpfi_matrix_get_element(tmp_ret, imax, k), mpfi_matrix_get_element(tmp_ret, j, k));
	  mpfi_set(mpfi_matrix_get_element(tmp_ret, j, k), dum);
	}
	ret_val = (ret_val + 1) % 2;
	mpfi_set(vv->data + imax, vv->data + j);
      }
      indx[j] = imax;
      if (mpfi_has_zero(mpfi_matrix_get_element(tmp_ret, j, j)) > 0) {
	ret_val = -1;
	break;
      }
      if (j < tmp_ret->row - 1) {
	mpfi_ui_div(dum, 1, mpfi_matrix_get_element(tmp_ret, j, j));
	for (i = j + 1; i < tmp_ret->row; i++) {
	  mpfi_mul(mpfi_matrix_get_element(tmp_ret, i, j), mpfi_matrix_get_element(tmp_ret, i, j), dum);
	}
      }
    }
  }
  mpfi_matrix_set(ret, tmp_ret);
  
  r_mpfi_matrix_temp_free(tmp_ret);
  r_mpfi_temp_free(big);
  r_mpfi_temp_free(sum);
  r_mpfi_temp_free(dum);
  r_mpfi_temp_free(tmp1);
  r_mpfi_temp_free(tmp2);
  r_mpfi_matrix_temp_free(vv);
  return ret_val;
}

void mpfi_2d_square_matrix_determinant(MPFI *det, MPFIMatrix *x){
  MPFI *tmp;
  r_mpfi_temp_alloc_init(tmp);
  mpfi_mul(det, x->data, x->data + 3);
  mpfi_mul(tmp, x->data + 1, x->data + 2);
  mpfi_sub(det, det, tmp);
  r_mpfi_temp_free(tmp);
}

void mpfi_3d_square_matrix_determinant(MPFI *det, MPFIMatrix *x){
  MPFI *tmp;
  r_mpfi_temp_alloc_init(tmp);

  mpfi_mul(tmp, mpfi_matrix_get_element(x, 0, 0), mpfi_matrix_get_element(x, 1, 1));
  mpfi_mul(tmp, mpfi_matrix_get_element(x, 2, 2), tmp);
  mpfi_set(det, tmp);

  mpfi_mul(tmp, mpfi_matrix_get_element(x, 0, 1), mpfi_matrix_get_element(x, 1, 2));
  mpfi_mul(tmp, mpfi_matrix_get_element(x, 2, 0), tmp);
  mpfi_add(det, det, tmp);

  mpfi_mul(tmp, mpfi_matrix_get_element(x, 0, 2), mpfi_matrix_get_element(x, 1, 0));
  mpfi_mul(tmp, mpfi_matrix_get_element(x, 2, 1), tmp);
  mpfi_add(det, det, tmp);

  mpfi_mul(tmp, mpfi_matrix_get_element(x, 0, 0), mpfi_matrix_get_element(x, 1, 2));
  mpfi_mul(tmp, mpfi_matrix_get_element(x, 2, 1), tmp);
  mpfi_sub(det, det, tmp);
  
  mpfi_mul(tmp, mpfi_matrix_get_element(x, 0, 1), mpfi_matrix_get_element(x, 1, 0));
  mpfi_mul(tmp, mpfi_matrix_get_element(x, 2, 2), tmp);
  mpfi_sub(det, det, tmp);
  
  mpfi_mul(tmp, mpfi_matrix_get_element(x, 0, 2), mpfi_matrix_get_element(x, 1, 1));
  mpfi_mul(tmp, mpfi_matrix_get_element(x, 2, 0), tmp);
  mpfi_sub(det, det, tmp);
  
  r_mpfi_temp_free(tmp);
}

void mpfi_square_matrix_determinant(MPFI *det, MPFIMatrix *x){
  if (x->column == 2 && x->row == 2){
    mpfi_2d_square_matrix_determinant(det, x);
  }else if(x->column == 3 && x->row == 3){
    mpfi_3d_square_matrix_determinant(det, x);
  }else{
    MPFIMatrix *ptr_lu;
    r_mpfi_matrix_temp_alloc_init(ptr_lu, x->row, x->column);
    int indx[x->row], i;
    if((i = mpfi_square_matrix_lu_decomp (ptr_lu, indx, x)) >= 0){
      if (i == 0) {
	mpfi_set_si(det, 1);
      } else if (i == 1) {
	mpfi_set_si(det, -1);
      }
      for (i = 0; i < x->row; i++) {
        mpfi_mul(det, det, mpfi_matrix_get_element(ptr_lu, i, i));
      }
    }else{
      mpfi_set_ui(det, 0);
    }
    r_mpfi_matrix_temp_free(ptr_lu);
  }

}

void mpfi_square_matrix_qr_decomp(MPFIMatrix *q, MPFIMatrix *r, MPFIMatrix *x){
  MPFIMatrix *q_mat, *r_mat;
  r_mpfi_matrix_temp_alloc_init(q_mat, q->row, q->column);
  r_mpfi_matrix_temp_alloc_init(r_mat, r->row, r->column);

  int size = x->row;
  MPFIMatrix *ary;
  r_mpfi_matrix_temp_alloc_init(ary, size, size);
  mpfi_matrix_set(ary, x);
  MPFI *tmp;
  r_mpfi_temp_alloc_init(tmp);
  int i, j, k, ind1, ind2, ind3;
  for (i = 0; i < size; i++) {
    ind1 = i * size;
    ind2 = i + ind1;
    mpfi_set_si(r_mat->data + ind2, 0);
    for (j = 0; j < size; j++) {
      mpfi_mul(tmp, ary->data + j + ind1, ary->data + j + ind1);
      mpfi_add(r_mat->data + ind2, r_mat->data + ind2, tmp);
    }
    mpfi_sqrt(r_mat->data + ind2, r_mat->data + ind2);
    for (j = 0; j < size; j++) {
      mpfi_div(q_mat->data + j + ind1, ary->data + j + ind1, r_mat->data + ind2);
    }
    for (j = (i + 1); j < size; j++) {
      ind2 = j * size;
      ind3 = i + ind2;
      mpfi_set_si(r_mat->data + ind3, 0);
      for (k = 0; k < size; k++) {
        mpfi_mul(tmp, q_mat->data + k + ind1, ary->data + k + ind2);
	mpfi_add(r_mat->data + ind3, r_mat->data + ind3, tmp);
      }
      for (k = 0; k < size; k++) {
        mpfi_mul(tmp, r_mat->data + ind3, q_mat->data + k + ind1);
	mpfi_sub(ary->data + k + ind2, ary->data + k + ind2, tmp);
      }
    }
  }
  mpfi_matrix_set(q, q_mat);
  mpfi_matrix_set(r, r_mat);

  r_mpfi_matrix_temp_free(q_mat);
  r_mpfi_matrix_temp_free(r_mat);
  r_mpfi_matrix_temp_free(ary);
  r_mpfi_temp_free(tmp);
}

/* If inverse matrix does not exist, return 1. Otherwise return 0. */
int mpfi_2d_square_matrix_inverse_matrix(MPFIMatrix *inv, MPFIMatrix *x){
  MPFIMatrix *t_mat;
  r_mpfi_matrix_temp_alloc_init(t_mat, inv->row, inv->column);
  MPFI *t_fi;
  r_mpfi_temp_alloc_init(t_fi);
  mpfi_2d_square_matrix_determinant(t_fi, x);
  if(mpfi_has_zero(t_fi) > 0){
    return 1;
  }else{
    mpfi_ui_div(t_fi, 1, t_fi);
    mpfi_mul(t_mat->data, x->data + 3, t_fi);
    mpfi_mul(t_mat->data + 3, x->data, t_fi);
    mpfi_neg(t_fi, t_fi);
    mpfi_mul(t_mat->data + 1, x->data + 1, t_fi);
    mpfi_mul(t_mat->data + 2, x->data + 2, t_fi);
  }
  mpfi_matrix_set(inv, t_mat);
  r_mpfi_matrix_temp_free(t_mat);
  r_mpfi_temp_free(t_fi);
  return 0;
}

/* x = -(sqrt(a11**2-2*a00*a11+4*a01*a10+a00**2)-a11-a00)/2.0E+0 */
/* x = (sqrt(a11**2-2*a00*a11+4*a01*a10+a00**2)+a11+a00)/2.0E+0 */
/* If there are two real eigenvalues, return positive number. */
/* If only one eigenvalue exists, return 0. */
/* If there are two complex eigenvalues, this functionreturn negative number and */
/* first returned value is real part and second one is imaginary part. */
int mpfi_2d_square_matrix_eigenvalue(MPFI *val1, MPFI *val2, MPFIMatrix *x){
  int ret;
  MPFI *d;
  r_mpfi_temp_alloc_init(d);

  mpfi_sub(val1, x->data, x->data + 3);
  mpfi_mul(val1, val1, val1);
  mpfi_mul(d, x->data + 1, x->data + 2);
  mpfi_mul_ui(d, d, 4);
  mpfi_add(d, d, val1);

  mpfi_add(val1, x->data, x->data + 3);
  mpfi_div_ui(val1, val1, 2);
  if(mpfr_cmp_si(r_mpfi_right_ptr(d), 0) > 0){
    ret = 1;
    mpfi_sqrt(d, d);
    mpfi_div_ui(d, d, 2);
    mpfi_sub(val2, val1, d);
    mpfi_add(val1, val1, d);
  }else if(mpfr_cmp_si(r_mpfi_right_ptr(d), 0) < 0){
    ret = -1;
    mpfi_neg(d, d);
    mpfi_sqrt(d, d);
    mpfi_div_ui(val2, d, 2);
  }else{
    ret = 0;
    mpfi_set(val2, val1);
  }

  r_mpfi_temp_free(d);
  return ret;
}

void mpfi_2d_square_matrix_real_eigenvector(MPFIMatrix *vec, MPFIMatrix *x, MPFI *eigenval){
  MPFIMatrix *tmp;
  r_mpfi_matrix_temp_alloc_init(tmp, 2, 1);
  MPFI *tmp_fi;
  r_mpfi_temp_alloc_init(tmp_fi);
  mpfi_sub(tmp_fi, x->data + 3, eigenval);
  if(mpfi_has_zero(x->data + 1) > 0 && mpfi_has_zero(tmp_fi) > 0){
    mpfi_set(tmp->data, x->data + 2);
    mpfi_sub(tmp->data + 1, eigenval, x->data);
  }else{
    mpfi_sub(tmp->data, eigenval, x->data + 3);
    mpfi_set(tmp->data + 1, x->data + 1);
  }
  if(mpfi_vector_normalize(vec, tmp) == 1){
    gmp_printf("Argument matrix\n%.Ff\n%.Ff\n%.Ff\n%.Ff\n", x->data, x->data + 1, x->data + 2, x->data + 3);
    gmp_printf("Argument eigenvalue\n%.Ff\n", eigenval);
    rb_raise(rb_eArgError, "Invalid eigenvalue or eigenvector.");
  }
  r_mpfi_matrix_temp_free(tmp);
  r_mpfi_temp_free(tmp_fi);
}

void mpfi_square_matrix_identity(MPFIMatrix *id){
  int i, j, index;
  for (j = 0; j < id->column; j++) {
    index = j * id->row;
    for (i = 0; i < id->row; i++) {
      if(i == j){
	mpfi_set_si(id->data + i + index, 1);
      }else{
	mpfi_set_si(id->data + i + index, 0);
      }
    }
  }
  
}
