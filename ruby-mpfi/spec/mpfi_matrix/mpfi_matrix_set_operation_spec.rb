require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFI::Matrix, "when an interval includes other" do
  before(:all) do
    MPFR.set_default_prec(100)
    @row = 3
    @column = 2
    tmp = GenerateNumber.float_matrix_interval_arguments(100, @row, @column).map{ |a| MPFI::Matrix.interval(a) }
    @args1 = tmp[0...50]
    @args2 = tmp[50..-1]
  end

  
end

describe MPFI::Matrix, "when a box intersects other" do
  before(:all) do
    MPFR.set_default_prec(128)
    @row = 3
    @column = 2
    tmp = GenerateNumber.float_matrix_interval_arguments(100, @row, @column).map{ |a| MPFI::Matrix.interval(a) }
    @args1 = tmp[0...50]
    @args2 = tmp[50..-1]
  end

  it "should return box or nil" do
    total = [0,0]
    @args1.each do |a1|
      @args2.each do |a2|
        res = a1.intersect(a2)
        tmp = []
        (0...(a1.size)).each { |k| tmp << a1.at(k).intersect(a2.at(k)) }
        if tmp.any?{ |b| NilClass === b }
          res.should be_nil
          total[1] += 1
        else
          (0...(res.size)).each { |k| res.at(k).should == tmp[k] }
          a1.include?(res).should be_true
          a2.include?(res).should be_true
          total[0] += 1
        end
      end
    end
  end
end

describe MPFI::Matrix, "when checking whether matrix is bounded or not bounded" do
  before(:all) do
    MPFR.set_default_prec(128)
    @row = 2
    @column = 3
    @bounded = GenerateNumber.float_matrix_interval_arguments(100, @row, @column).map{ |a| MPFI::Matrix.interval(a) }
    @unbounded = GenerateNumber.float_matrix_interval_arguments(100, @row, @column).map do |a|
      if rand > 0.5
        a[rand(@row)][rand(@column)][0] = MPFR.minf
      else
        a[rand(@row)][rand(@column)][1] = MPFR.pinf
      end
      MPFI::Matrix.interval(a)
    end
  end

  it "should be bounded" do
    @bounded.each { |a| a.bounded?.should be_true }
  end

  it "should be unbounded" do
    @unbounded.each { |a| a.bounded?.should be_nil }
  end
  
end
