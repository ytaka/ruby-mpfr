require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFI::Matrix, "when setting number to particular element" do
  before(:all) do
    MPFR.set_default_prec(128)
    @row = 4
    @column = 7
    @matrix = MPFI::Matrix.new(@row, @column)
    @source = GenerateNumber.float_matrix_arguments(1, @row, @column)[0]
  end

  it "should have elements which equals to source number." do
    (0...(@matrix.row_size)).each do |i|
      (0...(@matrix.column_size)).each do |j|
        fr = MPFI.new(@source[i][j].to_s)
        @matrix.set_element(i, j, fr)
        @matrix[i, j].should eql fr
      end
    end
  end

end

describe MPFI::Matrix, "when calculating row and column" do
  before(:all) do
    MPFR.set_default_prec(128)
    @row = 6
    @column = 4
    @sources = GenerateNumber.float_matrix_arguments(100, @row, @column)
    @matrixies = @sources.map { |a| MPFI::Matrix.new(a) }
  end

  it "should have row vector." do
    @matrixies.each do |m|
      (0...(m.column_size)).each do |i|
        col = m.column(i)
        (0...(m.row_size)).each do |j|
          col[j].should eql m[j, i]
        end
      end
    end
  end

  it "should have column vector." do
    @matrixies.each do |m|
      (0...(m.row_size)).each do |i|
        row = m.row(i)
        (0...(m.column_size)).each do |j|
          row[j].should eql m[i, j]
        end
      end
    end
  end
  
end
