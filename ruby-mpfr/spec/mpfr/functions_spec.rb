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

describe MPFR::Math do
  context "when executing functions in MPFR::Math." do
    it "should return MPFR" do
      methods = [:const_log2, :const_pi, :const_euler, :const_catalan]
      methods.each { |m| MPFR::Math.__send__(m).should be_an_instance_of MPFR }
    end

    it "should return MPFR from one arbitrary number." do
      methods = [:sqr, :sqrt, :rec_sqrt, :cbrt,
                 :log, :log2, :log10, :exp, :exp2, :exp10,
                 :cos, :sin, :tan, :sec, :csc, :cot, :acos, :sin, :atan,
                 :cosh, :sinh, :tanh, :sech, :csch, :coth, :acosh, :asinh, :atanh,
                 :log1p, :expm1, :eint, :li2, :gamma, :lngamma, :digamma,
                 :zeta, :erf, :erfc, :j0, :j1, :y0, :y1, :ai]
      [[MPFR('2.2379')]].each do |args|
        methods.each { |m| MPFR::Math.__send__(m, *args).should be_an_instance_of MPFR }
      end
    end

    it "should return MPFR from one MPFR number and one positive integer." do
      methods = [:mul_2si, :div_2si, :fac_ui,
                 :jn, :yn]
      [[MPFR('2.2379'), 10]].each do |args|
        methods.each { |m| MPFR::Math.__send__(m, *args).should be_an_instance_of MPFR }
      end
    end

    it "should return an array from one arbitrary number." do
      methods = [:sin_cos, :sinh_cosh]
      [[MPFR('2.2379')]].each do |args|
        methods.each { |m| MPFR::Math.__send__(m, *args).should be_an_instance_of Array }
      end
    end

    it "should return MPFR from two arbitrary numbers." do
      methods = [:add, :sub, :mul, :div, :pow, :dim,
                 :atan2, :agm, :hypot]
      [[MPFR('10.287'), MPFR('3.827')]].each do |args|
        methods.each { |m| MPFR::Math.__send__(m, *args).should be_an_instance_of MPFR }
      end
    end

    it "should return MPFR from three arbitrary numbers." do
      methods = [ :fma, :fms]
      [[MPFR('2.199'), MPFR('7.83'), MPFR('5.28')]].each do |args|
        methods.each { |m| MPFR::Math.__send__(m, *args).should be_an_instance_of MPFR }
      end
    end


  end
end
