require File.expand_path(File.join(File.dirname(__FILE__), "../spec_helper.rb"))

describe MPFR::Matrix, "when MPFR instance is converted to string by some methods." do
  
  before(:all) do
    MPFR.set_default_prec(128)
    @row = 2
    @column = 3
    @matrixes = ((0..100).map { |i| MPFR::Matrix.new(GenerateNumber.float_arguments(@column, @row)) })
  end

  it "should return array having strings." do
    @matrixes.each do |m|
      m.str_ary("%.Re").each_with_index do |s, i|
        s.to_s.should eql m[i % @row, i / @row].to_s
      end
    end
  end

  it "should return inspect string." do
    @matrixes.each do |m|
      ins = m.inspect
      ins.should match(/#{Regexp.escape(sprintf("%x", m.object_id))}/)
      m.each do |a|
        ins.should match(/#{Regexp.escape(a.to_s)}/)
      end
    end
  end
  
end

