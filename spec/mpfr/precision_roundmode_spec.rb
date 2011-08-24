require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

def check_rnd(a)
  MPFR.new(a).should == MPFR.new(a, MPFR.get_default_rounding_mode)
  n = MPFR.new(a, MPFR::RNDN)
  z = MPFR.new(a, MPFR::RNDZ)
  u = MPFR.new(a, MPFR::RNDU)
  d = MPFR.new(a, MPFR::RNDD)
  a = MPFR.new(a, MPFR::RNDA)
  (n > 0 ? d : u).should == z
  [(a - z).abs, (a - u).abs, (a - d).abs].min.should >= (a - n).abs
  (d <= u && (n == z || n == u || n == d)).should be_true
  z.abs.should <= a.abs
end

def check_set_rnd(rnd1, rnd2)
  MPFR.set_default_rounding_mode(rnd1)
  MPFR.get_default_rounding_mode.should == rnd1
  MPFR.set_default_rounding_mode(rnd2)
  MPFR.get_default_rounding_mode.should_not == rnd1
  MPFR.get_default_rounding_mode.should == rnd2
end

describe MPFR, 'when getting rounding mode' do
  it "should return rounding mode" do
    args = GenerateNumber.float(100)
    args += [Math::PI, -Math::PI, Math::E, -Math::E, 0, 10, 27, -9, -293578]
    args.each{ |a| check_rnd(a) }
    [MPFR::RNDN, MPFR::RNDZ, MPFR::RNDU, MPFR::RNDD, MPFR::RNDA].each{ |rnd| args.each{ |a| check_rnd(a) } }
  end

  it "should be set rounding mode" do
    rnds = [MPFR::RNDN, MPFR::RNDZ, MPFR::RNDU, MPFR::RNDD, MPFR::RNDA]
    for i in 0...rnds.size
      for j in (i+1)...rnds.size
        check_set_rnd(rnds[i], rnds[j])
      end
    end
    MPFR.set_default_rounding_mode(MPFR::RNDN)
  end
end

def check_set_prec(prec)
  MPFR.set_default_prec(prec)
  MPFR.get_default_prec.should == prec
  a = MPFR.new(rand(10000) - rand)
  MPFR.get_default_prec.should == a.get_prec
  prec2 = prec / 2
  a.set_prec(prec2)
  a.nan?.should be_true
  MPFR.get_default_prec.should_not == a.get_prec
  prec2.should == a.get_prec
  b = MPFR.new(rand(10000) - rand)
  prec3 = prec * 3
  MPFR.set_default_prec(prec3)
  c = MPFR.new(rand(10000) - rand)
  prec3.should == c.get_prec
  c.get_prec.should_not == b.get_prec
end

describe MPFR, 'when setting precision' do
  it "should be set precision" do
    for i in 1...1000
      check_set_prec(20 + rand(i))
    end  
  end
end

