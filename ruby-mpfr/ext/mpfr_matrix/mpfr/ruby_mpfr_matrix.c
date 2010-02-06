#include "ruby_mpfr_matrix.h"

static ID eqq;

void r_mpfr_matrix_free(void *ptr)
{
  mpfr_matrix_clear(ptr);
  free(ptr);
}

void r_mpfr_matrix_suitable_matrix_init (VALUE *other, MPFRMatrix **ptr_other, int row, int column)
{
  if(column == 1){
    r_mpfr_make_col_vector_struct(*other, *ptr_other);
    mpfr_col_vector_init(*ptr_other, row);
  }else if (row == 1){
    r_mpfr_make_row_vector_struct(*other, *ptr_other);
    mpfr_row_vector_init(*ptr_other, column);
  }else{
    if (row == column){
      r_mpfr_make_square_matrix_struct(*other, *ptr_other);
    }else{  
      r_mpfr_make_matrix_struct(*other, *ptr_other);
    }
    mpfr_matrix_init(*ptr_other, row, column);
  }
}

/* VALUE r_mpfr_matrix_copied_robj(VALUE old){ */
/*   MPFRMatrix *ptr_old, *ptr_ret; */
/*   r_mpfr_get_matrix_struct(ptr_old, old); */
/*   VALUE ret; */
/*   r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_old->row, ptr_old->column); */
/*   mpfr_matrix_set(ptr_ret, ptr_old); */
/*   return ret; */
/* } */

VALUE r_mpfr_matrix_robj(MPFRMatrix *x)
{
  VALUE ret;
  MPFRMatrix *ptr_ret;
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, x->row, x->column);
  mpfr_matrix_set(ptr_ret, x);
  return ret;
}

/* Allocation function for MPFR::Matrix. */
static VALUE r_mpfr_matrix_alloc (VALUE self)
{
  MPFRMatrix *ptr;
  r_mpfr_make_matrix_struct(self, ptr);
  return self;
}

/* static void r_mpfr_matrix_init_set_from_rmatrix(MPFRMatrix *x, VALUE src){ */
/*   MPFRMatrix *ptr_src; */
/*   r_mpfr_get_matrix_struct(ptr_src, src); */
/*   mpfr_matrix_init(x, ptr_src->row, ptr_src->column); */
/*   mpfr_matrix_set(x, ptr_src); */
/* } */

static void r_mpfr_matrix_set_initial_value(MPFRMatrix *ptr, int argc, VALUE *argv)
{
  int row, column, i, j;
  VALUE row_ary;
  switch (argc) {
  case 1:
    row = RARRAY_LEN(argv[0]);
    column = RARRAY_LEN(rb_ary_entry(argv[0], 0));
    mpfr_matrix_init(ptr, row, column);
    for (i = 0; i < row; i++) {
      row_ary = rb_ary_entry(argv[0], i);
      if (column != RARRAY_LEN(row_ary)) {
	rb_raise(rb_eArgError, "MPFR::Matrix.new needs Array which has arrays of same sizes.");
      }
      for (j = 0; j < column; j++) {
	r_mpfr_set_robj(mpfr_matrix_get_element(ptr, i, j), rb_ary_entry(row_ary, j), GMP_RNDN);
      }
    }
    break;
  case 2:
    mpfr_matrix_init(ptr, NUM2INT(argv[0]), NUM2INT(argv[1]));
    mpfr_matrix_set_zeros(ptr);
    break;
  default:
    rb_raise(rb_eArgError, "MPFR::Matrix.new needs one or two arguments.");
    break;
  }
}

/* Return new MPFR::Matrix instance. The same arguments as MPFR::Matrix.new is acceptable. */
static VALUE r_mpfr_matrix_global_new(int argc, VALUE *argv, VALUE self)
{
  MPFRMatrix *ptr;
  VALUE val;
  r_mpfr_make_matrix_struct(val, ptr);
  r_mpfr_matrix_set_initial_value(ptr, argc, argv);
  return val;
}

/*
  Initialization function for MPFR::Matrix.
  If this method gets one argument which is two dimensional Array,
  it returns MPFR::Matrix instance of which size is the same as the two dimensional array.
  Getting two argument integers (i, j), it returns MPFR::Matrix instance of which
  size is (i x j).
*/
static VALUE r_mpfr_matrix_initialize (int argc, VALUE *argv, VALUE self)
{
  MPFRMatrix *ptr;
  r_mpfr_get_matrix_struct(ptr, self);
  r_mpfr_matrix_set_initial_value(ptr, argc, argv);
  return Qtrue;
}

/* Allocation function for MPFR::SquareMatrix. */
static VALUE r_mpfr_square_matrix_alloc (VALUE self)
{
  MPFRMatrix *ptr;
  r_mpfr_make_square_matrix_struct(self, ptr);
  return self;
}

static void r_mpfr_square_matrix_set_initial_value(MPFRMatrix *ptr, VALUE arg)
{
  int row, column, i, j;
  VALUE row_ary;
  switch(TYPE(arg)){
  case T_ARRAY:
    row = RARRAY_LEN(arg);
    column = RARRAY_LEN(rb_ary_entry(arg, 0));
    if(row == column){
      mpfr_matrix_init(ptr, row, column);
      for (i = 0; i < row; i++) {
	row_ary = rb_ary_entry(arg, i);
	if (column != RARRAY_LEN(row_ary)) {
	  rb_raise(rb_eArgError, "MPFR::Matrix.new needs Array which has arrays of same sizes.");
	}
	for (j = 0; j < column; j++) {
	  r_mpfr_set_robj(ptr->data + i + j * row, rb_ary_entry(row_ary, j), GMP_RNDN);
	}
      }
    }else{
      rb_raise(rb_eArgError, "MPFR::SquareMatrix.new needs two dimensinal Array which has the same sizes of row and column.");
    }
    break;
  case T_FIXNUM:
    row = NUM2INT(arg);
    mpfr_matrix_init(ptr, row, row);
    mpfr_matrix_set_zeros(ptr);
    break;
  default:
    rb_raise(rb_eArgError, "MPFR::SquareMatrix.new needs Array or Fixnum.");
    break;
  }  
}

/* Return new MPFR::SquareMatrix instance. The same arguments as MPFR::SquareMatrix.new is acceptable. */
static VALUE r_mpfr_square_matrix_global_new(int argc, VALUE arg)
{
  MPFRMatrix *ptr;
  VALUE val;
  r_mpfr_make_square_matrix_struct(val, ptr);
  r_mpfr_square_matrix_set_initial_value(ptr, arg);
  return val;
}

/* Initialization function for MPFR::SquareMatrix. */
static VALUE r_mpfr_square_matrix_initialize (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr;
  r_mpfr_get_matrix_struct(ptr, self);
  r_mpfr_square_matrix_set_initial_value(ptr, arg);
  return Qtrue;
}

/* initialize_copy */
static VALUE r_mpfr_matrix_initialize_copy (VALUE self, VALUE other)
{
  MPFRMatrix *ptr_self, *ptr_other;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_other, other);
  mpfr_matrix_init(ptr_self, ptr_other->row, ptr_other->column);
  mpfr_matrix_set(ptr_self, ptr_other);
  return Qtrue;
}

/* Allocation function for MPFR::ColumnVector. */
static VALUE r_mpfr_col_vector_alloc (VALUE self)
{
  MPFRMatrix *ptr;
  r_mpfr_make_col_vector_struct(self, ptr);
  return self;
}

static void r_mpfr_col_vector_set_initial_value(MPFRMatrix *ptr, VALUE arg)
{
  int row, i;
  switch(TYPE(arg)){
  case T_ARRAY:
    row = RARRAY_LEN(arg);
    mpfr_col_vector_init(ptr, row);
    for (i = 0; i < row; i++) {
      r_mpfr_set_robj(ptr->data + i, rb_ary_entry(arg, i), GMP_RNDN);
    }
    break;
  case T_FIXNUM:
    row = NUM2INT(arg);
    mpfr_col_vector_init(ptr, row);
    for (i = 0; i < row; i++) {
      mpfr_set_si(ptr->data + i, 0, GMP_RNDN);
    }
    break;
  default:
    rb_raise(rb_eArgError, "MPFR::ColumnVector.new needs Array or Fixnum.");
    break;
  }
}

/* Return new MPFR::ColumnVector instance. The same arguments as MPFR::ColumnVector.new is acceptable. */
static VALUE r_mpfr_col_vector_global_new(VALUE self, VALUE arg)
{
  MPFRMatrix *ptr;
  VALUE val;
  r_mpfr_make_col_vector_struct(val, ptr);
  r_mpfr_col_vector_set_initial_value(ptr, arg);
  return val;
}

/* Initialization function for MPFR::ColumnVector. */
static VALUE r_mpfr_col_vector_initialize (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr;
  r_mpfr_get_matrix_struct(ptr, self);
  r_mpfr_col_vector_set_initial_value(ptr, arg);
  return Qtrue;
}

/* Allocation function for MPFR::RowVector. */
static VALUE r_mpfr_row_vector_alloc (VALUE self)
{
  MPFRMatrix *ptr;
  r_mpfr_make_row_vector_struct(self, ptr);
  return self;
}

static void r_mpfr_row_vector_set_initial_value(MPFRMatrix *ptr, VALUE arg)
{
  int column, i;
  switch(TYPE(arg)){
  case T_ARRAY:
    column = RARRAY_LEN(arg);
    mpfr_row_vector_init(ptr, column);
    for (i = 0; i < column; i++) {
      r_mpfr_set_robj(ptr->data + i, rb_ary_entry(arg, i), GMP_RNDN);
    }
    break;
  case T_FIXNUM:
    column = NUM2INT(arg);
    mpfr_row_vector_init(ptr, column);
    for (i = 0; i < column; i++) {
      mpfr_set_si(ptr->data + i, 0, GMP_RNDN);
    }
    break;
  default:
    rb_raise(rb_eArgError, "MPFR::RowVector.new needs Array or Fixnum.");
    break;
  }  
}

/* Return new MPFR::RowVector instance. The same arguments as MPFR::RowVector.new is acceptable. */
static VALUE r_mpfr_row_vector_global_new(int argc, VALUE arg)
{
  MPFRMatrix *ptr;
  VALUE val;
  r_mpfr_make_row_struct(val, ptr);
  r_mpfr_row_vector_set_initial_value(ptr, arg);
  return val;
}

/* Initialization function for MPFR::RowVector. */
static VALUE r_mpfr_row_vector_initialize (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr;
  r_mpfr_get_matrix_struct(ptr, self);
  r_mpfr_row_vector_set_initial_value(ptr, arg);
  return Qtrue;
}

/* Return true if all elements of _self_ equal to corresponding elements of _other_. */
static VALUE r_mpfr_matrix_equal_p (VALUE self, VALUE other)
{
  MPFRMatrix *ptr_self, *ptr_other;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_other, other);
  VALUE ret = Qtrue;
  if (mpfr_matrix_equal_p(ptr_self, ptr_other) != 0) {
    ret = Qnil;
  }
  return ret;
}

/* Return size of data array which equals to column * row. */
static VALUE r_mpfr_matrix_size (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  return INT2FIX(ptr_self->size);
}

/* Return column size of matrix. */
static VALUE r_mpfr_matrix_column_size (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  return INT2FIX(ptr_self->column);
}

/* Return row size of matrix. */
static VALUE r_mpfr_matrix_row_size (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  return INT2FIX(ptr_self->row);
}

/* Return element with _row_ and _column_.*/
static VALUE r_mpfr_matrix_element (VALUE self, VALUE row, VALUE column)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  int i = NUM2INT(row), j = NUM2INT(column);
  if (i < ptr_self->row && j < ptr_self->column) {
    VALUE ret;
    MPFR *ptr_ret;
    r_mpfr_make_struct_init(ret, ptr_ret);
    mpfr_set(ptr_ret, ptr_self->data + i + j * ptr_self->row, GMP_RNDN);
    return ret;
  }else{
    return Qnil;
  }
}

/* Return element at _p1_.*/
static VALUE r_mpfr_matrix_at (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  int i = NUM2INT(arg);
  if (i < ptr_self->size) {
    VALUE ret;
    MPFR *ptr_ret;
    r_mpfr_make_struct_init(ret, ptr_ret);
    mpfr_set(ptr_ret, ptr_self->data + i, GMP_RNDN);
    return ret;
  }else{
    return Qnil;
  }
}

/* Set _robj_ to element of which row is _row_ and column is _column_. */
static VALUE r_mpfr_matrix_set_element (VALUE self, VALUE row, VALUE column, VALUE robj)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  int i = NUM2INT(row), j = NUM2INT(column);
  if (i < ptr_self->row && j < ptr_self->column) {
    r_mpfr_set_robj(mpfr_matrix_get_element(ptr_self, i, j), robj, GMP_RNDN);
  }
  return Qnil;
}

/* Evaluate block with each element of matrix. */
static VALUE r_mpfr_matrix_each_element (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret = Qnil;
  int i, j;
  for (i = 0; i < ptr_self->row; i++) {
    for (j = 0; j < ptr_self->column; j++) {
      volatile VALUE arg = r_mpfr_make_new_fr_obj(mpfr_matrix_get_element(ptr_self, i, j));
      ret = rb_yield(arg);
    }
  }
  return ret;
}

/* Evaluate block with each element and its index. */
static VALUE r_mpfr_matrix_each_element_with_index (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret = Qnil, tmp_i;
  int i, j;
  for (i = 0; i < ptr_self->row; i++) {
    tmp_i = INT2NUM(i);
    for (j = 0; j < ptr_self->column; j++) {
      volatile VALUE arg = r_mpfr_make_new_fr_obj(mpfr_matrix_get_element(ptr_self, i, j));
      ret = rb_yield(rb_ary_new3(3, arg, tmp_i, INT2NUM(j)));
    }
  }
  return ret;
}

/* Return one dimensinal array including strings which elements of matrix is converted to by MPFR#to_strf. */
static VALUE r_mpfr_matrix_str_ary(VALUE self, VALUE format_str)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  char *format = StringValuePtr(format_str), *tmp_str;
  VALUE ret_val[ptr_self->size];
  int i;
  for (i = 0; i < ptr_self->size; i++) {
    mpfr_asprintf(&tmp_str, format, ptr_self->data + i);
    ret_val[i] = rb_str_new2(tmp_str);
    mpfr_free_str(tmp_str);
  }
  return rb_ary_new4(ptr_self->size, ret_val);
}

/* Return two dimensinal array including strings which elements of matrix is converted to by MPFR#to_strf. */
static VALUE r_mpfr_matrix_str_ary2(VALUE self, VALUE format_str)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  char *format = StringValuePtr(format_str), *tmp_str;
  VALUE ary[ptr_self->row];
  int i, j;
  for(i = 0; i < ptr_self->row; i++){
    ary[i] = rb_ary_new();
  }
  for (i = 0; i < ptr_self->size; i += ptr_self->row) {
    for (j = 0; j < ptr_self->row; j++) {
      mpfr_asprintf(&tmp_str, format, ptr_self->data + i + j);
      rb_ary_push(ary[j], rb_str_new2(tmp_str));
      mpfr_free_str(tmp_str);
    }
  }
  return rb_ary_new4(ptr_self->row, ary);
}

/* Retrn two dimensinal array including MPFR elements of matrix. */
static VALUE r_mpfr_matrix_to_array(VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret_val[ptr_self->size];
  int i;
  for (i = 0; i < ptr_self->size; i++) {
    ret_val[i] = r_mpfr_make_new_fr_obj(ptr_self->data + i);
  }
  return rb_ary_new4(ptr_self->size, ret_val);
}

/* Retrn one dimensinal array including MPFR elements of matrix. */
static VALUE r_mpfr_matrix_to_array2(VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ary[ptr_self->row];
  int i, j;
  for(i = 0; i < ptr_self->row; i++){
    ary[i] = rb_ary_new();
  }
  for (i = 0; i < ptr_self->size; i += ptr_self->row) {
    for (j = 0; j < ptr_self->row; j++) {
      rb_ary_push(ary[j], r_mpfr_make_new_fr_obj(ptr_self->data + i + j));
    }
  }
  return rb_ary_new4(ptr_self->row, ary);
}

/* Return _p1_-th row vector. */
static VALUE r_mpfr_matrix_row (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr_self, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  int num = NUM2INT(arg);
  VALUE ret;
  if(num < ptr_self->row){
    r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, 1, ptr_self->column);
    mpfr_matrix_row(ptr_ret, ptr_self, num);
    return ret;
  }else{
    return Qnil;
  }
}

/* Return _p1_-th column vector. */
static VALUE r_mpfr_matrix_column (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr_self, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  int num = NUM2INT(arg);
  VALUE ret;
  if(num < ptr_self->column){
    r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, 1);
    mpfr_matrix_column(ptr_ret, ptr_self, num);
    return ret;
  }else{
    return Qnil;
  }
}

/* Return transposed matrix which is new object. */
static VALUE r_mpfr_matrix_transpose (VALUE self)
{
  MPFRMatrix *ptr_self, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->column, ptr_self->row);
  mpfr_matrix_transpose(ptr_ret, ptr_self);
  return ret;
}

/* Transpose self. This method is destructive method. */
static VALUE r_mpfr_matrix_transpose2 (VALUE self)
{
  MPFRMatrix *ptr_self, tmp;
  r_mpfr_get_matrix_struct(ptr_self, self);
  if (ptr_self->column > 1 && ptr_self->row > 1) {
    mpfr_matrix_init(&tmp, ptr_self->column, ptr_self->row);
    mpfr_matrix_transpose(&tmp, ptr_self);
    mpfr_matrix_set(ptr_self, &tmp);
    mpfr_matrix_clear(&tmp);
  }
  int i;
  i = ptr_self->column;
  ptr_self->column = ptr_self->row;
  ptr_self->row = i;
  return self;
}

/* Multiply all elements by -1. This method is non-destructive. */
static VALUE r_mpfr_matrix_neg (VALUE self)
{
  MPFRMatrix *ptr_self, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  mpfr_matrix_neg(ptr_ret, ptr_self);
  return ret;
}

/* Return _self_ + _other_. */
static VALUE r_mpfr_matrix_add (VALUE self, VALUE other)
{
  MPFRMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_other, other);
  VALUE ret;
  if (ptr_self->column == ptr_other->column && ptr_self->row == ptr_other->row) {
    r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
    mpfr_matrix_add(ptr_ret, ptr_self, ptr_other);
  }else{
    rb_raise(rb_eArgError, "Matrixes must have same size.");
  }
  return ret;
}

/* Return _self_ + _other_ which is MPFR::Matrix. */
static VALUE r_mpfr_matrix_add2 (VALUE self, VALUE other)
{
  MPFRMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_other, other);
  VALUE ret;
  r_mpfr_make_matrix_struct(ret, ptr_ret);
  if (ptr_self->column == ptr_other->column && ptr_self->row == ptr_other->row) {
    mpfr_matrix_init(ptr_ret, ptr_self->row, ptr_self->column);
    mpfr_matrix_add(ptr_ret, ptr_self, ptr_other);
  }else{
    rb_raise(rb_eArgError, "Matrixes must have same size.");
  }
  return ret;
}

/* Return _self_ - _other_. */
static VALUE r_mpfr_matrix_sub (VALUE self, VALUE other)
{
  MPFRMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_other, other);
  VALUE ret;
  r_mpfr_make_matrix_struct(ret, ptr_ret);
  if (ptr_self->column == ptr_other->column && ptr_self->row == ptr_other->row) {
    r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
    mpfr_matrix_sub(ptr_ret, ptr_self, ptr_other);
  }else{
    rb_raise(rb_eArgError, "Matrixes must have same size.");
  }
  return ret;
}

/* Return _self_ - _other_ which is MPFR::Matrix. */
static VALUE r_mpfr_matrix_sub2 (VALUE self, VALUE other)
{
  MPFRMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_other, other);
  VALUE ret;
  r_mpfr_make_matrix_struct(ret, ptr_ret);
  if (ptr_self->column == ptr_other->column && ptr_self->row == ptr_other->row) {
    mpfr_matrix_init(ptr_ret, ptr_self->row, ptr_self->column);
    mpfr_matrix_sub(ptr_ret, ptr_self, ptr_other);
  }else{
    rb_raise(rb_eArgError, "Matrixes must have same size.");
  }
  return ret;
}

/* Regard matrix as vector and return the value of inner product. */
static VALUE r_mpfr_vector_inner_product (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr_self, *ptr_arg;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_arg, arg);
  VALUE ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init(ret, ptr_ret);
  if(ptr_self->size == ptr_arg->size){
    mpfr_matrix_inner_product(ptr_ret, ptr_self, ptr_arg);
  }else{
    rb_raise(rb_eArgError, "Vectors must have same size.");
  }
  return ret;
}

/* Return _self_ * _other_. */
static VALUE r_mpfr_matrix_mul_matrix (VALUE self, VALUE other)
{
  MPFRMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_other, other);
  VALUE ret;
  r_mpfr_make_matrix_struct(ret, ptr_ret);
  if (ptr_self->column == ptr_other->row) {
    if((ptr_other->column == 1) && (ptr_self->row == 1)){
      ret = r_mpfr_vector_inner_product(self, other);
    }else{
      r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_other->column);
      mpfr_matrix_mul(ptr_ret, ptr_self, ptr_other);
    }
  }else{
    rb_raise(rb_eArgError, "Row size of first matrixes must be same as colmun size of second matrixes.");
  }
  return ret;
}

/* Return _self_ * _other_ which is MPFR::Matrix. */
static VALUE r_mpfr_matrix_mul_matrix2 (VALUE self, VALUE other)
{
  MPFRMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_other, other);
  VALUE ret;
  r_mpfr_make_matrix_struct(ret, ptr_ret);
  if (ptr_self->column == ptr_other->row) {
    mpfr_matrix_init(ptr_ret, ptr_self->row, ptr_other->column);
    mpfr_matrix_mul(ptr_ret, ptr_self, ptr_other);
  }else{
    rb_raise(rb_eArgError, "Row size of first matrixes must be same as colmun size of second matrixes.");
  }
  return ret;
}

/* Return _scalar_ * _self_, where _scalar_ is scalar and _self_ is matrix. */
static VALUE r_mpfr_matrix_mul_scalar (VALUE self, VALUE scalar)
{
  MPFRMatrix *ptr_self, *ptr_ret;
  MPFR *ptr_scalar;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(RTEST(rb_funcall(r_mpfr_class, eqq, 1, scalar))){
    r_mpfr_get_struct(ptr_scalar, scalar);
    mpfr_matrix_mul_scalar(ptr_ret, ptr_self, ptr_scalar);
  }else{
    r_mpfr_temp_alloc_init(ptr_scalar);
    r_mpfr_set_robj(ptr_scalar, scalar, GMP_RNDN);
    mpfr_matrix_mul_scalar(ptr_ret, ptr_self, ptr_scalar);
    r_mpfr_temp_free(ptr_scalar);
  }
  return ret;
}

/* Return _scalar_ * _self_, where _scalar_ is scalar and _self_ is matrix. */
static VALUE r_mpfr_matrix_div_scalar (VALUE self, VALUE scalar)
{
  MPFRMatrix *ptr_self, *ptr_ret;
  MPFR *ptr_scalar;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(RTEST(rb_funcall(r_mpfr_class, eqq, 1, scalar))){
    r_mpfr_get_struct(ptr_scalar, scalar);
    mpfr_matrix_div_scalar(ptr_ret, ptr_self, ptr_scalar);
  }else{
    r_mpfr_temp_alloc_init(ptr_scalar);
    r_mpfr_set_robj(ptr_scalar, scalar, GMP_RNDN);
    mpfr_matrix_div_scalar(ptr_ret, ptr_self, ptr_scalar);
    r_mpfr_temp_free(ptr_scalar);
  }
  return ret;
}

/* Regard matrix as vector and return length of the vector. */
static VALUE r_mpfr_matrix_vector_norm (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init(ret, ptr_ret);
  mpfr_matrix_vector_norm(ptr_ret, ptr_self);
  return ret;
}

/* Return maximum value of absolute value of element. */
static VALUE r_mpfr_matrix_max_norm (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init(ret, ptr_ret);
  mpfr_matrix_max_norm(ptr_ret, ptr_self);
  return ret;
}

/* Return row size of matrix. */
static VALUE r_mpfr_square_matrix_dim (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  return INT2FIX(ptr_self->row);
}

/* Perform LU decomposition and return [matrix_l, matrix_r, indx]. */
static VALUE r_mpfr_square_matrix_lu_decomp (VALUE self)
{
  MPFRMatrix *ptr_self, *ptr_ret_l, *ptr_ret_u;
  VALUE ret_l, ret_u;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_matrix_suitable_matrix_init (&ret_l, &ptr_ret_l, ptr_self->row, ptr_self->column);
  r_mpfr_matrix_suitable_matrix_init (&ret_u, &ptr_ret_u, ptr_self->row, ptr_self->column);
  VALUE ret_indx[ptr_self->row];
  int indx[ptr_self->row];
  if(mpfr_square_matrix_lu_decomp (ptr_ret_u, indx, ptr_self) >= 0){
    int i, j;
    for(i = 1; i < ptr_ret_u->row; i++){
      for(j = 0; j < i; j++){
	mpfr_set(mpfr_matrix_get_element(ptr_ret_l, i, j), mpfr_matrix_get_element(ptr_ret_u, i, j), GMP_RNDN);
	mpfr_set_si(mpfr_matrix_get_element(ptr_ret_u, i, j), 0, GMP_RNDN);
      }
    }
    for(i = 0; i < ptr_ret_u->row; i++){
      mpfr_set_si(mpfr_matrix_get_element(ptr_ret_l, i, i), 1, GMP_RNDN);
    }
    for(i = 0; i < ptr_ret_u->row; i++){
      for(j = i + 1; j < ptr_ret_u->column; j++){
	mpfr_set_si(mpfr_matrix_get_element(ptr_ret_l, i, j), 0, GMP_RNDN);
      }
    }
    for(i = 0; i < ptr_ret_u->row; i++){
      ret_indx[i] = INT2NUM(indx[i]);
    }
    return rb_ary_new3(3, ret_l, ret_u, rb_ary_new4(ptr_ret_u->row, ret_indx));
  }else{
    return Qnil;
  }
}

/* Return determinant of matrix. */
static VALUE r_mpfr_square_matrix_determinant (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  MPFR *ptr_ret;
  VALUE ret;
  r_mpfr_make_struct_init(ret, ptr_ret);
  mpfr_square_matrix_determinant(ptr_ret, ptr_self);
  return ret;
}

/* Perform QR decomposition and return [matrix_q, matrix_r]. */
static VALUE r_mpfr_square_matrix_qr_decomp (VALUE self)
{
  MPFRMatrix *ptr_self, *ptr_q, *ptr_r;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE q, r;
  r_mpfr_matrix_suitable_matrix_init (&q, &ptr_q, ptr_self->row, ptr_self->column);
  r_mpfr_matrix_suitable_matrix_init (&r, &ptr_r, ptr_self->column, ptr_self->column);
  mpfr_square_matrix_qr_decomp(ptr_q, ptr_r, ptr_self);
  return rb_ary_new3(2, q, r);
}

/* Return identity matrix. */
static VALUE r_mpfr_square_matrix_identity (VALUE self, VALUE size)
{
  VALUE ret;
  MPFRMatrix *ptr_ret;
  int s = NUM2INT(size);
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, s, s);
  mpfr_square_matrix_identity(ptr_ret);
  return ret;
}

/* Return size of data array which equals to column * row. */
static VALUE r_mpfr_vector_dim (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  return INT2FIX(ptr_self->size);
}

/* Regard matrix as vector and return length of the vector. */
static VALUE r_mpfr_vector_abs (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init(ret, ptr_ret);
  mpfr_matrix_vector_norm(ptr_ret, ptr_self);
  return ret;
}

/* Return element at _p1_.*/
static VALUE r_mpfr_vector_element (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  int i = NUM2INT(arg);
  if (i < ptr_self->size) {
    VALUE ret;
    MPFR *ptr_ret;
    r_mpfr_make_struct_init(ret, ptr_ret);
    mpfr_set(ptr_ret, ptr_self->data + i, GMP_RNDN);
    return ret;
  }else{
    return Qnil;
  }
}

/* Set _p2_ to _p1_ th element. */
static VALUE r_mpfr_vector_set_element (VALUE self, VALUE arg, VALUE robj)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  int i = NUM2INT(arg);
  if (i < ptr_self->size) {
    r_mpfr_set_robj(ptr_self->data + i, robj, GMP_RNDN);
  }
  return Qnil;
}

/* Evaluate block with each element of vector. */
static VALUE r_mpfr_vector_each_element (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret = Qnil, tmp;
  MPFR *tmp_ptr;
  r_mpfr_make_struct_init(tmp, tmp_ptr);
  int i;
  for (i = 0; i < ptr_self->size; i++) {
    mpfr_set(tmp_ptr, ptr_self->data + i, GMP_RNDN);
    ret = rb_yield(tmp);
  }
  return ret;
}

/* Evaluate block with each element and its index. */
static VALUE r_mpfr_vector_each_element_with_index (VALUE self)
{
  MPFRMatrix *ptr_self;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret = Qnil, tmp;
  MPFR *tmp_ptr;
  r_mpfr_make_struct_init(tmp, tmp_ptr);
  int i;
  for (i = 0; i < ptr_self->size; i++) {
    mpfr_set(tmp_ptr, ptr_self->data + i, GMP_RNDN);
    ret = rb_yield(rb_ary_new3(2, tmp, INT2NUM(i)));
  }
  return ret;
}

/* Return distance from _p1_, i.e. sqrt((x1 - x2)^2 + (y1 - y2)^2). */
static VALUE r_mpfr_vector_distance_from (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr_self, *ptr_arg;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_arg, arg);
  VALUE ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init(ret, ptr_ret);
  if(ptr_self->size == ptr_arg->size){
    mpfr_matrix_vector_distance(ptr_ret, ptr_self, ptr_arg);
  }else{
    rb_raise(rb_eArgError, "Vectors must have same size.");
  }
  return ret;
}

/* Return midpoint between _self_ and _p1_. */
static VALUE r_mpfr_vector_midpoint (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr_self, *ptr_arg, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_arg, arg);
  VALUE ret;
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  mpfr_vector_midpoint(ptr_ret, ptr_self, ptr_arg);
  return ret;
}

/* Return dividing point of _self_ and _p1_ by _p2_. */
static VALUE r_mpfr_vector_dividing_point (VALUE self, VALUE arg, VALUE div)
{
  MPFRMatrix *ptr_self, *ptr_arg, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  r_mpfr_get_matrix_struct(ptr_arg, arg);
  MPFR *ptr_div;
  r_mpfr_get_struct(ptr_div, div);
  VALUE ret;
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  mpfr_vector_dividing_point(ptr_ret, ptr_self, ptr_arg, ptr_div);
  return ret;
}

/* Return normalized vector of _self_. */
static VALUE r_mpfr_vector_normalize (VALUE self)
{
  MPFRMatrix *ptr_self, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(mpfr_vector_normalize(ptr_ret, ptr_self) == 0){
    return ret;
  }else{
    return Qnil;
  }
}

/* Return normalized vector of _self_. This method is destructive method. */
static VALUE r_mpfr_vector_normalize2 (VALUE self)
{
  MPFRMatrix *ptr_self, tmp;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret = self;
  mpfr_matrix_init(&tmp, ptr_self->column, ptr_self->row);
  if(mpfr_vector_normalize(&tmp, ptr_self) == 0){
    mpfr_matrix_set(ptr_self, &tmp);
  }else{
    ret = Qnil;
  }
  mpfr_matrix_clear(&tmp);
  return ret;
}

/* Return new vector whose length is _p1_.  */
static VALUE r_mpfr_vector_set_length (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr_self, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret, returned_value = Qnil;
  MPFR *ptr_l;
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(RTEST(rb_funcall(r_mpfr_class, eqq, 1, arg))){
    r_mpfr_get_struct(ptr_l, arg);
    if(mpfr_vector_set_length(ptr_ret, ptr_self, ptr_l) == 0){
      returned_value = ret;
    }
  }else{
    r_mpfr_temp_alloc_init(ptr_l);
    r_mpfr_set_robj(ptr_l, arg, GMP_RNDN);
    if(mpfr_vector_set_length(ptr_ret, ptr_self, ptr_l) == 0){
      returned_value = ret;
    }
    r_mpfr_temp_free(ptr_l);
  }
  return returned_value;
}

/* Return new vector whose length is _p1_. This method is destructive. */
static VALUE r_mpfr_vector_set_length2 (VALUE self, VALUE arg)
{
  MPFRMatrix *ptr_self, *ptr_ret;
  r_mpfr_get_matrix_struct(ptr_self, self);
  VALUE ret, returned_value = Qnil;
  MPFR *ptr_l;
  r_mpfr_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(RTEST(rb_funcall(r_mpfr_class, eqq, 1, arg))){
    r_mpfr_get_struct(ptr_l, arg);
    if(mpfr_vector_set_length(ptr_ret, ptr_self, ptr_l) == 0){
      mpfr_matrix_set(ptr_self, ptr_ret);
      returned_value = self;
    }
  }else{
    r_mpfr_temp_alloc_init(ptr_l);
    r_mpfr_set_robj(ptr_l, arg, GMP_RNDN);
    if(mpfr_vector_set_length(ptr_ret, ptr_self, ptr_l) == 0){
      mpfr_matrix_set(ptr_self, ptr_ret);
      returned_value = self;
    }
    r_mpfr_temp_free(ptr_l);
  }
  return returned_value;
}

void Init_matrix()
{
  
  VALUE tmp_r_mpfr_class = rb_define_class("MPFR", rb_cNumeric);

  /*
    MPFR::Matrix is class in which instances means matrix and
    have MPFR instances as elements.
    MPFR::SquareMatrix, MPFR::ColumnVector, and MPFR::RowVector are subclass of MPFR::Matrix.
  */
  r_mpfr_matrix = rb_define_class_under(tmp_r_mpfr_class, "Matrix", rb_cObject);
  r_mpfr_square_matrix = rb_define_class_under(tmp_r_mpfr_class, "SquareMatrix", r_mpfr_matrix);
  r_mpfr_col_vector = rb_define_class_under(tmp_r_mpfr_class, "ColumnVector", r_mpfr_matrix);
  r_mpfr_row_vector = rb_define_class_under(tmp_r_mpfr_class, "RowVector", r_mpfr_matrix);

  rb_define_singleton_method(tmp_r_mpfr_class, "Matrix", r_mpfr_matrix_global_new, -1);
  rb_define_alloc_func(r_mpfr_matrix, r_mpfr_matrix_alloc);
  rb_define_private_method(r_mpfr_matrix, "initialize", r_mpfr_matrix_initialize, -1);
  rb_define_private_method(r_mpfr_matrix, "initialize_copy", r_mpfr_matrix_initialize_copy, 1);

  rb_define_singleton_method(tmp_r_mpfr_class, "SquareMatrix", r_mpfr_square_matrix_global_new, 1);
  rb_define_alloc_func(r_mpfr_square_matrix, r_mpfr_square_matrix_alloc);
  rb_define_private_method(r_mpfr_square_matrix, "initialize", r_mpfr_square_matrix_initialize, 1);

  rb_define_singleton_method(tmp_r_mpfr_class, "ColumnVector", r_mpfr_col_vector_global_new, 1);
  rb_define_alloc_func(r_mpfr_col_vector, r_mpfr_col_vector_alloc);
  rb_define_private_method(r_mpfr_col_vector, "initialize", r_mpfr_col_vector_initialize, 1);

  rb_define_singleton_method(tmp_r_mpfr_class, "RowVector", r_mpfr_row_vector_global_new, 1);
  rb_define_alloc_func(r_mpfr_row_vector, r_mpfr_row_vector_alloc);
  rb_define_private_method(r_mpfr_row_vector, "initialize", r_mpfr_row_vector_initialize, 1);


  rb_define_method(r_mpfr_matrix, "size", r_mpfr_matrix_size, 0);
  rb_define_method(r_mpfr_matrix, "column_size", r_mpfr_matrix_column_size, 0);
  rb_define_method(r_mpfr_matrix, "row_size", r_mpfr_matrix_row_size, 0);
  
  rb_define_method(r_mpfr_matrix, "set_element", r_mpfr_matrix_set_element, 3);
  rb_define_method(r_mpfr_matrix, "at", r_mpfr_matrix_at, 1);
  rb_define_method(r_mpfr_matrix, "element", r_mpfr_matrix_element, 2);
  rb_define_method(r_mpfr_matrix, "each_element", r_mpfr_matrix_each_element, 0);
  rb_define_method(r_mpfr_matrix, "each_element_with_index", r_mpfr_matrix_each_element_with_index, 0);

  rb_define_alias(r_mpfr_matrix, "[]=", "set_element");
  rb_define_alias(r_mpfr_matrix, "[]", "element");
  rb_define_alias(r_mpfr_matrix, "each", "each_element");

  rb_include_module(r_mpfr_matrix, rb_mEnumerable);


  rb_define_method(r_mpfr_matrix, "str_ary", r_mpfr_matrix_str_ary, 1);
  rb_define_method(r_mpfr_matrix, "str_ary2", r_mpfr_matrix_str_ary2, 1);


  rb_define_method(r_mpfr_matrix, "to_a", r_mpfr_matrix_to_array2, 0);
  rb_define_method(r_mpfr_matrix, "to_a2", r_mpfr_matrix_to_array, 0);

  rb_define_method(r_mpfr_matrix, "row", r_mpfr_matrix_row, 1);
  rb_define_method(r_mpfr_matrix, "column", r_mpfr_matrix_column, 1);
  rb_define_method(r_mpfr_matrix, "transpose", r_mpfr_matrix_transpose, 0);
  rb_define_method(r_mpfr_matrix, "transpose!", r_mpfr_matrix_transpose2, 0);
  rb_define_method(r_mpfr_matrix, "neg", r_mpfr_matrix_neg, 0);


  rb_define_method(r_mpfr_matrix, "==", r_mpfr_matrix_equal_p, 1);
  rb_define_method(r_mpfr_matrix, "add", r_mpfr_matrix_add, 1);
  rb_define_method(r_mpfr_matrix, "add2", r_mpfr_matrix_add2, 1);
  rb_define_method(r_mpfr_matrix, "sub", r_mpfr_matrix_sub, 1);
  rb_define_method(r_mpfr_matrix, "sub2", r_mpfr_matrix_sub2, 1);
  rb_define_method(r_mpfr_matrix, "mul_matrix", r_mpfr_matrix_mul_matrix, 1);
  rb_define_method(r_mpfr_matrix, "mul_matrix2", r_mpfr_matrix_mul_matrix2, 1);
  rb_define_method(r_mpfr_matrix, "mul_scalar", r_mpfr_matrix_mul_scalar, 1);
  rb_define_method(r_mpfr_matrix, "div_scalar", r_mpfr_matrix_div_scalar, 1);

  rb_define_alias(r_mpfr_matrix, "+", "add");
  rb_define_alias(r_mpfr_matrix, "-", "sub");
  rb_define_alias(r_mpfr_matrix, "*", "mul_matrix");

  rb_define_method(r_mpfr_matrix, "vector_norm", r_mpfr_matrix_vector_norm, 0);
  rb_define_method(r_mpfr_matrix, "max_norm", r_mpfr_matrix_max_norm, 0);


  rb_define_method(r_mpfr_square_matrix, "dim", r_mpfr_square_matrix_dim, 0);
  rb_define_method(r_mpfr_square_matrix, "lu_decomp", r_mpfr_square_matrix_lu_decomp, 0);
  rb_define_method(r_mpfr_square_matrix, "determinant", r_mpfr_square_matrix_determinant, 0);
  rb_define_method(r_mpfr_square_matrix, "qr_decomp", r_mpfr_square_matrix_qr_decomp, 0);

  rb_define_singleton_method(r_mpfr_square_matrix, "identity", r_mpfr_square_matrix_identity, 1);

  /*
    This module add the functionality as vector.
    MPFR::ColumnVector and MPFR::RowVector include MPFR::Vector module.
  */
  r_mpfr_vector_module = rb_define_module_under(tmp_r_mpfr_class, "Vector");

  rb_define_method(r_mpfr_vector_module, "[]=", r_mpfr_vector_set_element, 2);
  rb_define_method(r_mpfr_vector_module, "[]", r_mpfr_vector_element, 1);
  rb_define_method(r_mpfr_vector_module, "each_element", r_mpfr_vector_each_element, 0);
  rb_define_method(r_mpfr_vector_module, "each_element_with_index", r_mpfr_vector_each_element_with_index, 0);

  rb_define_alias(r_mpfr_vector_module, "each", "each_element");
  rb_define_alias(r_mpfr_vector_module, "each_with_index", "each_element_with_index");

  rb_define_method(r_mpfr_vector_module, "dim", r_mpfr_vector_dim, 0);
  rb_define_method(r_mpfr_vector_module, "normalize", r_mpfr_vector_normalize, 0);
  rb_define_method(r_mpfr_vector_module, "normalize!", r_mpfr_vector_normalize2, 0);
  rb_define_method(r_mpfr_vector_module, "set_length", r_mpfr_vector_set_length, 1);
  rb_define_method(r_mpfr_vector_module, "set_length!", r_mpfr_vector_set_length2, 1);


  rb_define_method(r_mpfr_vector_module, "abs", r_mpfr_vector_abs, 0);
  rb_define_method(r_mpfr_vector_module, "inner_product", r_mpfr_vector_inner_product, 1);
  rb_define_method(r_mpfr_vector_module, "distance_from", r_mpfr_vector_distance_from, 1);
  rb_define_method(r_mpfr_vector_module, "midpoint", r_mpfr_vector_midpoint, 1);
  rb_define_method(r_mpfr_vector_module, "dividing_point", r_mpfr_vector_dividing_point, 2);

  rb_include_module(r_mpfr_col_vector, r_mpfr_vector_module);
  rb_include_module(r_mpfr_row_vector, r_mpfr_vector_module);


  eqq = rb_intern("===");


}

