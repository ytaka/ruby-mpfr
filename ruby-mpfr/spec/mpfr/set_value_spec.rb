require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

MPFR.set_default_prec(300)

describe MPFR, 'when setting abnormal number' do
  it "should be set nan" do
    a = MPFR.new(rand)
    a.set_nan
    a.nan?.should be_true
    a.to_s.should == "nan"
  end

  it "should be set infinity" do
    a = MPFR.new(rand)
    a.set_inf(0)
    a.infinite?.should be_true
    a.to_s.should == "inf"
    
    a.set_inf(1)
    a.infinite?.should be_true
    a.to_s.should == "inf"
    
    a.set_inf(-1)
    a.infinite?.should be_true
    a.to_s.should == "-inf"
  end  
end

describe MPFR, 'when setting value' do
  it "should swap each other" do
    num = 100
    args = [GenerateNumber.mpfr_args(num), GenerateNumber.mpfr_args(num)]
    args[0].each_index{ |i| check_swap(args[0][i], args[1][i]) }
  end

  it "should set other value" do
    [0, 3, 2.2, -0.2, '1.0e-3', '-2e14', '2225'].each{ |val| check_set(val) }
  end

  it "should set value by set_fixnum_2exp" do
    for i in 0..100
      check_fixnum_2exp(rand(100), rand(100))
    end
  end
end

def check_swap(a, b)
  tmp_a = a.dup
  tmp_b = b.dup
  tmp_a.swap(tmp_b)
  a.should == tmp_b
  b.should == tmp_a
end

def check_set(val)
  tmp = MPFR.new
  tmp.set(val)
  tmp2 = tmp.dup
  a = (tmp2 + 3) / Math::PI
  tmp.nan?.should_not be_true
  tmp.should_not == a
end

def check_fixnum_2exp(num, exp)
  ret = MPFR.new(num)
  if exp < 0
    for i in 0...(exp.abs)
      ret /= MPFR.new(2)
    end
  else
    for i in 0...exp
      ret *= MPFR.new(2)
    end
  end
  MPFR.new.set_fixnum_2exp(num, exp).should == ret
end

