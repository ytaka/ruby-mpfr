require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFR, 'when allocating objects from integer' do
  it "should equal bignum" do
    MPFR.set_default_prec(100)
    num = 1
    for i in 0..300
      num *= 10
      MPFR.new(num).should == num
    end
  end

  it "should allocate from string" do
    MPFR.set_default_prec(53)
    for i in 0..1000
      MPFR.new(i.to_s).should == i
    end
  end

  it "should equal float" do
    MPFR.set_default_prec(256)
    error = MPFR.new('1.0e-10')
    GenerateNumber.float(100) do |f|
      MPFR.new(f).should == f
    end
  end

  it "should duplicate" do
    MPFR.set_default_prec(141)
    for i in 0..1000
      a = rand(10**10)
      b = rand(10**50)
      str = "#{a}.#{b}"
      c = MPFR.new(str)
      d = c.dup
      c.should == d
      c.object_id.should_not == d.object_id
    end
  end

  it "should coerce" do
    MPFR.set_default_prec(89)
    a = MPFR.new('0.253242')
    for i in 0..1000
      b = MPFR.new(i)
      c = a.coerce(i)[0]
      b.should == c
    end

    a = MPFR.new('-13572834.923')
    for i in 0..1000
      str = (i + rand).to_s
      b = MPFR.new(str)
      c = a.coerce(str)[0]
      b.should == c
    end
  end
  
end

