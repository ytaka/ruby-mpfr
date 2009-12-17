require File.dirname(__FILE__) + '/spec_helper.rb'

MPFR.set_default_prec(90)

describe MPFR, 'when converting instance of other class' do
  it "should be transformed to float" do
    num = GenerateNumber.float(1000, 100) + [0.0]
    num.each do |a|
      b = MPFR.new(a).get_d(MPFR::RNDZ)
      a.should == b
    end
  end
end

