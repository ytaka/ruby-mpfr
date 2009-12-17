require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFI::Matrix, "when making mid_interval" do
  before(:all) do
    MPFR.set_default_prec(512)
    @row = 4
    @column = 1
    @args = GenerateNumber.float_vector_arguments(1000, @row, @column).map{ |a| MPFI::ColumnVector.new(a) }
  end

  it "should have mid_interval including midpoint" do
    @args.each do |a|
      mid_int = a.mid_interval
      a.include?(mid_int).should be_true
      mid_int.include?(a.mid).should be_true
    end
  end

  it "should have midpoint of two points." do
    @args.each_with_index do |a, ind|
      m = a.midpoint(@args[ind-1])
      m2 = (a + @args[ind-1]).div_scalar(MPFI.new(2))
      (0...@row).each do |i|
        m[i].should == m2[i]
      end
      m.include?(a.mid.midpoint(@args[ind-1].mid)).should be_true
    end
  end
end

