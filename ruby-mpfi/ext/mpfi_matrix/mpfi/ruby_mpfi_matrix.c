#include "ruby_mpfi_matrix.h"

static ID eqq;
static VALUE __mpfr_matrix_class__, __mpfi_matrix_class__;

void r_mpfi_matrix_free(void *ptr){
  mpfi_matrix_clear(ptr);
  free(ptr);
}

void r_mpfi_matrix_suitable_matrix_init (VALUE *other, MPFIMatrix **ptr_other, int row, int column){
  if(column == 1){
    r_mpfi_make_col_vector_struct(*other, *ptr_other);
    mpfi_col_vector_init(*ptr_other, row);
  }else if (row == 1){
    r_mpfi_make_row_vector_struct(*other, *ptr_other);
    mpfi_row_vector_init(*ptr_other, column);
  }else{
    if (row == column){
      r_mpfi_make_square_matrix_struct(*other, *ptr_other);
    }else{  
      r_mpfi_make_matrix_struct(*other, *ptr_other);
    }
    mpfi_matrix_init(*ptr_other, row, column);
  }
}

/* VALUE r_mpfi_matrix_copied_robj(VALUE old){ */
/*   MPFIMatrix *ptr_old, *ptr_ret; */
/*   r_mpfi_get_matrix_struct(ptr_old, old); */
/*   VALUE ret; */
/*   r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_old->row, ptr_old->column); */
/*   mpfi_matrix_set(ptr_ret, ptr_old); */
/*   return ret; */
/* } */

VALUE r_mpfi_matrix_robj(MPFIMatrix *x){
  VALUE ret;
  MPFIMatrix *ptr_ret;
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, x->row, x->column);
  mpfi_matrix_set(ptr_ret, x);
  return ret;
}

/* Allocation function for MPFI::Matrix. */
static VALUE r_mpfi_matrix_alloc (VALUE self){
  MPFIMatrix *ptr;
  r_mpfi_make_matrix_struct(self, ptr);
  return self;
}

/* static void r_mpfi_matrix_init_set_from_rmatrix(MPFIMatrix *x, VALUE src){ */
/*   MPFIMatrix *ptr_src; */
/*   r_mpfi_get_matrix_struct(ptr_src, src); */
/*   mpfi_matrix_init(x, ptr_src->row, ptr_src->column); */
/*   mpfi_matrix_set(x, ptr_src); */
/* } */

static void r_mpfi_matrix_set_initial_value(MPFIMatrix *ptr, int argc, VALUE *argv)
{
  int row, column, i, j;
  VALUE row_ary;
  switch (argc) {
  case 1:
    row = RARRAY_LEN(argv[0]);
    column = RARRAY_LEN(rb_ary_entry(argv[0], 0));
    mpfi_matrix_init(ptr, row, column);
    for (i = 0; i < row; i++) {
      row_ary = rb_ary_entry(argv[0], i);
      if (column != RARRAY_LEN(row_ary)) {
	rb_raise(rb_eArgError, "MPFI::Matrix.new needs Array which has arrays of same sizes.");
      }
      for (j = 0; j < column; j++) {
	r_mpfi_set_robj(mpfi_matrix_get_element(ptr, i, j), rb_ary_entry(row_ary, j));
      }
    }
    break;
  case 2:
    mpfi_matrix_init(ptr, NUM2INT(argv[0]), NUM2INT(argv[1]));
    mpfi_matrix_set_zeros(ptr);
    break;
  default:
    rb_raise(rb_eArgError, "MPFI::Matrix.new needs one or two arguments.");
    break;
  }
}

/* Return new MPFI::Matrix instance. The same arguments as MPFI::Matrix.new is acceptable. */
static VALUE r_mpfi_matrix_global_new(int argc, VALUE *argv, VALUE self)
{
  MPFIMatrix *ptr;
  VALUE val;
  r_mpfi_make_matrix_struct(val, ptr);
  r_mpfi_matrix_set_initial_value(ptr, argc, argv);
  return val;
}

/* Initialization function for MPFI::Matrix. */
static VALUE r_mpfi_matrix_initialize (int argc, VALUE *argv, VALUE self){
  MPFIMatrix *ptr;
  r_mpfi_get_matrix_struct(ptr, self);
  r_mpfi_matrix_set_initial_value(ptr, argc, argv);
  return Qtrue;
}

/* Allocation function for MPFI::SquareMatrix. */
static VALUE r_mpfi_square_matrix_alloc (VALUE self){
  MPFIMatrix *ptr;
  r_mpfi_make_square_matrix_struct(self, ptr);
  return self;
}

static void r_mpfi_square_matrix_set_initial_value(MPFIMatrix *ptr, VALUE arg)
{
  int row, column, i, j;
  VALUE row_ary;
  switch(TYPE(arg)){
  case T_ARRAY:
    row = RARRAY_LEN(arg);
    column = RARRAY_LEN(rb_ary_entry(arg, 0));
    if(row == column){
      mpfi_matrix_init(ptr, row, column);
      for (i = 0; i < row; i++) {
	row_ary = rb_ary_entry(arg, i);
	if (column != RARRAY_LEN(row_ary)) {
	  rb_raise(rb_eArgError, "MPFI::Matrix.new needs Array which has arrays of same sizes.");
	}
	for (j = 0; j < column; j++) {
	  r_mpfi_set_robj(mpfi_matrix_get_element(ptr, i, j), rb_ary_entry(row_ary, j));
	}
      }
    }else{
      rb_raise(rb_eArgError, "MPFI::SquareMatrix.new needs two dimensinal Array which has the same sizes of row and column.");
    }
    break;
  case T_FIXNUM:
    row = NUM2INT(arg);
    mpfi_matrix_init(ptr, row, row);
    mpfi_matrix_set_zeros(ptr);
    break;
  default:
    rb_raise(rb_eArgError, "MPFI::SquareMatrix.new needs Array or Fixnum.");
    break;
  }  
}

/* Return new MPFI::SquareMatrix instance. The same arguments as MPFI::SquareMatrix.new is acceptable. */
static VALUE r_mpfi_square_matrix_global_new(int argc, VALUE arg)
{
  MPFIMatrix *ptr;
  VALUE val;
  r_mpfi_make_matrix_struct(val, ptr);
  r_mpfi_square_matrix_set_initial_value(ptr, arg);
  return val;
}

/* Initialization function for MPFI::SquareMatrix. */
static VALUE r_mpfi_square_matrix_initialize (VALUE self, VALUE arg){
  MPFIMatrix *ptr;
  r_mpfi_get_matrix_struct(ptr, self);
  r_mpfi_square_matrix_set_initial_value(ptr, arg);
  return Qtrue;
}

/* initialize_copy */
static VALUE r_mpfi_matrix_initialize_copy (VALUE self, VALUE other){
  MPFIMatrix *ptr_self, *ptr_other;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_other, other);
  mpfi_matrix_init(ptr_self, ptr_other->row, ptr_other->column);
  mpfi_matrix_set(ptr_self, ptr_other);
  return Qtrue;
}

/* Allocation function for MPFI::ColumnVector. */
static VALUE r_mpfi_col_vector_alloc (VALUE self){
  MPFIMatrix *ptr;
  r_mpfi_make_col_vector_struct(self, ptr);
  return self;
}

static void r_mpfi_col_vector_set_initial_value(MPFIMatrix *ptr, VALUE arg)
{
  int row, i;
  switch(TYPE(arg)){
  case T_ARRAY:
    row = RARRAY_LEN(arg);
    mpfi_col_vector_init(ptr, row);
    for (i = 0; i < row; i++) {
      r_mpfi_set_robj(mpfi_matrix_get_ptr(ptr, i), rb_ary_entry(arg, i));
    }
    break;
  case T_FIXNUM:
    row = NUM2INT(arg);
    mpfi_col_vector_init(ptr, row);
    for (i = 0; i < row; i++) {
      mpfi_set_si(mpfi_matrix_get_ptr(ptr, i), 0);
    }
    break;
  default:
    rb_raise(rb_eArgError, "MPFI::ColumnVector.new needs Array or Fixnum.");
    break;
  }  
}

/* Return new MPFI::ColumnVector instance. The same arguments as MPFI::ColumnVector.new is acceptable. */
static VALUE r_mpfi_col_vector_global_new(int argc, VALUE arg)
{
  MPFIMatrix *ptr;
  VALUE val;
  r_mpfi_make_matrix_struct(val, ptr);
  r_mpfi_col_vector_set_initial_value(ptr, arg);
  return val;
}

/* Initialization function for MPFI::ColumnVector. */
static VALUE r_mpfi_col_vector_initialize (VALUE self, VALUE arg){
  MPFIMatrix *ptr;
  r_mpfi_get_matrix_struct(ptr, self);
  r_mpfi_col_vector_set_initial_value(ptr, arg);
  return Qtrue;
}

/* Allocation function for MPFI::RowVector. */
static VALUE r_mpfi_row_vector_alloc (VALUE self){
  MPFIMatrix *ptr;
  r_mpfi_make_row_vector_struct(self, ptr);
  return self;
}

static void r_mpfi_row_vector_set_initial_value(MPFIMatrix *ptr, VALUE arg)
{
  int column, i;
  switch(TYPE(arg)){
  case T_ARRAY:
    column = RARRAY_LEN(arg);
    mpfi_row_vector_init(ptr, column);
    for (i = 0; i < column; i++) {
      r_mpfi_set_robj(mpfi_matrix_get_ptr(ptr, i), rb_ary_entry(arg, i));
    }
    break;
  case T_FIXNUM:
    column = NUM2INT(arg);
    mpfi_row_vector_init(ptr, column);
    for (i = 0; i < column; i++) {
      mpfi_set_si(mpfi_matrix_get_ptr(ptr, i), 0);
    }
    break;
  default:
    rb_raise(rb_eArgError, "MPFI::RowVector.new needs Array or Fixnum.");
    break;
  }
}

/* Return new MPFI::RowVector instance. The same arguments as MPFI::RowVector.new is acceptable. */
static VALUE r_mpfi_row_vector_global_new(int argc, VALUE arg)
{
  MPFIMatrix *ptr;
  VALUE val;
  r_mpfi_make_matrix_struct(val, ptr);
  r_mpfi_row_vector_set_initial_value(ptr, arg);
  return val;
}

/* Initialization function for MPFI::RowVector. */
static VALUE r_mpfi_row_vector_initialize (VALUE self, VALUE arg){
  MPFIMatrix *ptr;
  r_mpfi_get_matrix_struct(ptr, self);
  r_mpfi_row_vector_set_initial_value(ptr, arg);
  return Qtrue;
}

/* Return size of data array which equals to column * row. */
static VALUE r_mpfi_matrix_size (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  return INT2FIX(ptr_self->size);
}

/* Return column size of matrix. */
static VALUE r_mpfi_matrix_column_size (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  return INT2FIX(ptr_self->column);
}

/* Return row size of matrix. */
static VALUE r_mpfi_matrix_row_size (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  return INT2FIX(ptr_self->row);
}

/* Return element at _arg_.*/
static VALUE r_mpfi_matrix_at (VALUE self, VALUE arg){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  int i = NUM2INT(arg);
  if (i < ptr_self->size) {
    VALUE ret;
    MPFI *ptr_ret;
    r_mpfi_make_struct_init(ret, ptr_ret);
    mpfi_set(ptr_ret, ptr_self->data + i);
    return ret;
  }else{
    return Qnil;
  }
}

/* Return element with _row_ and _column_.*/
static VALUE r_mpfi_matrix_element (VALUE self, VALUE row, VALUE column){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  int i = NUM2INT(row), j = NUM2INT(column);
  if (i < ptr_self->row && j < ptr_self->column) {
    VALUE ret;
    MPFI*ptr_ret;
    r_mpfi_make_struct_init(ret, ptr_ret);
    mpfi_set(ptr_ret, ptr_self->data + i + j * ptr_self->row);
    return ret;
  }else{
    return Qnil;
  }
}

/* Set _robj_ to element of which row is _row_ and column is _column_. */
static VALUE r_mpfi_matrix_set_element (VALUE self, VALUE row, VALUE column, VALUE robj){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  int i = NUM2INT(row), j = NUM2INT(column);
  if (i < ptr_self->row && j < ptr_self->column) {
    r_mpfi_set_robj(ptr_self->data + i + j * ptr_self->row, robj);
  }
  return Qnil;
}

/* Evaluate block with each element of matrix. */
static VALUE r_mpfi_matrix_each_element (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret = Qnil;
  int i, j;
  for (i = 0; i < ptr_self->row; i++) {
    for (j = 0; j < ptr_self->column; j++) {
      volatile VALUE arg = r_mpfi_make_new_fi_obj(ptr_self->data + i + j * ptr_self->row);
      ret = rb_yield(arg);
    }
  }
  return ret;
}

/* Evaluate block with each element and its index. */
static VALUE r_mpfi_matrix_each_element_with_index (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret = Qnil, tmp_i;
  int i, j;
  for (i = 0; i < ptr_self->row; i++) {
    tmp_i = INT2NUM(i);
    for (j = 0; j < ptr_self->column; j++) {
      volatile VALUE arg = r_mpfi_make_new_fi_obj(ptr_self->data + i + j * ptr_self->row);
      ret = rb_yield(rb_ary_new3(3, arg, tmp_i, INT2NUM(j)));
    }
  }
  return ret;
}

/* Return one dimensinal array which has strings converted elements to. */
static VALUE r_mpfi_matrix_str_ary_for_inspect(VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  char *tmp_str;
  VALUE ret_val[ptr_self->size];
  int i;
  for (i = 0; i < ptr_self->size; i++) {
    mpfr_asprintf(&tmp_str, "%.Re %.Re",
		  r_mpfi_left_ptr((ptr_self->data + i)), r_mpfi_right_ptr(ptr_self->data + i));
    ret_val[i] = rb_str_new2(tmp_str);
    mpfr_free_str(tmp_str);
  }
  return rb_ary_new4(ptr_self->size, ret_val);
}

/* Return two dimensinal array which has strings converted elements to. */
static VALUE r_mpfi_matrix_str_ary_for_inspect2(VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  char *tmp_str;
  VALUE ary[ptr_self->row];
  int i, j;
  for(i = 0; i < ptr_self->row; i++){
    ary[i] = rb_ary_new();
  }
  for (i = 0; i < ptr_self->size; i += ptr_self->row) {
    for (j = 0; j < ptr_self->row; j++) {
      mpfr_asprintf(&tmp_str, "%.Re %.Re",
		    r_mpfi_left_ptr((ptr_self->data + i + j)), r_mpfi_right_ptr(ptr_self->data + i + j));
      rb_ary_push(ary[j], rb_str_new2(tmp_str));
      mpfr_free_str(tmp_str);
    }
  }
  return rb_ary_new4(ptr_self->row, ary);
}

static VALUE r_mpfi_matrix_to_str_ary(VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  char *tmp_str1, *tmp_str2;
  VALUE ary[ptr_self->row];
  int i, j;
  for(i = 0; i < ptr_self->row; i++){
    ary[i] = rb_ary_new();
  }
  for (i = 0; i < ptr_self->size; i += ptr_self->row) {
    for (j = 0; j < ptr_self->row; j++) {
      mpfr_asprintf(&tmp_str1, "%.Re", r_mpfi_left_ptr((ptr_self->data + i + j)));
      mpfr_asprintf(&tmp_str2, "%.Re", r_mpfi_right_ptr(ptr_self->data + i + j));
      rb_ary_push(ary[j], rb_ary_new3(2, rb_str_new2(tmp_str1), rb_str_new2(tmp_str2)));
      mpfr_free_str(tmp_str1);
      mpfr_free_str(tmp_str2);
    }
  }
  return rb_ary_new4(ptr_self->row, ary);
}

static VALUE r_mpfi_matrix_to_strf_ary(VALUE self, VALUE format_str){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  char *tmp_str1, *tmp_str2;
  char *format = StringValuePtr(format_str);
  VALUE ary[ptr_self->row];
  int i, j;
  for(i = 0; i < ptr_self->row; i++){
    ary[i] = rb_ary_new();
  }
  for (i = 0; i < ptr_self->size; i += ptr_self->row) {
    for (j = 0; j < ptr_self->row; j++) {
      mpfr_asprintf(&tmp_str1, format, r_mpfi_left_ptr((ptr_self->data + i + j)));
      mpfr_asprintf(&tmp_str2, format, r_mpfi_right_ptr(ptr_self->data + i + j));
      rb_ary_push(ary[j], rb_ary_new3(2, rb_str_new2(tmp_str1), rb_str_new2(tmp_str2)));
      mpfr_free_str(tmp_str1);
      mpfr_free_str(tmp_str2);
    }
  }
  return rb_ary_new4(ptr_self->row, ary);
}

static VALUE r_mpfi_matrix_to_array(VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret_val[ptr_self->size];
  int i;
  for (i = 0; i < ptr_self->size; i++) {
    ret_val[i] = r_mpfi_make_new_fi_obj(ptr_self->data + i);
  }
  return rb_ary_new4(ptr_self->size, ret_val);
}

static VALUE r_mpfi_matrix_to_array2(VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ary[ptr_self->row];
  int i, j;
  for(i = 0; i < ptr_self->row; i++){
    ary[i] = rb_ary_new();
  }
  for (i = 0; i < ptr_self->size; i += ptr_self->row) {
    for (j = 0; j < ptr_self->row; j++) {
      rb_ary_push(ary[j], r_mpfi_make_new_fi_obj(ptr_self->data + i + j));
    }
  }
  return rb_ary_new4(ptr_self->row, ary);
}

static VALUE r_mpfi_matrix_row (VALUE self, VALUE arg) {
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  int num = NUM2INT(arg);
  VALUE ret;
  if(num < ptr_self->row){
    r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, 1, ptr_self->column);
    mpfi_matrix_row(ptr_ret, ptr_self, num);
    return ret;
  }else{
    return Qnil;
  }
}

static VALUE r_mpfi_matrix_column (VALUE self, VALUE arg) {
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  int num = NUM2INT(arg);
  VALUE ret;
  if(num < ptr_self->column){
    r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, 1);
    mpfi_matrix_column(ptr_ret, ptr_self, num);
    return ret;
  }else{
    return Qnil;
  }
}

/* Return transposed matrix which is new object. */
static VALUE r_mpfi_matrix_transpose (VALUE self){
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->column, ptr_self->row);
  mpfi_matrix_transpose(ptr_ret, ptr_self);
  return ret;
}

/* Transpose self. This method is destroying method. */
static VALUE r_mpfi_matrix_transpose2 (VALUE self){
  MPFIMatrix *ptr_self, tmp;
  r_mpfi_get_matrix_struct(ptr_self, self);
  if (ptr_self->column > 1 && ptr_self->row > 1) {
    mpfi_matrix_init(&tmp, ptr_self->column, ptr_self->row);
    mpfi_matrix_transpose(&tmp, ptr_self);
    mpfi_matrix_set(ptr_self, &tmp);
    mpfi_matrix_clear(&tmp);
  }
  int i;
  i = ptr_self->column;
  ptr_self->column = ptr_self->row;
  ptr_self->row = i;
  return self;
}

/* Return transposed matrix which is new object. */
static VALUE r_mpfi_matrix_neg (VALUE self){
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  mpfi_matrix_neg(ptr_ret, ptr_self);
  return ret;
}


/* Return true if all elements of _self_ equal to corresponding elements of _other_. */
static VALUE r_mpfi_matrix_equal_p (VALUE self, VALUE other){
  MPFIMatrix *ptr_self, *ptr_other;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_other, other);
  VALUE ret = Qtrue;
  if (mpfi_matrix_equal_p(ptr_self, ptr_other) != 0) {
    ret = Qnil;
  }
  return ret;
}

/* Return _self_ + _other_. */
static VALUE r_mpfi_matrix_add (VALUE self, VALUE other){
  MPFIMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_other, other);
  VALUE ret;
  if (ptr_self->column == ptr_other->column && ptr_self->row == ptr_other->row) {
    r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
    mpfi_matrix_add(ptr_ret, ptr_self, ptr_other);
  }else{
    rb_raise(rb_eArgError, "Matrixes must have same size.");
  }
  return ret;
}

/* Return _self_ + _other_ which is MPFI::Matrix. */
static VALUE r_mpfi_matrix_add2 (VALUE self, VALUE other){
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfi_make_matrix_struct(ret, ptr_ret);

  if(RTEST(rb_funcall(__mpfi_matrix_class__, eqq, 1, other))){
    MPFIMatrix *ptr_other;
    r_mpfi_get_matrix_struct(ptr_other, other);
    if (ptr_self->column == ptr_other->column && ptr_self->row == ptr_other->row) {
      r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
      mpfi_matrix_add(ptr_ret, ptr_self, ptr_other);
    }else{
      rb_raise(rb_eArgError, "Matrixes must have same size.");
    }
  }else if(RTEST(rb_funcall(__mpfr_matrix_class__, eqq, 1, other))){
    MPFRMatrix *ptr_other;
    r_mpfr_get_matrix_struct(ptr_other, other);
    if (ptr_self->column == ptr_other->column && ptr_self->row == ptr_other->row) {
      r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
      mpfi_matrix_add_fr(ptr_ret, ptr_self, ptr_other);
    }else{
      rb_raise(rb_eArgError, "Matrixes must have same size.");
    }    
  }else{
    rb_raise(rb_eArgError, "Argument matrix must be an instance of MPFI::Matrix or MPFR::Matrix.");
  }
  return ret;
}

/* Return _self_ - _other_. */
static VALUE r_mpfi_matrix_sub (VALUE self, VALUE other){
  MPFIMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_other, other);
  VALUE ret;
  r_mpfi_make_matrix_struct(ret, ptr_ret);
  if (ptr_self->column == ptr_other->column && ptr_self->row == ptr_other->row) {
    r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
    mpfi_matrix_sub(ptr_ret, ptr_self, ptr_other);
  }else{
    rb_raise(rb_eArgError, "Matrixes must have same size.");
  }
  return ret;
}

/* Return _self_ - _other_ which is MPFI::Matrix. */
static VALUE r_mpfi_matrix_sub2 (VALUE self, VALUE other){
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfi_make_matrix_struct(ret, ptr_ret);

  if(RTEST(rb_funcall(__mpfi_matrix_class__, eqq, 1, other))){
    MPFIMatrix *ptr_other;
    r_mpfi_get_matrix_struct(ptr_other, other);
    if (ptr_self->column == ptr_other->column && ptr_self->row == ptr_other->row) {
      r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
      mpfi_matrix_sub(ptr_ret, ptr_self, ptr_other);
    }else{
      rb_raise(rb_eArgError, "Matrixes must have same size.");
    }
  }else if(RTEST(rb_funcall(__mpfr_matrix_class__, eqq, 1, other))){
    MPFRMatrix *ptr_other;
    r_mpfr_get_matrix_struct(ptr_other, other);
    if (ptr_self->column == ptr_other->column && ptr_self->row == ptr_other->row) {
      r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
      mpfi_matrix_sub_fr(ptr_ret, ptr_self, ptr_other);
    }else{
      rb_raise(rb_eArgError, "Matrixes must have same size.");
    }    
  }else{
    rb_raise(rb_eArgError, "Argument matrix must be an instance of MPFI::Matrix or MPFR::Matrix.");
  }
  return ret;
}

static VALUE r_mpfi_matrix_vector_inner_product (VALUE self, VALUE arg){
  MPFIMatrix *ptr_self, *ptr_arg;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_arg, arg);
  VALUE ret;
  MPFI *ptr_ret;
  r_mpfi_make_struct_init(ret, ptr_ret);
  if(ptr_self->size == ptr_arg->size){
    mpfi_matrix_inner_product(ptr_ret, ptr_self, ptr_arg);
  }else{
    rb_raise(rb_eArgError, "Vectors must have same size.");
  }
  return ret;
}

/* Return _self_ * _other_. */
static VALUE r_mpfi_matrix_mul_matrix (VALUE self, VALUE other){
  MPFIMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_other, other);
  VALUE ret;
  r_mpfi_make_matrix_struct(ret, ptr_ret);
  if (ptr_self->column == ptr_other->row) {
    if((ptr_other->column == 1) && (ptr_self->row == 1)){
      ret = r_mpfi_matrix_vector_inner_product(self, other);
    }else{
      r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_other->column);
      mpfi_matrix_mul(ptr_ret, ptr_self, ptr_other);
    }
  }else{
    rb_raise(rb_eArgError, "Row size of first matrixes must be same as colmun size of second matrixes.");
  }
  return ret;
}

/* Return _self_ * _other_ which is MPFI::Matrix. */
static VALUE r_mpfi_matrix_mul_matrix2 (VALUE self, VALUE other){
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfi_make_matrix_struct(ret, ptr_ret);

  if(RTEST(rb_funcall(__mpfi_matrix_class__, eqq, 1, other))){
    MPFIMatrix *ptr_other;
    r_mpfi_get_matrix_struct(ptr_other, other);
    if (ptr_self->column == ptr_other->row) {
      r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_other->column);
      mpfi_matrix_mul(ptr_ret, ptr_self, ptr_other);
    }else{
      rb_raise(rb_eArgError, "Row size of first matrixes must be same as colmun size of second matrixes.");
    }
  }else if(RTEST(rb_funcall(__mpfr_matrix_class__, eqq, 1, other))){
    MPFRMatrix *ptr_other;
    r_mpfr_get_matrix_struct(ptr_other, other);
    if (ptr_self->column == ptr_other->row) {
      r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_other->column);
      mpfi_matrix_mul_fr(ptr_ret, ptr_self, ptr_other);
    }else{
      rb_raise(rb_eArgError, "Row size of first matrixes must be same as colmun size of second matrixes.");
    }    
  }else{
    rb_raise(rb_eArgError, "Argument matrix must be an instance of MPFI::Matrix or MPFR::Matrix.");
  }
  return ret;
}

/* Return _scalar_ * _self_, where _scalar_ is scalar and _self_ is matrix. */
static VALUE r_mpfi_matrix_mul_scalar (VALUE self, VALUE scalar){
  MPFIMatrix *ptr_self, *ptr_ret;
  MPFI *ptr_scalar;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(RTEST(rb_funcall(r_mpfi_class, eqq, 1, scalar))){
    r_mpfi_get_struct(ptr_scalar, scalar);
    mpfi_matrix_mul_scalar(ptr_ret, ptr_self, ptr_scalar);
  }else{
    r_mpfi_temp_alloc_init(ptr_scalar);
    r_mpfi_set_robj(ptr_scalar, scalar);
    mpfi_matrix_mul_scalar(ptr_ret, ptr_self, ptr_scalar);
    r_mpfi_temp_free(ptr_scalar);
  }
  return ret;
}

static VALUE r_mpfi_matrix_div_scalar (VALUE self, VALUE scalar){
  MPFIMatrix *ptr_self, *ptr_ret;
  MPFI *ptr_scalar;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(RTEST(rb_funcall(r_mpfi_class, eqq, 1, scalar))){
    r_mpfi_get_struct(ptr_scalar, scalar);
    mpfi_matrix_div_scalar(ptr_ret, ptr_self, ptr_scalar);
  }else{
    r_mpfi_temp_alloc_init(ptr_scalar);
    r_mpfi_set_robj(ptr_scalar, scalar);
    mpfi_matrix_div_scalar(ptr_ret, ptr_self, ptr_scalar);
    r_mpfi_temp_free(ptr_scalar);
  }
  return ret;
}


static VALUE r_mpfi_matrix_include_p (VALUE self, VALUE other){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret = Qnil;
  if(RTEST(rb_funcall(r_mpfi_matrix, eqq, 1, other))){
    MPFIMatrix *ptr_other;
    r_mpfi_get_matrix_struct(ptr_other, other);
    if(mpfi_matrix_include_p(ptr_self, ptr_other) == 0){
      ret = Qtrue;
    }
  }else if(RTEST(rb_funcall(r_mpfr_matrix, eqq, 1, other))){
    MPFRMatrix *ptr_other;
    r_mpfr_get_matrix_struct(ptr_other, other);
    if(mpfi_matrix_include_fr_p(ptr_self, ptr_other) == 0){
      ret = Qtrue;
    }
  }else{
    rb_raise(rb_eArgError, "Argument must be MPFI::Matrix or MPFR::Matrix instance.");
  }
  return ret;
}

static VALUE r_mpfi_matrix_strictly_include_p (VALUE self, VALUE other){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret = Qnil;
  if(RTEST(rb_funcall(r_mpfi_matrix, eqq, 1, other))){
    MPFIMatrix *ptr_other;
    r_mpfi_get_matrix_struct(ptr_other, other);
    if(mpfi_matrix_strictly_include_p(ptr_self, ptr_other) == 0){
      ret = Qtrue;
    }
  }else{
    rb_raise(rb_eArgError, "Argument must be MPFI::Matrix instance.");
  }
  return ret;
}

static VALUE r_mpfi_matrix_bounded_p (VALUE self) {
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  if(mpfi_matrix_bounded_p(ptr_self) == 0){
    return Qtrue;
  }else{
    return Qnil;
  }
}

static VALUE r_mpfi_matrix_mid (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE val_ret;
  MPFRMatrix *ptr_ret;
  r_mpfr_matrix_suitable_matrix_init(&val_ret, &ptr_ret, ptr_self->row, ptr_self->column);
  mpfi_matrix_mid(ptr_ret, ptr_self);
  return val_ret;
}

static VALUE r_mpfi_matrix_mid_interval (VALUE self){
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE val_ret;
  r_mpfi_matrix_suitable_matrix_init(&val_ret, &ptr_ret, ptr_self->row, ptr_self->column);
  mpfi_matrix_mid_interval(ptr_ret, ptr_self);
  return val_ret;
}

static VALUE r_mpfi_matrix_intersect (VALUE self, VALUE other){
  MPFIMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_other, other);
  VALUE val_ret;
  r_mpfi_matrix_suitable_matrix_init(&val_ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(mpfi_matrix_intersect(ptr_ret, ptr_self, ptr_other) == 0){
    return val_ret;
  }else{
    return Qnil;
  }
}

static VALUE r_mpfi_matrix_union (VALUE self, VALUE other){
  MPFIMatrix *ptr_self, *ptr_other, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_other, other);
  VALUE val_ret;
  r_mpfi_matrix_suitable_matrix_init(&val_ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(mpfi_matrix_union(ptr_ret, ptr_self, ptr_other) == 0){
    return val_ret;
  }else{
    return Qnil;
  }
}


static VALUE r_mpfi_matrix_max_diam_abs (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  MPFR *ptr_ret;
  VALUE ret_val;
  r_mpfr_make_struct_init(ret_val, ptr_ret);
  mpfi_matrix_max_diam_abs(ptr_ret, ptr_self);
  return ret_val;
}

/* Return row size of matrix. */
static VALUE r_mpfi_square_matrix_dim (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  return INT2FIX(ptr_self->row);
}

/* Return [matrix_l, matrix_r, indx]. */
static VALUE r_mpfi_square_matrix_lu_decomp (VALUE self){
  MPFIMatrix *ptr_self, *ptr_ret_l, *ptr_ret_u;
  VALUE ret_l, ret_u;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_matrix_suitable_matrix_init (&ret_l, &ptr_ret_l, ptr_self->row, ptr_self->column);
  r_mpfi_matrix_suitable_matrix_init (&ret_u, &ptr_ret_u, ptr_self->row, ptr_self->column);
  VALUE ret_indx[ptr_self->row];
  int indx[ptr_self->row];
  if(mpfi_square_matrix_lu_decomp (ptr_ret_u, indx, ptr_self) >= 0){
    int i, j;
    for(i = 1; i < ptr_ret_u->row; i++){
      for(j = 0; j < i; j++){
	mpfi_set(mpfi_matrix_get_element(ptr_ret_l, i, j), mpfi_matrix_get_element(ptr_ret_u, i, j));
	mpfi_set_si(mpfi_matrix_get_element(ptr_ret_u, i, j), 0);
      }
    }
    for(i = 0; i < ptr_ret_u->row; i++){
      mpfi_set_si(mpfi_matrix_get_element(ptr_ret_l, i, i), 1);
    }
    for(i = 0; i < ptr_ret_u->row; i++){
      for(j = i + 1; j < ptr_ret_u->column; j++){
	mpfi_set_si(mpfi_matrix_get_element(ptr_ret_l, i, j), 0);
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

static VALUE r_mpfi_square_matrix_determinant (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  MPFI *ptr_ret;
  VALUE ret;
  r_mpfi_make_struct_init(ret, ptr_ret);
  mpfi_square_matrix_determinant(ptr_ret, ptr_self);
  return ret;
}

static VALUE r_mpfi_square_matrix_qr_decomp (VALUE self){
  MPFIMatrix *ptr_self, *ptr_q, *ptr_r;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE q, r;
  r_mpfi_matrix_suitable_matrix_init (&q, &ptr_q, ptr_self->row, ptr_self->column);
  r_mpfi_matrix_suitable_matrix_init (&r, &ptr_r, ptr_self->column, ptr_self->column);
  mpfi_square_matrix_qr_decomp(ptr_q, ptr_r, ptr_self);
  return rb_ary_new3(2, q, r);
}

static VALUE r_mpfi_square_matrix_identity (VALUE self, VALUE size){
  VALUE ret;
  MPFIMatrix *ptr_ret;
  int s = NUM2INT(size);
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, s, s);
  mpfi_square_matrix_identity(ptr_ret);
  return ret;
}


/* Return size of data array which equals to column * row. */
static VALUE r_mpfi_vector_dim (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  return INT2FIX(ptr_self->size);
}

/* Take matrix for vector and return length of the vector. */
static VALUE r_mpfi_vector_abs (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret;
  MPFI *ptr_ret;
  r_mpfi_make_struct_init(ret, ptr_ret);
  mpfi_matrix_vector_norm(ptr_ret, ptr_self);
  return ret;
}

/* Return element at _arg_.*/
static VALUE r_mpfi_vector_element (VALUE self, VALUE arg){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  int i = NUM2INT(arg);
  if (i < ptr_self->size) {
    VALUE ret;
    MPFI *ptr_ret;
    r_mpfi_make_struct_init(ret, ptr_ret);
    mpfi_set(ptr_ret, ptr_self->data + i);
    return ret;
  }else{
    return Qnil;
  }
}

/* Set _robj_ to _arg_ th element. */
static VALUE r_mpfi_vector_set_element (VALUE self, VALUE arg, VALUE robj){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  int i = NUM2INT(arg);
  if (i < ptr_self->size) {
    r_mpfi_set_robj(ptr_self->data + i, robj);
  }
  return Qnil;
}

/* Evaluate block with each element of vector. */
static VALUE r_mpfi_vector_each_element (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret = Qnil, tmp;
  MPFI *tmp_ptr;
  r_mpfi_make_struct_init(tmp, tmp_ptr);
  int i;
  for (i = 0; i < ptr_self->size; i++) {
    mpfi_set(tmp_ptr, ptr_self->data + i);
    ret = rb_yield(tmp);
  }
  return ret;
}

/* Evaluate block with each element and its index. */
static VALUE r_mpfi_vector_each_element_with_index (VALUE self){
  MPFIMatrix *ptr_self;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret = Qnil, tmp;
  MPFI *tmp_ptr;
  r_mpfi_make_struct_init(tmp, tmp_ptr);
  int i;
  for (i = 0; i < ptr_self->size; i++) {
    mpfi_set(tmp_ptr, ptr_self->data + i);
    ret = rb_yield(rb_ary_new3(2, tmp, INT2NUM(i)));
  }
  return ret;
}

static VALUE r_mpfi_vector_inner_product (VALUE self, VALUE arg){
  MPFIMatrix *ptr_self, *ptr_arg;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_arg, arg);
  VALUE ret;
  MPFI *ptr_ret;
  r_mpfi_make_struct_init(ret, ptr_ret);
  if(ptr_self->size == ptr_arg->size){
    mpfi_matrix_inner_product(ptr_ret, ptr_self, ptr_arg);
  }else{
    rb_raise(rb_eArgError, "Vectors must have same size.");
  }
  return ret;
}

static VALUE r_mpfi_vector_distance_from (VALUE self, VALUE arg){
  MPFIMatrix *ptr_self, *ptr_arg;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_arg, arg);
  VALUE ret;
  MPFI *ptr_ret;
  r_mpfi_make_struct_init(ret, ptr_ret);
  if(ptr_self->size == ptr_arg->size){
    mpfi_matrix_vector_distance(ptr_ret, ptr_self, ptr_arg);
  }else{
    rb_raise(rb_eArgError, "Vectors must have same size.");
  }
  return ret;
}

static VALUE r_mpfi_vector_distance_center_pts (VALUE self, VALUE arg){
  MPFIMatrix *ptr_self, *ptr_arg;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_arg, arg);
  VALUE ret;
  MPFR *ptr_ret;
  r_mpfr_make_struct_init(ret, ptr_ret);
  if(ptr_self->size == ptr_arg->size){
    mpfi_matrix_vector_distance_center_pts(ptr_ret, ptr_self, ptr_arg);
  }else{
    rb_raise(rb_eArgError, "Vectors must have same size.");
  }
  return ret;
}

static VALUE r_mpfi_vector_midpoint (VALUE self, VALUE arg){
  MPFIMatrix *ptr_self, *ptr_arg, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  r_mpfi_get_matrix_struct(ptr_arg, arg);
  VALUE ret;
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  mpfi_vector_midpoint(ptr_ret, ptr_self, ptr_arg);
  return ret;
}

static VALUE r_mpfi_vector_normalize (VALUE self){
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret;
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(mpfi_vector_normalize(ptr_ret, ptr_self) == 0){
    return ret;
  }else{
    return Qnil;
  }
}

/* Return normalized vector of _self_. This method is destroy method. */
static VALUE r_mpfi_vector_normalize2 (VALUE self){
  MPFIMatrix *ptr_self, tmp;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret = self;
  mpfi_matrix_init(&tmp, ptr_self->column, ptr_self->row);
  if(mpfi_vector_normalize(&tmp, ptr_self) == 0){
    mpfi_matrix_set(ptr_self, &tmp);
  }else{
    ret = Qnil;
  }
  mpfi_matrix_clear(&tmp);
  return ret;
}

static VALUE r_mpfi_vector_set_length (VALUE self, VALUE arg){
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret, returned_value = Qnil;
  MPFR *ptr_l;
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(RTEST(rb_funcall(r_mpfr_class, eqq, 1, arg))){
    r_mpfr_get_struct(ptr_l, arg);
    if(mpfi_vector_set_length(ptr_ret, ptr_self, ptr_l) == 0){
      returned_value = ret;
    }
  }else{
    r_mpfr_temp_alloc_init(ptr_l);
    r_mpfr_set_robj(ptr_l, arg, GMP_RNDN);
    if(mpfi_vector_set_length(ptr_ret, ptr_self, ptr_l) == 0){
      returned_value = ret;
    }
    r_mpfr_temp_free(ptr_l);
  }
  return returned_value;
}

static VALUE r_mpfi_vector_set_length2 (VALUE self, VALUE arg){
  MPFIMatrix *ptr_self, *ptr_ret;
  r_mpfi_get_matrix_struct(ptr_self, self);
  VALUE ret, returned_value = Qnil;
  MPFR *ptr_l;
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr_self->row, ptr_self->column);
  if(RTEST(rb_funcall(r_mpfi_class, eqq, 1, arg))){
    r_mpfr_get_struct(ptr_l, arg);
    if(mpfi_vector_set_length(ptr_ret, ptr_self, ptr_l) == 0){
      mpfi_matrix_set(ptr_self, ptr_ret);
      returned_value = self;
    }
  }else{
    r_mpfr_temp_alloc_init(ptr_l);
    r_mpfr_set_robj(ptr_l, arg, GMP_RNDN);
    if(mpfi_vector_set_length(ptr_ret, ptr_self, ptr_l) == 0){
      mpfi_matrix_set(ptr_self, ptr_ret);
      returned_value = self;
    }
    r_mpfr_temp_free(ptr_l);
  }
  return returned_value;
}



static VALUE r_mpfr_matrix_to_fi_matrix(VALUE self){
  MPFRMatrix *ptr;
  r_mpfr_get_matrix_struct(ptr, self);
  VALUE ret;
  MPFIMatrix *ptr_ret;
  r_mpfi_matrix_suitable_matrix_init (&ret, &ptr_ret, ptr->row, ptr->column);
  mpfi_matrix_from_mpfr_matrix(ptr_ret, ptr);
  return ret;
}

void Init_matrix(){

  /* Initialization of MPFI::Matrix, MPFI::SquareMatrix, MPFI::ColumnVector and MPFI::RowVector */
  VALUE tmp_r_mpfi_class = rb_define_class("MPFI", rb_cNumeric);
  r_mpfi_matrix = rb_define_class_under(tmp_r_mpfi_class, "Matrix", rb_cObject);
  r_mpfi_square_matrix = rb_define_class_under(tmp_r_mpfi_class, "SquareMatrix", r_mpfi_matrix);
  r_mpfi_col_vector = rb_define_class_under(tmp_r_mpfi_class, "ColumnVector", r_mpfi_matrix);
  r_mpfi_row_vector = rb_define_class_under(tmp_r_mpfi_class, "RowVector", r_mpfi_matrix);

  rb_define_singleton_method(r_mpfi_matrix, "Matrix", r_mpfi_matrix_global_new, -1);
  rb_define_alloc_func(r_mpfi_matrix, r_mpfi_matrix_alloc);
  rb_define_private_method(r_mpfi_matrix, "initialize", r_mpfi_matrix_initialize, -1);
  rb_define_private_method(r_mpfi_matrix, "initialize_copy", r_mpfi_matrix_initialize_copy, 1);

  rb_define_singleton_method(tmp_r_mpfi_class, "SquareMatrix", r_mpfi_square_matrix_global_new, 1);
  rb_define_alloc_func(r_mpfi_square_matrix, r_mpfi_square_matrix_alloc);
  rb_define_private_method(r_mpfi_square_matrix, "initialize", r_mpfi_square_matrix_initialize, 1);

  rb_define_singleton_method(tmp_r_mpfi_class, "ColumnVector", r_mpfi_col_vector_global_new, 1);
  rb_define_alloc_func(r_mpfi_col_vector, r_mpfi_col_vector_alloc);
  rb_define_private_method(r_mpfi_col_vector, "initialize", r_mpfi_col_vector_initialize, 1);

  rb_define_singleton_method(tmp_r_mpfi_class, "RowVector", r_mpfi_row_vector_global_new, 1);
  rb_define_alloc_func(r_mpfi_row_vector, r_mpfi_row_vector_alloc);
  rb_define_private_method(r_mpfi_row_vector, "initialize", r_mpfi_row_vector_initialize, 1);


  rb_define_method(r_mpfi_matrix, "at", r_mpfi_matrix_at, 1);
  rb_define_method(r_mpfi_matrix, "set_element", r_mpfi_matrix_set_element, 3);
  rb_define_method(r_mpfi_matrix, "element", r_mpfi_matrix_element, 2);
  rb_define_method(r_mpfi_matrix, "each_element", r_mpfi_matrix_each_element, 0);
  rb_define_method(r_mpfi_matrix, "each_element_with_index", r_mpfi_matrix_each_element_with_index, 0);

  rb_define_method(r_mpfi_matrix, "size", r_mpfi_matrix_size, 0);
  rb_define_method(r_mpfi_matrix, "column_size", r_mpfi_matrix_column_size, 0);
  rb_define_method(r_mpfi_matrix, "row_size", r_mpfi_matrix_row_size, 0);

  rb_define_alias(r_mpfi_matrix, "[]=", "set_element");
  rb_define_alias(r_mpfi_matrix, "[]", "element");
  rb_define_alias(r_mpfi_matrix, "each", "each_element");

  rb_include_module(r_mpfi_matrix, rb_mEnumerable);


  rb_define_method(r_mpfi_matrix, "str_ary_for_inspect", r_mpfi_matrix_str_ary_for_inspect, 0);
  rb_define_method(r_mpfi_matrix, "str_ary_for_inspect2", r_mpfi_matrix_str_ary_for_inspect2, 0);
  rb_define_method(r_mpfi_matrix, "to_str_ary", r_mpfi_matrix_to_str_ary, 0);
  rb_define_method(r_mpfi_matrix, "to_strf_ary", r_mpfi_matrix_to_strf_ary, 1);
  rb_define_method(r_mpfi_matrix, "to_a", r_mpfi_matrix_to_array2, 0);
  rb_define_method(r_mpfi_matrix, "to_a2", r_mpfi_matrix_to_array, 0);


  rb_define_method(r_mpfi_matrix, "row", r_mpfi_matrix_row, 1);
  rb_define_method(r_mpfi_matrix, "column", r_mpfi_matrix_column, 1);
  rb_define_method(r_mpfi_matrix, "transpose", r_mpfi_matrix_transpose, 0);
  rb_define_method(r_mpfi_matrix, "transpose!", r_mpfi_matrix_transpose2, 0);
  rb_define_method(r_mpfi_matrix, "neg", r_mpfi_matrix_neg, 0);


  rb_define_method(r_mpfi_matrix, "==", r_mpfi_matrix_equal_p, 1);
  rb_define_method(r_mpfi_matrix, "+", r_mpfi_matrix_add, 1);
  rb_define_method(r_mpfi_matrix, "add", r_mpfi_matrix_add2, 1);
  rb_define_method(r_mpfi_matrix, "-", r_mpfi_matrix_sub, 1);
  rb_define_method(r_mpfi_matrix, "sub", r_mpfi_matrix_sub2, 1);
  rb_define_method(r_mpfi_matrix, "*", r_mpfi_matrix_mul_matrix, 1);
  rb_define_method(r_mpfi_matrix, "mul", r_mpfi_matrix_mul_matrix2, 1);
  rb_define_method(r_mpfi_matrix, "mul_scalar", r_mpfi_matrix_mul_scalar, 1);
  rb_define_method(r_mpfi_matrix, "div_scalar", r_mpfi_matrix_div_scalar, 1);


  rb_define_method(r_mpfi_matrix, "include?", r_mpfi_matrix_include_p, 1);
  rb_define_method(r_mpfi_matrix, "strictly_include?", r_mpfi_matrix_strictly_include_p, 1);
  rb_define_method(r_mpfi_matrix, "bounded?", r_mpfi_matrix_bounded_p, 0);
  rb_define_method(r_mpfi_matrix, "mid", r_mpfi_matrix_mid, 0);
  rb_define_method(r_mpfi_matrix, "mid_interval", r_mpfi_matrix_mid_interval, 0);
  rb_define_method(r_mpfi_matrix, "intersect", r_mpfi_matrix_intersect, 1);
  rb_define_method(r_mpfi_matrix, "union", r_mpfi_matrix_union, 1);

  rb_define_method(r_mpfi_matrix, "max_diam_abs", r_mpfi_matrix_max_diam_abs, 0);


  rb_define_method(r_mpfi_square_matrix, "dim", r_mpfi_square_matrix_dim, 0);
  rb_define_method(r_mpfi_square_matrix, "lu_decomp", r_mpfi_square_matrix_lu_decomp, 0);
  rb_define_method(r_mpfi_square_matrix, "determinant", r_mpfi_square_matrix_determinant, 0);
  rb_define_method(r_mpfi_square_matrix, "qr_decomp", r_mpfi_square_matrix_qr_decomp, 0);

  rb_define_singleton_method(r_mpfi_square_matrix, "identity", r_mpfi_square_matrix_identity, 1);
  
  /* Initialization of MPFI::Vector module */
  r_mpfi_vector_module = rb_define_module_under(tmp_r_mpfi_class, "Vector");

  rb_include_module(r_mpfi_col_vector, r_mpfi_vector_module);
  rb_include_module(r_mpfi_row_vector, r_mpfi_vector_module);

  rb_define_method(r_mpfi_vector_module, "[]=", r_mpfi_vector_set_element, 2);
  rb_define_method(r_mpfi_vector_module, "[]", r_mpfi_vector_element, 1);
  rb_define_method(r_mpfi_vector_module, "each_element", r_mpfi_vector_each_element, 0);
  rb_define_method(r_mpfi_vector_module, "each_element_with_index", r_mpfi_vector_each_element_with_index, 0);

  rb_define_alias(r_mpfi_vector_module, "each", "each_element");
  rb_define_alias(r_mpfi_vector_module, "each_with_index", "each_element_with_index");

  rb_define_method(r_mpfi_vector_module, "dim", r_mpfi_vector_dim, 0);
  rb_define_method(r_mpfi_vector_module, "abs", r_mpfi_vector_abs, 0);
  rb_define_method(r_mpfi_vector_module, "normalize", r_mpfi_vector_normalize, 0);
  rb_define_method(r_mpfi_vector_module, "normalize!", r_mpfi_vector_normalize2, 0);
  rb_define_method(r_mpfi_vector_module, "set_length", r_mpfi_vector_set_length, 1);
  rb_define_method(r_mpfi_vector_module, "set_length!", r_mpfi_vector_set_length2, 1);

  rb_define_method(r_mpfi_vector_module, "inner_product", r_mpfi_vector_inner_product, 1);
  rb_define_method(r_mpfi_vector_module, "distance_from", r_mpfi_vector_distance_from, 1);
  rb_define_method(r_mpfi_vector_module, "distance_center_pts", r_mpfi_vector_distance_center_pts, 1);
  rb_define_method(r_mpfi_vector_module, "midpoint", r_mpfi_vector_midpoint, 1);

  /* rb_define_method(r_mpfi_matrix, "vector_norm", r_mpfi_matrix_vector_norm, 0); */
  /* rb_define_method(r_mpfi_matrix, "max_norm", r_mpfi_matrix_max_norm, 0); */

  rb_define_method(r_mpfr_matrix, "to_fi_matrix", r_mpfr_matrix_to_fi_matrix, 0);


  eqq = rb_intern("===");
  __mpfr_matrix_class__ = rb_eval_string("MPFR::Matrix");
  __mpfi_matrix_class__ = rb_eval_string("MPFI::Matrix");

}

