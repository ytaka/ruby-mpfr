require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

MPFR.set_default_prec(256)

describe MPFR, 'when executing mathematical functions' do
  it "should calculate basic functions" do
    error = MPFR.new('1.0e-10')
    a = MPFR::Math.const_pi
    (MPFR.new(Math::PI) - a).abs.should < error
    (MPFR::Math.sin(a) - MPFR.new('0')).abs.should < error
    (MPFR::Math.cos(a) - MPFR.new('-1')).abs.should < error
    a = MPFR.new(Math::PI/4.0)
    (MPFR::Math.sin(a) - MPFR.new(Math.sqrt(2.0)/2.0)).abs.should < error
    (MPFR::Math.cos(a) - MPFR.new(Math.sqrt(2.0)/2.0)).abs.should < error
    (MPFR::Math.tan(a) - MPFR.new(1)).abs.should < error

    (MPFR::Math.atan(MPFR.new(1) / MPFR::Math.sqrt('3')) - MPFR::Math.const_pi / MPFR.new(6)).abs.should < error

    a = MPFR::Math.exp(1)
    (Math::E - a).abs.should < error
    (MPFR.new(1) - MPFR::Math.log(a)).abs.should < error
  end
end


