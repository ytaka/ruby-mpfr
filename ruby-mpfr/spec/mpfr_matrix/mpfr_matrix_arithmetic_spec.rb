require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFR::Matrix, "when making matrix negative" do
  before(:all) do
    MPFR.set_default_prec(512)
    @row = 8
    @column = 3
    @args = GenerateNumber.float_matrix_arguments(1000, @row, @column).map{ |a| MPFR::Matrix.new(a) }
  end

  it "should be negative" do
    zero_matrix = MPFR::Matrix.new(@row, @column)
    @args.each { |m| (zero_matrix - m).should == m.neg }
  end
  
end

describe MPFR::Matrix, "when arithmetic operator applies to 2x3 matrixies" do
  before(:all) do
    MPFR.set_default_prec(128)
    @row = 2
    @column = 3
    @args = GenerateNumber.float_matrix_arguments(1000, @row, @column).map!{ |a| MPFR::Matrix.new(a) }
  end

  it "should be added" do
    @args.each_index do |i|
      if i > 0
        res = @args[i-1] + @args[i]
        (0...@row).each do |j|
          (0...@column).each { |k| res[j, k].should eql(@args[i-1][j, k] + @args[i][j, k]) }
        end
      end
    end
  end

  it "should be subtracted" do
    @args.each_index do |i|
      if i > 0
        res = @args[i-1] + @args[i]
        (0...@row).each do |j|
          (0...@column).each { |k| res[j, k].should eql(@args[i-1][j, k] + @args[i][j, k]) }
        end
      end
    end
  end

  it "should be multiplied" do
    @args.each_index do |i|
      if i > 0
        trans = @args[i].transpose
        res = @args[i-1] * trans
        (0...@row).each do |j|
          (0...@row).each do |k|
            res2 = MPFR.new(0)
            (0...@column).each { |l| res2 += @args[i-1][j, l] * trans[l, k] }
            res[j, k].should eql res2
          end
        end
        trans = @args[i-1].transpose
        res = trans * @args[i]
        (0...@column).each do |j|
          (0...@column).each do |k|
            res2 = MPFR.new(0)
            (0...@row).each { |l| res2 += trans[j, l] * @args[i][l, k] }
            res[j, k].should eql res2
          end
        end
      end
    end
  end

  it "should be multyplied by scalar" do
    @args.each do |a|
      scalar = MPFR.new(GenerateNumber.float(1)[0])
      res = a.mul_scalar(scalar)
      (0...@row).each do |j|
        (0...@column).each { |k| res[j, k].should eql a[j, k] * scalar }
      end
    end
  end
  
  it "should be divided by scalar" do
    @args.each do |a|
      scalar = MPFR.new(GenerateNumber.float(1)[0])
      res = a.div_scalar(scalar)
      (0...@row).each do |j|
        (0...@column).each { |k| res[j, k].should eql a[j, k] / scalar }
      end
    end
  end
  
end
