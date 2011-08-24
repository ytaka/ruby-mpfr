require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

MPFR.set_default_prec(256)

def check_arithmetic_op(arg_ary, error, &block)
  ruby_res = yield(*arg_ary)
  mpfr_res = yield(*arg_ary.map{ |a| MPFR.new(a) })
  mpfr_res = MPFR.new(mpfr_res) unless MPFR === mpfr_res
  (ruby_res - mpfr_res).abs.should < error
end

describe MPFR, 'when calculating MPFR' do

  it "should equal the result of built-in Numeric instance" do
    error = 1.0e-10
    arg = GenerateNumber.float_arguments(1, 1000)
    arg << [0]
    arg.each do |ary|
      check_arithmetic_op(ary, error){ |a| a + 4 }
      check_arithmetic_op(ary, error){ |a| 4 + a }
      check_arithmetic_op(ary, error){ |a| a - 3 }
      check_arithmetic_op(ary, error){ |a| 3 - a }
      check_arithmetic_op(ary, error){ |a| a * 7 }
      check_arithmetic_op(ary, error){ |a| 7 * a }
      check_arithmetic_op(ary, error){ |a| a / 2 }
      check_arithmetic_op(ary, error){ |a| (a.zero? ? 0 : 7 / a) }

      check_arithmetic_op(ary, error){ |a| a + 4.0 }
      check_arithmetic_op(ary, error){ |a| 4.0 + a }
      check_arithmetic_op(ary, error){ |a| a - 3.0 }
      check_arithmetic_op(ary, error){ |a| 3.0 - a }
      check_arithmetic_op(ary, error){ |a| a * 7.0 }
      check_arithmetic_op(ary, error){ |a| 7.0 * a }
      check_arithmetic_op(ary, error){ |a| a / 2.0 }
      check_arithmetic_op(ary, error){ |a| (a.zero? ? 0 : 7.0 / a) }
    end
  end

  it "should equal the result of built-in Numeric instance" do
    error = 1.0e-10
    args = GenerateNumber.float_arguments(2, 1000)
    args += [[0.0, 0.0], [0, -2.2], [0.0, 2.9], [1.1, 1000], [-23.1, 8.7]]
    
    args.each do |ary|
      check_arithmetic_op(ary, error){ |a, b|  a + b }
      check_arithmetic_op(ary, error){ |a, b|  a - b }
      check_arithmetic_op(ary, error){ |a, b|  a * b }
      check_arithmetic_op(ary, error){ |a, b|  (b.zero? ? 0 : a / b) }
    end
  end

  it "should equal the result of built-in Numeric instance" do
    error = 1.0e-10
    args = GenerateNumber.float_arguments(3, 1000)
    args += [[0.0, 0.0, 0.0], [0, 0.0, -2.2], [0.0, 2.9, 0.0], [1.1, 1000, 0.0], [-23.1, 8.7, 0.0]]

    args.each do |ary|
      check_arithmetic_op(ary, error){ |a, b, c|  a * a - b * b + c * c }
      check_arithmetic_op(ary, error){ |a, b, c|  (c.zero? ? (a + b * (-0.3) - c) / 3 : (a + b * (-0.3) - c) / (3 * c)) }
    end
  end
  
end

