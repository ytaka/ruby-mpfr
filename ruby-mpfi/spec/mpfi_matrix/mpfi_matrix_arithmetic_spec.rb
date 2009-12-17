require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFI::Matrix, "when making matrix negative" do
  before(:all) do
    MPFR.set_default_prec(512)
    @row = 8
    @column = 3
    @args = GenerateNumber.float_matrix_arguments(1000, @row, @column).map{ |a| MPFI::Matrix.new(a) }
  end

  it "should be negative" do
    zero_matrix = MPFI::Matrix.new(@row, @column)
    @args.each { |m| (zero_matrix - m).should == m.neg }
  end
  
end

describe MPFI::Matrix, "when arithmetic operator applies to 2x3 matrixies" do
  before(:all) do
    MPFR.set_default_prec(128)
    @row = 2
    @column = 3
    args = GenerateNumber.float_matrix_arguments(1000, @row, @column)
    @args_i = args.map{ |a| MPFI::Matrix.new(a) }
    @args_r = args.map{ |a| MPFR::Matrix.new(a) }
  end

  it "should be added" do
    @args_i.each_index do |i|
      if i > 0
        res = @args_i[i-1] + @args_i[i]
        (0...@row).each do |j|
          (0...@column).each { |k| res[j, k].should eql(@args_i[i-1][j, k] + @args_i[i][j, k]) }
        end
        res.include?(@args_r[i-1] + @args_r[i]).should be_true
      end
    end
  end

  it "should be subtracted" do
    @args_i.each_index do |i|
      if i > 0
        res = @args_i[i-1] - @args_i[i]
        (0...@row).each do |j|
          (0...@column).each { |k| res[j, k].should eql(@args_i[i-1][j, k] - @args_i[i][j, k]) }
        end
        res.include?(@args_r[i-1] - @args_r[i]).should be_true
      end
    end
  end

  it "should be multiplied" do
    @args_i.each_index do |i|
      if i > 0
        trans = @args_i[i].transpose
        res = @args_i[i-1] * trans
        (0...@row).each do |j|
          (0...@row).each do |k|
            res2 = MPFI.new(0)
            (0...@column).each { |l| res2 += @args_i[i-1][j, l] * trans[l, k] }
            res[j, k].should eql res2
          end
        end
        res.include?(@args_r[i-1] * @args_r[i].transpose).should be_true
        
        trans = @args_i[i-1].transpose
        res = trans * @args_i[i]
        (0...@column).each do |j|
          (0...@column).each do |k|
            res2 = MPFI.new(0)
            (0...@row).each { |l| res2 += trans[j, l] * @args_i[i][l, k] }
            res[j, k].should eql res2
          end
        end
        res.include?(@args_r[i-1].transpose * @args_r[i]).should be_true
      end
    end
  end

  it "should be multyplied by scalar" do
    @args_i.each do |a|
      scalar = MPFI.new(GenerateNumber.float(1)[0])
      res = a.mul_scalar(scalar)
      (0...@row).each do |j|
        (0...@column).each { |k| res[j, k].should eql a[j, k] * scalar }
      end
    end
  end
  
  it "should be divided by scalar" do
    @args_i.each do |a|
      scalar = MPFI.new(GenerateNumber.float(1)[0])
      res = a.div_scalar(scalar)
      (0...@row).each do |j|
        (0...@column).each { |k| res[j, k].should eql a[j, k] / scalar }
      end
    end
  end
  
end

describe MPFI::Matrix, "when arithmetic operator applies to 2x3 MPFR::Matrix" do
  before(:all) do
    MPFR.set_default_prec(175)
    @row = 2
    @column = 3
    @args = GenerateNumber.float_matrix_arguments(1000, @row, @column)
  end

  it "should be added" do
    @args.each_index do |i|
      if i > 0
        rmat = MPFR::Matrix.new(@args[i-1])
        imat = MPFI::Matrix.new(@args[i])
        res = imat.add(rmat)
        res.should be_an_instance_of MPFI::Matrix
        (0...@row).each do |j|
          (0...@column).each { |k| (imat[j, k] + rmat[j, k]).should eql res[j, k] }
        end
      end
    end
  end

  it "should be subtracted" do
    @args.each_index do |i|
      if i > 0
        rmat = MPFR::Matrix.new(@args[i-1])
        imat = MPFI::Matrix.new(@args[i])
        res = imat.sub(rmat)
        res.should be_an_instance_of MPFI::Matrix
        (0...@row).each do |j|
          (0...@column).each { |k| (imat[j, k] - rmat[j, k]).should eql res[j, k] }
        end
      end
    end
  end

  it "should be multiplied" do
    @args.each_index do |i|
      if i > 0
        rmat = MPFR::Matrix.new(@args[i-1]).transpose
        imat = MPFI::Matrix.new(@args[i])
        res = imat.mul(rmat)

        (0...@row).each do |j|
          (0...@row).each do |k|
            res2 = MPFI.new(0)
            (0...@column).each { |l| res2 += (imat[j, l] * rmat[l, k]) }
            res[j, k].should eql res2
          end
        end
      end
    end
  end

end
