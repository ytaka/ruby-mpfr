require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

describe MPFR, "when calling constant of MPFR" do
  it "should be defined" do
    MPFR::MPFR_VERSION.should be_true
    MPFR::MPFR_VERSION_MAJOR.should be_true
    MPFR::MPFR_VERSION_MINOR.should be_true
    MPFR::MPFR_VERSION_PATCHLEVEL.should be_true
    MPFR::MPFR_VERSION_STRING.should be_true
    MPFR::PREC_MAX.should be_true
    MPFR::PREC_MIN.should be_true
    MPFR::EMAX_DEFAULT.should be_true
    MPFR::EMIN_DEFAULT.should be_true
    MPFR::RNDN.should be_true
    MPFR::RNDZ.should be_true
    MPFR::RNDU.should be_true
    MPFR::RNDD.should be_true
    # MPFR::VERSION
  end
end

