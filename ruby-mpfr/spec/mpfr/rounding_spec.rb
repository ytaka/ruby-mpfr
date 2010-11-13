require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

describe MPFR, "when rounding number by prec_round" do
  before(:all) do
    MPFR.set_default_prec(200)
    @prec = 100
    @args = GenerateNumber.float(1000).map{ |a| MPFR.new(a.to_s) }
  end

  it "should be rounded up" do
    @args.each do |a|
      a.prec_round(MPFR::RNDU, @prec).should >= a
    end
  end

  it "should be rounded down" do
    @args.each do |a|
      a.prec_round(MPFR::RNDD, @prec).should <= a
    end
  end

  it "should be rounded to zero" do
    @args.each do |a|
      if a > 0
        a.prec_round(MPFR::RNDZ, @prec).should <= a
      else
        a.prec_round(MPFR::RNDZ, @prec).should >= a 
      end
    end
  end

  it "should be rounded to near number" do
    @args.each do |a|
      min = [(a - a.prec_round(MPFR::RNDU, @prec)).abs, (a - a.prec_round(MPFR::RNDD, @prec)).abs].min
      (a - a.prec_round(MPFR::RNDN, @prec)).abs.should eql min
    end
  end

  it "should be rounded by prec_round!" do
    @args.each do |a|
      [MPFR::RNDU, MPFR::RNDD, MPFR::RNDN, MPFR::RNDZ].each do |rnd|
        b = a.dup
        old_object_id = b.object_id
        b.prec_round!(rnd, @prec)
        b.should eql a.prec_round(rnd, @prec)
        b.object_id.should eql old_object_id
      end
    end
  end
  
end
