require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

MPFR.set_default_prec(256)

def check_comparison(a, b)
  ((a < b) == (MPFR.new(a) < MPFR.new(b))).should be_true
  ((a > b) == (MPFR.new(a) > MPFR.new(b))).should be_true
  ((a == b) == (MPFR.new(a) == MPFR.new(b))).should be_true
end

describe MPFR, "when comparing two numbers" do
  it "should return the same result to built-in Numeric comparison" do
    args = GenerateNumber.float_arguments(2, 1000)
    args += [[0.0, 0.0], [-2.89, -2.89], [5.2, 2.9], [1000, 1000], [8.7, 8.7]]

    args.each do |ary|
      check_comparison(*ary)
    end
  end
end

