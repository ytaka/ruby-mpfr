require_relative 'spec_helper'

describe MPFI, "when initializing MPFI instance" do
  before(:all) do
    @prec = 150
    MPFR.set_default_prec(@prec)
    @srcs = GenerateNumber.float(1000) + GenerateNumber.float(1000).map!{ |a| MPFR.new(a.to_s) }
    @args = @srcs.map{ |a| MPFI.new(a) }
  end

  it "should include source number and has endpoints which is MPFR" do
    @args.each_with_index do |a, i|
      a.get_prec.should eql @prec
      endpoints = a.endpoints
      endpoints.each do |b|
        b.should be_an_instance_of MPFR
        b.get_prec.should eql @prec
      end
      endpoints[0].should <= @srcs[i]
      endpoints[1].should >= @srcs[i]
      a.include?(@srcs[i]).should be_true
    end
  end
  
end

describe MPFI, "when initializing MPFI instance by method interval" do
  before(:all) do
    @prec = 130
    MPFR.set_default_prec(@prec)
    @srcs = GenerateNumber.interval_float_arguments(1000)
    @srcs += GenerateNumber.interval_float_arguments(1000).map!{ |a| a.map!{ |b| MPFR.new(b.to_s) }}
    @args = @srcs.map { |a| MPFI.interval(*a) }
  end

  it "should include source numbers and has endpoints which is MPFR" do
    @args.each_with_index do |a, i|
      a.get_prec.should eql @prec
      endpoints = a.endpoints
      endpoints.each do |b|
        b.should be_an_instance_of MPFR
        b.get_prec.should eql @prec
      end
      endpoints[0].should <= @srcs[i][0]
      endpoints[1].should >= @srcs[i][1]
      @srcs[i].each { |b| a.include?(b).should be_true }
    end
  end

end

describe MPFI, "when setting value from some objects" do
  before(:all) do
    MPFR.set_default_prec(128)
    MPFR.set_error_of_test(MPFR('1e-12'))
  end

  it "should create an MPFI object from a string" do
    n = MPFI("[1.2, 1.3]")
    n.left.check_error(1.2).should be_true
    n.right.check_error(1.3).should be_true
  end

  it "should create an MPFI object from an integer" do
    n = MPFI(8)
    n.left.check_error(8).should be_true
    n.right.check_error(8).should be_true
  end

  it "should create an MPFI object from an array of float numbers" do
    n = MPFI([-1.3, -0.5])
    n.left.check_error(-1.3)
    n.right.check_error(-0.5)
  end

  it "should create an MPFI object from an array of MPFR numbers" do
    n = MPFI([MPFR(-1.0), MPFR(-0.5)])
    n.left.check_error(-1.0)
    n.right.check_error(-0.5)
  end

  it "should create an MPFI object from an array of strings" do
    n = MPFI(['3.8', '4.9'])
    n.left.check_error(3.8)
    n.right.check_error(4.9)
  end

  it "should create an MPFI object from an array of some objects" do
    n = MPFI([MPFR(1.2), '4.9'])
    n.left.check_error(1.2)
    n.right.check_error(4.9)
  end

end
