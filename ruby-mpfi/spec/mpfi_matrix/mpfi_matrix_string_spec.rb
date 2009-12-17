require File.dirname(__FILE__) + '/spec_helper.rb'

describe "when MPFI instance is converted to string by some methods." do
  
  before(:all) do
    MPFR.set_default_prec(128)
    @row = 2
    @column = 3
    @matrixes = ((0..100).map { |i| MPFI::Matrix.new(GenerateNumber.float_arguments(@column, @row)) })
  end

  it "should return array having strings." do
    @matrixes.each do |m|
      m.str_ary_for_inspect.each_with_index do |s, i|
        s.should eql "#{m[i % @row, i / @row].left.to_strf('%.Re')} #{m[i % @row, i / @row].right.to_strf('%.Re')}"
      end
    end
  end

  it "should return inspect string." do
    @matrixes.each do |m|
      ins = m.inspect
      ins.should match(/#{Regexp.escape(sprintf("%x", m.object_id))}/)
      m.each do |a|
        str = "#{a.left.to_strf('%.Re')} #{a.right.to_strf('%.Re')}"
        ins.should match(/#{Regexp.escape(str)}/)
      end
    end
  end
  
end

