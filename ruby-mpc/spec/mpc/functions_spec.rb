require File.dirname(__FILE__) + '/spec_helper.rb'

require "generate_complex_number.rb"

MPFR.set_default_prec(512)

FUNCTIONS_NUM_OF_CALCULATE = 100

def check_mpc_function(arg1, error = 10e-10, &block)
  ruby_res = yield(Complex(*(arg1.map{ |a| a.to_f })))
  mpc_res = yield(MPC.new(*arg1))
  if MPC === mpc_res
    (MPFR.new(ruby_res.real) - mpc_res.real).abs.should < error
    (MPFR.new(ruby_res.imag) - mpc_res.imag).abs.should < error
  elsif MPFR === mpc_res
    (MPFR.new(ruby_res) - mpc_res).abs.should < error
  end
end

describe MPC, "when calculating some functions" do
  it "should be the same result as Complex class" do
    args = GenerateComplex.float_arguments(FUNCTIONS_NUM_OF_CALCULATE)
    args.each do |a|
      check_mpc_function(a) { |b| b.abs }
      check_mpc_function(a) { |b| b.arg }
      check_mpc_function(a) { |b| b.conj }
    end
  end
end
  
