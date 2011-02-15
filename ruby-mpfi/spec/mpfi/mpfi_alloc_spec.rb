require_relative 'spec_helper'

describe MPFI, "when initializing MPFI instance" do
  before(:all) do
    @prec = 150
    MPFR.set_default_prec(@prec)
    @srcs = GenerateNumber.float(1000) + GenerateNumber.float(1000).map!{ |a| MPFR.new(a.to_s) }
    @args = @srcs.map{ |a| MPFI.new(a) }
  end

  it "should include source number and has endpoints which is MPFR" do
    @args.each_with_index do |a, i|
      a.get_prec.should eql @prec
      endpoints = a.endpoints
      endpoints.each do |b|
        b.should be_an_instance_of MPFR
        b.get_prec.should eql @prec
      end
      endpoints[0].should <= @srcs[i]
      endpoints[1].should >= @srcs[i]
      a.include?(@srcs[i]).should be_true
    end
  end
  
end

describe MPFI, "when initializing MPFI instance by method interval" do
  before(:all) do
    @prec = 130
    MPFR.set_default_prec(@prec)
    @srcs = GenerateNumber.interval_float_arguments(1000)
    @srcs += GenerateNumber.interval_float_arguments(1000).map!{ |a| a.map!{ |b| MPFR.new(b.to_s) }}
    @args = @srcs.map { |a| MPFI.interval(*a) }
  end

  it "should include source numbers and has endpoints which is MPFR" do
    @args.each_with_index do |a, i|
      a.get_prec.should eql @prec
      endpoints = a.endpoints
      endpoints.each do |b|
        b.should be_an_instance_of MPFR
        b.get_prec.should eql @prec
      end
      endpoints[0].should <= @srcs[i][0]
      endpoints[1].should >= @srcs[i][1]
      @srcs[i].each { |b| a.include?(b).should be_true }
    end
  end

end

