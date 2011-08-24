require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

describe MPFR::Matrix, "when marshaling" do
  before(:all) do
    MPFR.set_default_prec(128)
  end

  it "should be restore" do
    row = 8
    column = 3
    args = GenerateNumber.float_matrix_arguments(1000, row, column).map{ |a| MPFR::Matrix.new(a) }
    args.each do |m|
      m2 = Marshal.load(Marshal.dump(m))
      m2.should == m
    end
  end
end

describe MPFR::SquareMatrix, "when marshaling" do
  before(:all) do
    MPFR.set_default_prec(128)
  end

  it "should be restore" do
    row = 4
    column = 4
    args = GenerateNumber.float_matrix_arguments(1000, row, column).map{ |a| MPFR::SquareMatrix.new(a) }
    args.each do |m|
      m2 = Marshal.load(Marshal.dump(m))
      m2.should == m
    end
  end
end

describe MPFR::ColumnVector, "when marshaling" do
  before(:all) do
    MPFR.set_default_prec(128)
  end

  it "should be restore" do
    row = 1
    column = 6
    args = GenerateNumber.float_matrix_arguments(1000, row, column).map{ |a| MPFR::ColumnVector.new(a[0]) }
    args.each do |m|
      m2 = Marshal.load(Marshal.dump(m))
      m2.should == m
    end
  end
end

describe MPFR::RowVector, "when marshaling" do
  before(:all) do
    MPFR.set_default_prec(128)
  end

  it "should be restore" do
    row = 1
    column = 6
    args = GenerateNumber.float_matrix_arguments(1000, row, column).map{ |a| MPFR::RowVector.new(a[0]) }
    args.each do |m|
      m2 = Marshal.load(Marshal.dump(m))
      m2.should == m
    end
  end
end
