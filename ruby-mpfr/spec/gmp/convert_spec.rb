require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

describe MPFR, "when creating random number" do
  before(:all) do
    @prec = 128
    MPFR.set_default_prec(@prec)
  end

  it "should return GMP::F objects" do
    n = MPFR.new('1.234')
    n.to_mpf.should be_an_instance_of GMP::F
    n.to_mpf.should == GMP::F.new('1.234')
  end

  it "should return GMP::F objects" do
    n = MPFR.new('3.878')
    n.to_mpz.should be_an_instance_of GMP::Z
    n.to_mpz.should == GMP::Z.new(4)
  end

  it "should return MPFR objects." do
    n = GMP::F.new('-87.21')
    n.to_fr.should be_an_instance_of MPFR
    n.to_fr.should == MPFR.new('-87.21')
  end

end
