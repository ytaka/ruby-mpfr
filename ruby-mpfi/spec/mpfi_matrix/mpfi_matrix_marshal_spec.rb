require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

describe MPFI::Matrix, "when marshaling" do
  before(:all) do
    MPFR.set_default_prec(128)
  end

  it "should be restore" do
    row = 8
    column = 3
    args = GenerateNumber.float_matrix_arguments(1000, row, column).map{ |a| MPFI::Matrix.new(a) }
    args.each do |m|
      m2 = Marshal.load(Marshal.dump(m))
      m2.should == m
    end
  end
end

describe MPFI::SquareMatrix, "when marshaling" do
  before(:all) do
    MPFR.set_default_prec(128)
  end

  it "should be restore" do
    row = 4
    column = 4
    args = GenerateNumber.float_matrix_arguments(1000, row, column).map{ |a| MPFI::SquareMatrix.new(a) }
    args.each do |m|
      m2 = Marshal.load(Marshal.dump(m))
      m2.should == m
    end
  end
end

describe MPFI::ColumnVector, "when marshaling" do
  before(:all) do
    MPFR.set_default_prec(128)
  end

  it "should be restore" do
    row = 1
    column = 6
    args = GenerateNumber.float_matrix_arguments(1000, row, column).map{ |a| MPFI::ColumnVector.new(a[0]) }
    args.each do |m|
      m2 = Marshal.load(Marshal.dump(m))
      m2.should == m
    end
  end
end

describe MPFI::RowVector, "when marshaling" do
  before(:all) do
    MPFR.set_default_prec(128)
  end

  it "should be restore" do
    row = 1
    column = 6
    args = GenerateNumber.float_matrix_arguments(1000, row, column).map{ |a| MPFI::RowVector.new(a[0]) }
    args.each do |m|
      m2 = Marshal.load(Marshal.dump(m))
      m2.should == m
    end
  end
end
