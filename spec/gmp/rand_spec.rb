require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

describe MPFR, "when creating random number" do
  before(:all) do
    @prec = 200
    MPFR.set_default_prec(@prec)
    @creator = [GMP::RandState.new, GMP::RandState.new(:mt),
                GMP::RandState.new(:lc_2exp, 3, 4, 10), GMP::RandState.new(:lc_2exp_size, 93)]
  end

  it "should return MPFR objects" do
    @creator.each do |c|
      10.times do |i|
        n = c.mpfr_urandomb2
        n.should be_an_instance_of MPFR
        n.should >= 0
        n.should <= 1
      end
    end
  end

end
