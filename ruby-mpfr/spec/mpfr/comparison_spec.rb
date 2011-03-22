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

describe MPFR do
  context "checking whether number is nan" do
    it "should return true" do
      MPFR('nan').nan?.should be_true
    end

    it "should return false" do
      MPFR('1.23').nan?.should be_false
      MPFR('inf').nan?.should be_false
    end
  end

  context "checking whether number is finite" do
    it "should return true" do
      MPFR('-0.28').finite?.should be_true
    end

    it "should return false" do
      MPFR('nan').finite?.should be_false
      MPFR('inf').finite?.should be_false
    end
  end

  context "checking whether number is zero" do
    it "should return true" do
      MPFR(0).zero?.should be_true
    end

    it "should return false" do
      MPFR(1).zero?.should be_false
      MPFR('nan').zero?.should be_false
      MPFR('inf').zero?.should be_false
    end
  end

  context "checking whether number is nonzero" do
    it "should return true" do
      MPFR(1).nonzero?.should be_true
      MPFR('nan').nonzero?.should be_true
      MPFR('inf').nonzero?.should be_true
    end

    it "should return false" do
      MPFR(0).nonzero?.should be_false
    end
  end

  context "checking whether number is regular" do
    it "should return true" do
      MPFR(-1).regular?.should be_true
    end

    it "should return false" do
      MPFR(0).regular?.should be_false
      MPFR('nan').regular?.should be_false
      MPFR('inf').regular?.should be_false
    end
  end
end
