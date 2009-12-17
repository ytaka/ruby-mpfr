require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPC, "when returning constants" do
  it "should have constants" do
    constants = MPC.constants
    [:RNDNN, :RNDNZ, :RNDNU, :RNDND, :RNDZN, :RNDZZ, :RNDZU, :RNDZD,
     :RNDUN, :RNDUZ, :RNDUU, :RNDUD, :RNDDN, :RNDDZ, :RNDDU, :RNDDD].each do |cst|
      constants.include?(cst).should be_true
      eval("MPC::#{cst}").should be_instance_of Fixnum
    end

    [:VERSION_MAJOR, :VERSION_MINOR, :VERSION_PATCHLEVEL].each do |cst|
      constants.include?(cst).should be_true
      eval("MPC::#{cst}").should be_instance_of Fixnum
    end

    constants.include?(:VERSION_STRING).should be_true
    MPC::VERSION_STRING.should be_instance_of String
  end

end


