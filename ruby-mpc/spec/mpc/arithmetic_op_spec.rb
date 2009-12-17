require File.dirname(__FILE__) + '/spec_helper.rb'

require "generate_complex_number.rb"

MPFR.set_default_prec(256)

ARITHMETIC_NUM_OF_CALCULATE = 1000

def check_mpc_arithmetic_op(arg1, arg2, error = 10e-10, &block)
  ruby_res = yield(Complex(*(arg1.map{ |a| a.to_f })), Complex(*(arg2.map{ |a| a.to_f })))
  mpc_res = yield(MPC.new(*arg1), MPC.new(*arg2))
  (MPFR.new(ruby_res.real) - mpc_res.real).abs.should < error
  (MPFR.new(ruby_res.imag) - mpc_res.imag).abs.should < error
end

describe MPC, "when calculating" do
  it "should be complex number" do
    args = GenerateComplex.float_arguments(ARITHMETIC_NUM_OF_CALCULATE)
    args.each_index do |i|
      if i > 0
        check_mpc_arithmetic_op(args[i-1], args[i]){ |a, b| a + b }
        check_mpc_arithmetic_op(args[i-1], args[i]){ |a, b| a - b }
        check_mpc_arithmetic_op(args[i-1], args[i]){ |a, b| a * b }
        check_mpc_arithmetic_op(args[i-1], args[i]){ |a, b| a / b }
      end
    end
  end

end

