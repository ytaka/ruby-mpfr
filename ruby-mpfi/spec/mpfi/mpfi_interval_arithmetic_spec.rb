require_relative 'spec_helper'

#
# To test method +, -, / and *, we calculate these operator for MPFR and MPFI
# and check whether resultant MPFI instance includes MPFR instance.
# Then, this program make MPFI instance to use MPFI.new and MPFI.interval.
#

def check_arithmetic_op_include(arg1, arg2, &block)
  mpfr_res = yield(MPFR.new(arg1), MPFR.new(arg2))
  mpfi_res = yield(MPFI.new(arg1), MPFI.new(arg2))
  mpfi_res.include?(mpfr_res)
end

def check_arithmetic_op_include2(arg1, arg2, error, &block)
  ary = [MPFR.new(arg1), MPFR.new(arg2)]
  mpfr_res = yield(ary[0], ary[1])
  mpfi_res = yield(MPFI.interval(ary[0] - error, ary[0] + error),
                   MPFI.interval(ary[1] - error, ary[1] + error))
  mpfi_res.include?(mpfr_res)
end

describe MPFI, "when applying arithmetic operations" do
  before(:all) do
    MPFR.set_default_prec(100)
    @args = GenerateNumber.float(5000)
    @error = MPFR.new('1e-20')
  end

  it "should be include" do
    @args.each_index do |i|
      if i > 0
        (check_arithmetic_op_include(@args[i-1], @args[i]) { |a, b| a + b }).should be_true
        (check_arithmetic_op_include(@args[i-1], @args[i]) { |a, b| a - b }).should be_true
        (check_arithmetic_op_include(@args[i-1], @args[i]) { |a, b| a * b }).should be_true
        (check_arithmetic_op_include(@args[i-1], @args[i]) { |a, b| a / b }).should be_true

        (check_arithmetic_op_include2(@args[i-1], @args[i], @error) { |a, b| a + b }).should be_true
        (check_arithmetic_op_include2(@args[i-1], @args[i], @error) { |a, b| a - b }).should be_true
        (check_arithmetic_op_include2(@args[i-1], @args[i], @error) { |a, b| a * b }).should be_true
        (check_arithmetic_op_include2(@args[i-1], @args[i], @error) { |a, b| a / b }).should be_true
      end
    end
  end
  
end

# It is assumed that op is :add, :sub, :mul or :div.
def arithmetic_op_with_mpfr(arg_r, arg_i, op, &block)
  r = MPFR.new(arg_r)
  i = MPFI.new(arg_i)
  res = MPFI::Math.send(op, i, r)
  res.should be_an_instance_of MPFI
  res.left.should eql MPFR::Math.send(op, i.left, r, MPFR::RNDD)
  res.right.should eql MPFR::Math.send(op, i.right, r, MPFR::RNDU)
end

describe MPFI, "when applying arithmetic operations with MPFR" do
  before(:all) do
    MPFR.set_default_prec(200)
    @args = GenerateNumber.float(5000)
  end

  it "should be include" do
    @args.each_index do |i|
      if i > 0
        arithmetic_op_with_mpfr(@args[i-1], @args[i], :add)
        arithmetic_op_with_mpfr(@args[i-1], @args[i], :sub)
        arithmetic_op_with_mpfr(@args[i-1], @args[i], :mul)
        arithmetic_op_with_mpfr(@args[i-1], @args[i], :div)
      end
    end
  end
  
end

# op must be :add or :sub.
def arithmetic_op_with_mpfr2(arg_r, arg_i, op, &block)
  r = MPFR.new(arg_r)
  i = MPFI.interval(*arg_i)
  res = MPFI::Math.send(op, i, r)
  res.should be_an_instance_of MPFI
  res.left.should eql MPFR::Math.send(op, i.left, r, MPFR::RNDD)
  res.right.should eql MPFR::Math.send(op, i.right, r, MPFR::RNDU)
  res.diam_abs.should eql i.diam_abs
end

describe MPFI, "when applying addition or subtraction with MPFR" do
  before(:all) do
    MPFR.set_default_prec(200)
    @args_r = GenerateNumber.float(5000)
    @args_i = GenerateNumber.interval_float_arguments(5000)
  end

  it "should be include" do
    @args_r.each_index do |i|
      arithmetic_op_with_mpfr2(@args_r[i], @args_i[i], :add)
      arithmetic_op_with_mpfr2(@args_r[i], @args_i[i], :sub)
    end
  end
end
