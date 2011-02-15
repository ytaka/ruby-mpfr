require_relative 'spec_helper'

#
# The following is test for interval functions and endpoints. 
#

describe MPFI, "when executing interval functions" do
  before(:all) do
    MPFR.set_default_prec(140)
    @args = GenerateNumber.float(1000).map{ |a| MPFI.new(a.to_s) }
  end

  it "should be equal the distance between endpoints" do
    @args.each { |a| a.diam_abs.should >= (a.right - a.left) }
  end

  it "should be equal midpoint" do
    @args.each { |a| a.mid.should == ((a.right + a.left) / 2) }
  end

  it "should include midpoint" do
    @args.each { |a| a.mid_interval.include?(a.mid).should be_true }
  end

  it "should be include self.alea" do
    @args.each { |a| a.include?(a.alea).should be_true }
  end
  
end

describe MPFI, "when getting endpoints of interval" do
  before(:all) do
    MPFR.set_default_prec(200)
    @args = GenerateNumber.float(1000).map{ |a| MPFI.new(a.to_s) }
  end

  it "should include two endpoints" do
    @args.each do |a|
      a.include?(a.left).should be_true
      a.include?(a.right).should be_true
    end
  end

  it "should not strictly include interval which consists of one endpoint" do
    @args.each do |a|
      a.strictly_include?(MPFI.new(a.left)).should be_nil
      a.strictly_include?(MPFI.new(a.right)).should be_nil
    end
  end
  
  it "should equal [left, right]." do
    @args.each do |a|
      epts = a.endpoints
      a.left.should eql epts[0]
      a.right.should eql epts[1]
    end
  end
  
end

describe MPFI, "when dividing interval" do
  before(:all) do
    MPFR.set_default_prec(200)
    @num = 5
    @args = [MPFI.interval(2, 8), MPFI.interval('-1e-2', '8.2e-2'), MPFI.interval('-1e2', '-3.738e1')]
    @error = MPFR.new('1e-20')
  end

  it "should split to some pieces" do
    @args.each do |x|
      res = x.subdivision(@num)
      res.size.should eql @num
      l = x.diam_abs / @num
      res.each { |a| (a.diam_abs - l).abs.should <= @error }
      (1...(@num - 1)).each { |i| res[i].left.should eql res[i-1].right }
      res[0].left.should eql x.left
      res[-1].right.should eql x.right
    end
  end

  it "should return unchanged object" do
    @args.each do |x|
      res = x.subdivision(1)
      res.size.should eql 1
      l = x.diam_abs
      res.each { |a| (a.diam_abs - l).abs.should <= @error }
      res[0].should eql x
    end
  end
end

