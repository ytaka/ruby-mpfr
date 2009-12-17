require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFI::ColumnVector, "when dividing to some parts" do
  before(:all) do
    MPFR.set_default_prec(128)
    @vec = MPFI::ColumnVector.new([MPFI.interval(1, 3), MPFI.interval(-5, -4), MPFI.interval(2, 5)])
  end

  it "should return divided boxes." do
    size = MPFR.new('0.5')
    ary = @vec.subdivision_by_size(size)
    ary.size.should == 48
  end

end
