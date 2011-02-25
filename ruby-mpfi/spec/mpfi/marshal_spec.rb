require File.expand_path(File.dirname(__FILE__)) + '/spec_helper.rb'

describe MPFI do
  context "when marshaling" do
    before(:all) do
      MPFR.set_default_prec(256)
    end

    it "should restore regular numbers" do
      GenerateNumber.string(1000).each do |s|
        n = MPFI(s, 128)
        n2 = Marshal.load(Marshal.dump(n))
        n2.should == n
        n2.get_prec.should == 128
      end
    end
  end
end
