require_relative 'spec_helper'

describe MPFI, "when puting arithmetic operation" do
  before(:all) do
    MPFR.set_default_prec(150)
    @args = GenerateNumber.float(5000).map{ |a| MPFI.new(a) }
  end

  it "should increase diameter" do
    @args.each_index do |i|
      if i > 0
        diam_sum = @args[i-1].diam + @args[i].diam
        add = @args[i-1] + @args[i]
        sub = @args[i-1] - @args[i]
        add.diam.should >= diam_sum
        sub.diam.should >= diam_sum
      end
    end
  end
  
end
