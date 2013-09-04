require File.expand_path(File.join(File.dirname(__FILE__), "../spec_helper.rb"))

describe MPFR do
  context "when marshaling" do
    before(:all) do
      MPFR.set_default_prec(256)
    end

    it "should restore nan" do
      n = MPFR.nan(128)
      n2 = Marshal.load(Marshal.dump(n))
      n2.nan?.should be_true
      n2.get_prec.should == 128
    end

    it "should restore plus infinity" do
      n = MPFR.pinf(128)
      n2 = Marshal.load(Marshal.dump(n))
      n2.infinite?.should be_true
      n2.get_prec.should == 128
      n2.sgn.should > 0
    end

    it "should restore minus infinity" do
      n = MPFR.minf(128)
      n2 = Marshal.load(Marshal.dump(n))
      n2.infinite?.should be_true
      n2.get_prec.should == 128
      n2.sgn.should < 0
    end

    it "should restore plus zero" do
      n = MPFR.zero(1, 128)
      n2 = Marshal.load(Marshal.dump(n))
      n2.zero?.should be_true
      n2.get_prec.should == 128
      n2.sgn.should >= 0
    end

    it "should restore minus zero" do
      n = MPFR.zero(-1, 128)
      n2 = Marshal.load(Marshal.dump(n))
      n2.zero?.should be_true
      n2.get_prec.should == 128
      n2.sgn.should <= 0
    end

    it "should restore regular numbers" do
      GenerateNumber.string(1000).each do |s|
        n = MPFR(s, MPFR::RNDN, 128)
        n2 = Marshal.load(Marshal.dump(n))
        n2.should == n
        n2.get_prec.should == 128
      end
    end
  end
end
