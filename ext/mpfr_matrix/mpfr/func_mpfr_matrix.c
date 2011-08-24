#include "func_mpfr_matrix.h"

void mpfr_matrix_init(MPFRMatrix *mat, int row, int column){
  int i;
  mat->row = row;
  mat->column = column;
  mat->size = row * column;
  /* mat->data = (MPFR *)malloc(sizeof(MPF) * mat->size); */
  mat->data = ALLOC_N(MPFR, mat->size);
  for(i = 0; i < mat->size; i++){
    mpfr_init(mat->data + i);
  }
}

void mpfr_matrix_set_zeros(MPFRMatrix *mat){
  int i;
  for(i = 0; i < mat->size; i++){
    mpfr_set_si(mat->data + i, 0, GMP_RNDN);
  }
}

void mpfr_matrix_clear(MPFRMatrix *mat){
  int i;
  for(i = 0; i < mat->size; i++){
    mpfr_clear(mat->data + i);
  }
  free(mat->data);
}

void mpfr_matrix_set_element(MPFRMatrix *mat, int row, int col, MPFR *a){
  mpfr_set(mpfr_matrix_get_element(mat, row, col), a, GMP_RNDN);
}

void mpfr_matrix_set(MPFRMatrix *new, MPFRMatrix *x){
  int i;
  for(i = 0; i < x->size; i++){
    mpfr_set(new->data + i, x->data + i, GMP_RNDN);
  }
}

void mpfr_matrix_swap(MPFRMatrix *x, MPFRMatrix *y){
  int i;
  for (i = 0; i < x->size; i++) {
    mpfr_swap(mpfr_matrix_get_ptr(x, i), mpfr_matrix_get_ptr(y, i));
  }
}

void mpfr_matrix_row(MPFRMatrix *new, MPFRMatrix *x, int row){
  int i;
  for (i = 0; i < x->column; i++) {
    mpfr_set(new->data + i, mpfr_matrix_get_element(x, row, i), GMP_RNDN);
  }
}

void mpfr_matrix_column(MPFRMatrix *new, MPFRMatrix *x, int column){
  int i;
  for (i = 0; i < x->row; i++) {
    mpfr_set(new->data + i, mpfr_matrix_get_element(x, i, column), GMP_RNDN);
  }
}

void mpfr_matrix_transpose(MPFRMatrix *new, MPFRMatrix *x){
  int i, j, index;
  for(j = 0; j < x->column; j++){
    index = j * x->row;
    for(i = 0; i < x->row; i++){
      mpfr_set(new->data + j + i * new->row, x->data + i + index, GMP_RNDN);
    }
  }
}

void mpfr_matrix_neg(MPFRMatrix *new, MPFRMatrix *x){
  int i;
  for(i = 0; i < x->size; i++){
    mpfr_neg(new->data + i, x->data + i, GMP_RNDN);
  }
}

/* Return 0 if *x and *y has the same elements. Otherwise return 1. */
int mpfr_matrix_equal_p(MPFRMatrix *x, MPFRMatrix *y){
  int i, ret = 0;
  if (x->column == y->column && x->row == y->row) {
    for (i = 0; i < x->size; i++) {
      if (mpfr_cmp(x->data + i, y->data + i) != 0) {
	ret = 1;
	break;
      }
    }
  }else{
    ret = 1;
  }
  return ret;
}

void mpfr_matrix_add(MPFRMatrix *new, MPFRMatrix *x, MPFRMatrix *y){
  int i, j, index;
  for(j = 0; j < x->column; j++){
    index = j * x->row;
    for(i = 0; i < x->row; i++){
      mpfr_add(new->data + i + index, x->data + i + index, y->data + i + index, GMP_RNDN);
    }
  }
}

void mpfr_matrix_sub(MPFRMatrix *new, MPFRMatrix *x, MPFRMatrix *y){
  int i, j, index;
  for(j = 0; j < x->column; j++){
    index = j * x->row;
    for(i = 0; i < x->row; i++){
      mpfr_sub(new->data + i + index, x->data + i + index, y->data + i + index, GMP_RNDN);
    }
  }
}

/* x and new must be different pointer from each other. */
void mpfr_matrix_mul(MPFRMatrix *new, MPFRMatrix *x, MPFRMatrix *y){
  int i, j, k, index, index_y;
  MPFR *tmp;
  r_mpfr_temp_alloc_init(tmp);
  for(i = 0; i < new->size; i++){
    mpfr_set_si(new->data + i, 0, GMP_RNDN);
  }
  for(j = 0; j < y->column; j++){
    for(i = 0; i < x->row; i++){
      index = i + j * new->row;
      index_y = j * y->row;
      for(k = 0; k < x->column; k++){
	mpfr_mul(tmp, x->data + i + k * x->row, y->data + k + index_y, GMP_RNDN);
	mpfr_add(new->data + index, new->data + index, tmp, GMP_RNDN);
      }
    }
  }
  r_mpfr_temp_free(tmp);
}

void mpfr_matrix_mul_scalar(MPFRMatrix *new, MPFRMatrix *x, MPFR *scalar){
  int i;
  for(i = 0; i < x->size; i++){
    mpfr_mul(new->data + i, x->data + i, scalar, GMP_RNDN);
  }
}

void mpfr_matrix_div_scalar(MPFRMatrix *new, MPFRMatrix *x, MPFR *scalar){
  int i;
  for(i = 0; i < x->size; i++){
    mpfr_div(new->data + i, x->data + i, scalar, GMP_RNDN);
  }
}

void mpfr_matrix_inner_product(MPFR *pr, MPFRMatrix *x, MPFRMatrix *y){
  MPFR *tmp;
  int i;
  r_mpfr_temp_alloc_init(tmp);
  mpfr_set_si(pr, 0, GMP_RNDN);
  for(i = 0; i < x->size; i++){
    mpfr_mul(tmp, x->data + i, y->data + i, GMP_RNDN);
    mpfr_add(pr, pr, tmp, GMP_RNDN);
  }
  r_mpfr_temp_free(tmp);
}

void mpfr_matrix_vector_distance(MPFR *distance, MPFRMatrix *x, MPFRMatrix *y){
  MPFR *tmp;
  int i;
  r_mpfr_temp_alloc_init(tmp);
  mpfr_set_si(distance, 0, GMP_RNDN);
  for(i = 0; i < x->size; i++){
    mpfr_sub(tmp, x->data + i, y->data + i, GMP_RNDN);
    mpfr_mul(tmp, tmp, tmp, GMP_RNDN);
    mpfr_add(distance, distance, tmp, GMP_RNDN);
  }
  mpfr_sqrt(distance, distance, GMP_RNDN);
  r_mpfr_temp_free(tmp);
}

void mpfr_matrix_vector_norm(MPFR *norm, MPFRMatrix *x){
  MPFR *tmp;
  int i;
  r_mpfr_temp_alloc_init(tmp);
  mpfr_set_si(norm, 0, GMP_RNDN);
  for(i = 0; i < x->size; i++){
    mpfr_mul(tmp, x->data + i, x->data + i, GMP_RNDN);
    mpfr_add(norm, norm, tmp, GMP_RNDN);
  }
  mpfr_sqrt(norm, norm, GMP_RNDN);
  r_mpfr_temp_free(tmp);
}

void mpfr_matrix_max_norm(MPFR *norm, MPFRMatrix *x){
  MPFR *tmp;
  int i;
  r_mpfr_temp_alloc_init(tmp);
  mpfr_set_si(norm, 0, GMP_RNDN);
  for(i = 0; i < x->size; i++){
    mpfr_abs(tmp, x->data + i, GMP_RNDN);
    if(mpfr_cmp(tmp, norm) > 0){
      mpfr_set(norm, tmp, GMP_RNDN);
    }
  }
  r_mpfr_temp_free(tmp);
}

void mpfr_col_vector_init(MPFRMatrix *mat, int row){
  int i;
  mat->row = row;
  mat->column = 1;
  mat->size = row;
  /* mat->data = (MPFR *)malloc(sizeof(MPF) * mat->size); */
  mat->data = ALLOC_N(MPFR, mat->size);
  for(i = 0; i < mat->size; i++){
    mpfr_init(mat->data + i);
  }
}

void mpfr_row_vector_init(MPFRMatrix *mat, int column){
  int i;
  mat->row = 1;
  mat->column = column;
  mat->size = column;
  /* mat->data = (MPFR *)malloc(sizeof(MPF) * mat->size); */
  mat->data = ALLOC_N(MPFR, mat->size);
  for(i = 0; i < mat->size; i++){
    mpfr_init(mat->data + i);
  }
}

/* If length of MPFRMatrix *x is zero, return 1. Otherwise return 0. */
int mpfr_vector_normalize(MPFRMatrix *new, MPFRMatrix *x){
  MPFR *norm;
  int i, j, index, ret = 0;
  r_mpfr_temp_alloc_init(norm);
  mpfr_matrix_vector_norm(norm, x);
  if(mpfr_cmp_ui(norm, 0) > 0){
    for(j = 0; j < x->column; j++){
      index = j * x->row;
      for(i = 0; i < x->row; i++){
	mpfr_div(new->data + i + index, x->data + i + index, norm, GMP_RNDN);
      }
    }
  }else{
    ret = 1;
  }
  r_mpfr_temp_free(norm);
  return ret;
}

/* If length of MPFRMatrix *x is zero, return 1. Otherwise return 0. */
int mpfr_vector_set_length(MPFRMatrix *new, MPFRMatrix *x, MPFR *length){
  MPFR *factor;
  int i, j, index, ret = 0;
  r_mpfr_temp_alloc_init(factor);
  mpfr_matrix_vector_norm(factor, x);
  if(mpfr_cmp_ui(factor, 0) > 0){
    mpfr_ui_div(factor, 1, factor, GMP_RNDN);
    mpfr_mul(factor, factor, length, GMP_RNDN);
    for(j = 0; j < x->column; j++){
      index = j * x->row;
      for(i = 0; i < x->row; i++){
	mpfr_mul(new->data + i + index, x->data + i + index, factor, GMP_RNDN);
      }
    }
  }else{
    ret = 1;
  }
  r_mpfr_temp_free(factor);
  return ret;
}

void mpfr_vector_midpoint(MPFRMatrix *new, MPFRMatrix *x, MPFRMatrix *y){
  int i;
  for (i = 0; i < new->size; i++) {
    mpfr_add(mpfr_matrix_get_ptr(new, i), mpfr_matrix_get_ptr(x, i), mpfr_matrix_get_ptr(y, i), GMP_RNDN);
    mpfr_div_ui(mpfr_matrix_get_ptr(new, i), mpfr_matrix_get_ptr(new, i), 2, GMP_RNDN);
  }
}

/* "distance between *new and *x" / "distance between *y and *x" = *div */
void mpfr_vector_dividing_point(MPFRMatrix *new, MPFRMatrix *x, MPFRMatrix *y, MPFR *div){
  MPFRMatrix *tmp_x, *tmp_y;
  MPFR *ratio;
  r_mpfr_matrix_temp_alloc_init(tmp_x, new->row, new->column);
  r_mpfr_matrix_temp_alloc_init(tmp_y, new->row, new->column);

  r_mpfr_temp_alloc_init(ratio);
  mpfr_ui_sub(ratio, 1, div, GMP_RNDN);
  mpfr_matrix_mul_scalar(tmp_y, y, ratio);
  mpfr_matrix_mul_scalar(tmp_x, x, div);
  mpfr_matrix_add(new, tmp_x, tmp_y);

  r_mpfr_matrix_temp_free(tmp_x);
  r_mpfr_matrix_temp_free(tmp_y);
  r_mpfr_temp_free(ratio);
}

/* If length of MPFRMatrix *x, return 1. Set normalized normal vector to MPFRMatrix *new. */
int mpfr_2d_normal_vector(MPFRMatrix *new, MPFRMatrix *x){
  int ret = 0;
  if (mpfr_vector_normalize(new, x) == 0){
    mpfr_swap(new->data, new->data + 1);
    mpfr_neg(new->data + 1, new->data + 1, GMP_RNDN);
  }else{
    ret = 1;
  }
  return ret;
}

static void mpfr_2d_square_matrix_determinant(MPFR *det, MPFRMatrix *x){
  MPFR *tmp;
  r_mpfr_temp_alloc_init(tmp);
  mpfr_mul(det, x->data, x->data + 3, GMP_RNDN);
  mpfr_mul(tmp, x->data + 1, x->data + 2, GMP_RNDN);
  mpfr_sub(det, det, tmp, GMP_RNDN);
  r_mpfr_temp_free(tmp);
}

static void mpfr_3d_square_matrix_determinant(MPFR *det, MPFRMatrix *x){
  MPFR *tmp;
  r_mpfr_temp_alloc_init(tmp);

  mpfr_mul(tmp, mpfr_matrix_get_element(x, 0, 0), mpfr_matrix_get_element(x, 1, 1), GMP_RNDN);
  mpfr_mul(tmp, mpfr_matrix_get_element(x, 2, 2), tmp, GMP_RNDN);
  mpfr_set(det, tmp, GMP_RNDN);

  mpfr_mul(tmp, mpfr_matrix_get_element(x, 0, 1), mpfr_matrix_get_element(x, 1, 2), GMP_RNDN);
  mpfr_mul(tmp, mpfr_matrix_get_element(x, 2, 0), tmp, GMP_RNDN);
  mpfr_add(det, det, tmp, GMP_RNDN);

  mpfr_mul(tmp, mpfr_matrix_get_element(x, 0, 2), mpfr_matrix_get_element(x, 1, 0), GMP_RNDN);
  mpfr_mul(tmp, mpfr_matrix_get_element(x, 2, 1), tmp, GMP_RNDN);
  mpfr_add(det, det, tmp, GMP_RNDN);

  mpfr_mul(tmp, mpfr_matrix_get_element(x, 0, 0), mpfr_matrix_get_element(x, 1, 2), GMP_RNDN);
  mpfr_mul(tmp, mpfr_matrix_get_element(x, 2, 1), tmp, GMP_RNDN);
  mpfr_sub(det, det, tmp, GMP_RNDN);
  
  mpfr_mul(tmp, mpfr_matrix_get_element(x, 0, 1), mpfr_matrix_get_element(x, 1, 0), GMP_RNDN);
  mpfr_mul(tmp, mpfr_matrix_get_element(x, 2, 2), tmp, GMP_RNDN);
  mpfr_sub(det, det, tmp, GMP_RNDN);
  
  mpfr_mul(tmp, mpfr_matrix_get_element(x, 0, 2), mpfr_matrix_get_element(x, 1, 1), GMP_RNDN);
  mpfr_mul(tmp, mpfr_matrix_get_element(x, 2, 0), tmp, GMP_RNDN);
  mpfr_sub(det, det, tmp, GMP_RNDN);
  
  r_mpfr_temp_free(tmp);
}

/* Return 0 if we execute even permutation for matrix, 1 if odd permutation or */
/* -1 if matrix is singular. */
int mpfr_square_matrix_lu_decomp (MPFRMatrix *ret, int *indx, MPFRMatrix *x){
  int i, j, k, imax, ret_val = 0;
  MPFR *big, *sum, *dum, *tmp;
  MPFRMatrix *vv, *tmp_ret;
  r_mpfr_matrix_temp_alloc_init(tmp_ret, ret->row, ret->column);

  mpfr_matrix_set(tmp_ret, x);

  r_mpfr_temp_alloc_init(big);
  r_mpfr_temp_alloc_init(sum);
  r_mpfr_temp_alloc_init(dum);
  r_mpfr_temp_alloc_init(tmp);
  r_mpfr_matrix_temp_alloc_init(vv, x->size, 1);
  for (i = 0; i < tmp_ret->row; i++) {
    mpfr_set_si(big, 0, GMP_RNDN);
    for (j = 0; j < tmp_ret->column; j++) {
      mpfr_abs(tmp, mpfr_matrix_get_element(x, i, j), GMP_RNDN);
      if (mpfr_cmp(tmp, big) > 0) {
        mpfr_set(big, tmp, GMP_RNDN);
      }
    }
    if (mpfr_cmp_si(big, 0) == 0) {
      ret_val = -1;
      break;
    }
    mpfr_ui_div(vv->data + i, 1, big, GMP_RNDN);
  }

  if (ret_val >= 0) {
    for (j = 0; j < tmp_ret->column; j++) {
      for (i = 0; i < j; i++) {
	mpfr_set(sum, mpfr_matrix_get_element(tmp_ret, i, j), GMP_RNDN);
	for (k = 0; k < i; k++) {
	  mpfr_mul(tmp, mpfr_matrix_get_element(tmp_ret, i, k), mpfr_matrix_get_element(tmp_ret, k, j), GMP_RNDN);
	  mpfr_sub(sum, sum, tmp, GMP_RNDN);
	}
	mpfr_set(mpfr_matrix_get_element(tmp_ret, i, j), sum, GMP_RNDN);
      }
      mpfr_set_si(big, 0, GMP_RNDN);
      imax = j;
      for (i = j; i < tmp_ret->row; i++) {
        mpfr_set(sum, mpfr_matrix_get_element(tmp_ret, i, j), GMP_RNDN);
	for (k = 0; k < j; k++) {
	  mpfr_mul(tmp, mpfr_matrix_get_element(tmp_ret, i, k), mpfr_matrix_get_element(tmp_ret, k, j), GMP_RNDN);
	  mpfr_sub(sum, sum, tmp, GMP_RNDN);
	}
	mpfr_set(mpfr_matrix_get_element(tmp_ret, i, j), sum, GMP_RNDN);
	mpfr_abs(dum, sum, GMP_RNDN);
	mpfr_mul(dum, vv->data + i, dum, GMP_RNDN);
	if (mpfr_cmp(dum, big) >= 0) {
	  mpfr_set(big, dum, GMP_RNDN);
	  imax = i;
	}
      }
      if (j != imax) {
	for (k = 0; k < tmp_ret->column; k++) {
	  mpfr_set(dum, mpfr_matrix_get_element(tmp_ret, imax, k), GMP_RNDN);
	  mpfr_set(mpfr_matrix_get_element(tmp_ret, imax, k), mpfr_matrix_get_element(tmp_ret, j, k), GMP_RNDN);
	  mpfr_set(mpfr_matrix_get_element(tmp_ret, j, k), dum, GMP_RNDN);
	}
	ret_val = (ret_val + 1) % 2;
	mpfr_set(vv->data + imax, vv->data + j, GMP_RNDN);
      }
      indx[j] = imax;
      if (mpfr_cmp_si(mpfr_matrix_get_element(tmp_ret, j, j), 0) == 0) {
	ret_val = -1;
	break;
      }
      if (j < tmp_ret->row - 1) {
	mpfr_ui_div(dum, 1, mpfr_matrix_get_element(tmp_ret, j, j), GMP_RNDN);
	for (i = j + 1; i < tmp_ret->row; i++) {
	  mpfr_mul(mpfr_matrix_get_element(tmp_ret, i, j), mpfr_matrix_get_element(tmp_ret, i, j), dum, GMP_RNDN);
	}
      }
    }
  }
  mpfr_matrix_set(ret, tmp_ret);
  
  r_mpfr_matrix_temp_free(tmp_ret);
  r_mpfr_temp_free(big);
  r_mpfr_temp_free(sum);
  r_mpfr_temp_free(dum);
  r_mpfr_temp_free(tmp);
  r_mpfr_matrix_temp_free(vv);
  return ret_val;
}

void mpfr_square_matrix_determinant(MPFR *det, MPFRMatrix *x){
  if (x->column == 2 && x->row == 2){
    mpfr_2d_square_matrix_determinant(det, x);
  }else if(x->column == 3 && x->row == 3){
    mpfr_3d_square_matrix_determinant(det, x);
  }else{
    MPFRMatrix *ptr_lu;
    int i, *indx;
    r_mpfr_matrix_temp_alloc_init(ptr_lu, x->row, x->column);
    indx = (int *)malloc(sizeof(int) * x->row);
    if((i = mpfr_square_matrix_lu_decomp (ptr_lu, indx, x)) >= 0){
      if (i == 0) {
	mpfr_set_si(det, 1, GMP_RNDN);
      } else if (i == 1) {
	mpfr_set_si(det, -1, GMP_RNDN);
      }
      for (i = 0; i < x->row; i++) {
        mpfr_mul(det, det, mpfr_matrix_get_element(ptr_lu, i, i), GMP_RNDN);
      }
    }else{
      mpfr_set_ui(det, 0, GMP_RNDN);
    }
    r_mpfr_matrix_temp_free(ptr_lu);
    free(indx);
  }
}

void mpfr_square_matrix_qr_decomp(MPFRMatrix *q, MPFRMatrix *r, MPFRMatrix *x){
  MPFRMatrix *q_mat, *r_mat, *ary;
  int i, j, k, ind1, ind2, ind3, size;
  MPFR *tmp;
  r_mpfr_matrix_temp_alloc_init(q_mat, q->row, q->column);
  r_mpfr_matrix_temp_alloc_init(r_mat, r->row, r->column);

  size = x->row;
  r_mpfr_matrix_temp_alloc_init(ary, size, size);
  mpfr_matrix_set(ary, x);
  r_mpfr_temp_alloc_init(tmp);
  for (i = 0; i < size; i++) {
    ind1 = i * size;
    ind2 = i + ind1;
    mpfr_set_si(r_mat->data + ind2, 0, GMP_RNDN);
    for (j = 0; j < size; j++) {
      mpfr_mul(tmp, ary->data + j + ind1, ary->data + j + ind1, GMP_RNDN);
      mpfr_add(r_mat->data + ind2, r_mat->data + ind2, tmp, GMP_RNDN);
    }
    mpfr_sqrt(r_mat->data + ind2, r_mat->data + ind2, GMP_RNDN);
    for (j = 0; j < size; j++) {
      mpfr_div(q_mat->data + j + ind1, ary->data + j + ind1, r_mat->data + ind2, GMP_RNDN);
    }
    for (j = (i + 1); j < size; j++) {
      ind2 = j * size;
      ind3 = i + ind2;
      mpfr_set_si(r_mat->data + ind3, 0, GMP_RNDN);
      for (k = 0; k < size; k++) {
        mpfr_mul(tmp, q_mat->data + k + ind1, ary->data + k + ind2, GMP_RNDN);
	mpfr_add(r_mat->data + ind3, r_mat->data + ind3, tmp, GMP_RNDN);
      }
      for (k = 0; k < size; k++) {
        mpfr_mul(tmp, r_mat->data + ind3, q_mat->data + k + ind1, GMP_RNDN);
	mpfr_sub(ary->data + k + ind2, ary->data + k + ind2, tmp, GMP_RNDN);
      }
    }
  }
  mpfr_matrix_set(q, q_mat);
  mpfr_matrix_set(r, r_mat);

  r_mpfr_matrix_temp_free(q_mat);
  r_mpfr_matrix_temp_free(r_mat);
  r_mpfr_matrix_temp_free(ary);
  r_mpfr_temp_free(tmp);
}

void mpfr_square_matrix_identity(MPFRMatrix *id){
  int i, j, index;
  for (j = 0; j < id->column; j++) {
    index = j * id->row;
    for (i = 0; i < id->row; i++) {
      if(i == j){
	mpfr_set_si(id->data + i + index, 1, GMP_RNDN);
      }else{
	mpfr_set_si(id->data + i + index, 0, GMP_RNDN);
      }
    }
  }  
}
