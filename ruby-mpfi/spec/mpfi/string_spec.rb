require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFI, "when converting to string" do
  it "should return string created by inspect" do
    s = MPFI(['1.2', '1.3']).inspect
    s.should be_an_instance_of String
  end

  it "should return an array of strings" do
    a = MPFI(['1.2', '1.3']).to_str_ary
    a.should be_an_instance_of Array
    a.size.should == 2
  end
end
