require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFI::Matrix, "when subdividing interval" do
  before(:all) do
    @size = MPFR.new('1')
    @arg = MPFI::ColumnVector.new([MPFI.interval(-1.8, -1.729), MPFI.interval('-1e-2', '8.2e-2'), MPFI.interval('1.82', '2.27')])
  end
  
  it "should return unchanged object" do
    res = @arg.subdivision_by_size(@size)
    res.size.should eql 1
    res.each { |a| a.max_diam_abs.should <= @size }
  end
end
