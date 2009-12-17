require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFR, "when executing methods which are exception related functions" do
  it "should get and set emin and emax" do
    (emin = MPFR.get_emin).should be_a_kind_of Integer
    (emax = MPFR.get_emax).should be_a_kind_of Integer
    test_num = 1000
    MPFR.set_emin(-test_num)
    MPFR.get_emin.should == -test_num
    MPFR.set_emax(test_num)
    MPFR.get_emax.should == test_num
    MPFR.set_emin(emin)
    MPFR.set_emax(emax)
    MPFR.get_emin_min.should be_a_kind_of Integer
    MPFR.get_emin_max.should be_a_kind_of Integer
    MPFR.get_emax_min.should be_a_kind_of Integer
    MPFR.get_emax_max.should be_a_kind_of Integer
  end
end

describe MPFR, "when executing exception of MPFR" do
  it "should set exception" do
    MPFR.set_underflow
    MPFR.set_overflow
    MPFR.set_nanflag
    MPFR.set_inexflag
    MPFR.set_erangeflag
    MPFR.underflow_p.should be_true
    MPFR.overflow_p.should be_true
    MPFR.nanflag_p.should be_true
    MPFR.inexflag_p.should be_true
    MPFR.erangeflag_p.should be_true
    MPFR.clear_underflow
    MPFR.clear_overflow
    MPFR.clear_nanflag
    MPFR.clear_inexflag
    MPFR.clear_erangeflag
    MPFR.underflow_p.should be_false
    MPFR.overflow_p.should be_false
    MPFR.nanflag_p.should be_false
    MPFR.inexflag_p.should be_false
    MPFR.erangeflag_p.should be_false

    MPFR.set_underflow
    MPFR.set_overflow
    MPFR.set_nanflag
    MPFR.set_inexflag
    MPFR.set_erangeflag
    MPFR.clear_flags
    MPFR.underflow_p.should be_false
    MPFR.overflow_p.should be_false
    MPFR.nanflag_p.should be_false
    MPFR.inexflag_p.should be_false
    MPFR.erangeflag_p.should be_false
  end
end

# Need to spec for the following methods.
# check_range
# subnormalize
