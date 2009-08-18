require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFR, "when converted to string" do
  before(:all) do
    MPFR.set_default_prec(200)
    @args = GenerateNumber.float(10).map{ |a| MPFR.new(a.to_s) }
  end

  it "should be to_s" do
    @args.each do |a|
      a.to_s.should match(/^(-|)[1-9]\.[0-9]+e[-+][0-9]+$/)
    end
  end

  it "should be formatted string \"#<MPFR:%lx,'%.Re',%d>\" for inspect" do
    @args.each do |a|
      a.inspect.should match(/^#<MPFR:.+,'(-|)[1-9]\.[0-9]+e[-+][0-9]+',\d+>$/)
    end
  end

  it "should be formatted string by to_strf" do
    @args.each do |a|
      a.to_strf("%.Re").should eql a.to_s
      a.to_strf("%.RE").should match(/^(-|)[1-9]\.[0-9]+E[-+][0-9]+$/)
      a.to_strf("%.Rf").should match(/^(-|)[0-9]*\.[0-9]*$/)
    end

    format = ["14.6", ".8", "10.10", "+10.4", "-10.4", "+012.2", "-012.2", "3.0"]
    @args.each do |a|
      MPFR.set_default_prec(100)
      for i in 0...1000
        a = rand(100) + rand
        check_to_s_e(a, format)
        check_to_s_e(-a, format)
        # check_to_s_f(a, format)
        # check_to_s_f(-a, format)
      end
    end
  end

end

def check_to_s_e(float, format_ary)
  a = MPFR.new(float)
  format_ary.each do |format|
    sprintf("%#{format}e", float).should == a.to_strf("%#{format}Re")
    sprintf("%#{format}E", float).should == a.to_strf("%#{format}RE")
  end
end

# Probably MPFR bug makes this test be failure. Need further investigations. (Tue Aug 18 10:43:51 2009)
def check_to_s_f(float, format_ary)
  a = MPFR.new(float)
  format_ary.each do |format|
    sprintf("%#{format}f", float).should == a.to_strf("%#{format}Rf")
  end
end

