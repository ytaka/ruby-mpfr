require_relative 'spec_helper'

describe MPFI, "when calculating mathematical functions" do
  it "should not raise error" do
    MPFR.set_default_prec(256)
    num = MPFI.new('4.2184930258398098')
    # p MPFI::Math.sqr(num)
    # p MPFI::Math.sqrt(num)
  end
end

