require 'rubygems'
require 'spec'
require 'mpfr'
require 'mpfi'
require "#{File.dirname(File.expand_path(__FILE__))}/generate_number_module.rb"

describe MPFI, "when checking inclusive relations" do
  before(:all) do
    @a = MPFI.interval(-2, 5)
    @b = MPFI.interval(-3, 2)
    @c = MPFI.interval(5, 8)
    @d = MPFI.interval(7, 9)
    @e = MPFI.interval(-2, 0)
    @f = MPFI.interval(-1, 0)
  end

  it "should be some variations of return value for method include?." do
    @a.include?(@b).should be_nil
    @a.include?(@c).should be_nil
    @a.include?(@d).should be_nil
    @a.include?(@e).should be_true
    @a.include?(@f).should be_true

    @a.strictly_include?(@b).should be_nil
    @a.strictly_include?(@c).should be_nil
    @a.strictly_include?(@d).should be_nil
    @a.strictly_include?(@e).should be_nil
    @a.strictly_include?(@f).should be_true

    @a.intersect(@b).should be_true
    @a.intersect(@c).should be_true
    @a.intersect(@d).should be_nil
    @a.intersect(@e).should be_true
    @a.intersect(@f).should be_true
  end
  
end

describe MPFI, "when making intersection of two intervals" do
  before(:all) do
    MPFR.set_default_prec(200)
    src = [[2, 3], [4, 5], [-7, 0]]
    tmp = GenerateNumber.float_arguments(2, 200).map { |a| a[0] < a[1] ? MPFI.interval(*a) : MPFI.interval(a[1], a[0])}
    @base = [MPFI.interval(1, 3)] + tmp[0...20]
    @intervals = src.map{ |a| MPFI.interval(*a) } + tmp[20..-1]
  end

  it "should return intersection intervals or nil" do
    res = [0, 0]
    @base.each do |a|
      @intervals.each do |b|
        intersect = a.intersect(b)
        if a.left > b.right || a.right < b.left
          intersect.should be_nil
          a.intersect2(b).empty?.should be_true
          res[1] += 1
        else
          intersect.should == MPFI.interval([a.left, b.left].max, [a.right, b.right].min)
          intersect.should == a.intersect2(b)
          a.include?(intersect).should be_true
          b.include?(intersect).should be_true
          res[0] += 1
        end
      end
    end
  end

end

describe MPFI, "when checking whether interval is bounded or not bounded" do
  before(:all) do
    MPFR.set_default_prec(128)
    @bounded = GenerateNumber.float_arguments(2, 200).map { |a| a[0] < a[1] ? MPFI.interval(*a) : MPFI.interval(a[1], a[0])}
    @unbounded = [[MPFR.new(3), MPFR.pinf], [MPFR.minf, MPFR.new(-9)]].map { |a| MPFI.interval(*a)}
  end

  it "should be bounded" do
    @bounded.each { |a| a.bounded?.should be_true }
  end

  it "should be unbounded" do
    @unbounded.each { |a| a.bounded?.should be_nil }
  end
  
end

describe MPFI, "when making interval" do
  before(:all) do
    MPFR.set_default_prec(128)
    @sources = GenerateNumber.float_arguments(2, 100)
  end

  it "should not raise error" do
    @sources.each do |a|
      interval = nil
      lambda { interval = (a[0] < a[1] ? MPFI.interval(a[0].to_s, a[1].to_s) : MPFI.interval(a[1].to_s, a[0].to_s)) }.should_not raise_error
      interval.include?(MPFR.new(a[0].to_s)).should be_true
      interval.include?(MPFR.new(a[1].to_s)).should be_true
    end
  end
  
end
