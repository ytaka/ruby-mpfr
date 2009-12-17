require File.dirname(__FILE__) + '/spec_helper.rb'

#
# 03/19/09 11:51:21
# This spec test is almost same test as the one for mpfr/matrix.
#

describe "initialization of matrix" do
  before(:all) do
    MPFR.set_default_prec(128)
    @sizes = [[1, 2], [2, 3], [8, 5], [7, 3]]
    @matrixes = @sizes.map{ |a| MPFI::Matrix.new(*a) }
  end

  it "should has size which equals size of array." do
    @matrixes.each_with_index do |a, i|
      a.size.should == (@sizes[i][0] * @sizes[i][1])
    end
  end

  it "should has size of row which equals size of array." do
    @matrixes.each_with_index do |a, i|
      a.row_size.should == @sizes[i][0]
    end
  end

  it "should has size of column which equals size of array." do
    @matrixes.each_with_index do |a, i|
      a.column_size.should == @sizes[i][1]
    end
  end

  it "should have MPFI instances as elements." do
    @matrixes.each do |m|
      m.each { |a| a.should be_an_instance_of MPFI }
    end
  end
  
end

describe "argument error raises for invalid initialization argument" do
  before(:all) do
    MPFR.set_default_prec(256)
    @data = [[[1, 2, 3], [0, 0]],
             [[MPFI.new(0)], [MPFI.new(1), 3]]]
  end
  
  it "should make error for invalid array argument" do
    @data.each do |ary|
      lambda { MPFI::Matrix.new(ary) }.should raise_error ArgumentError
    end
  end
end

describe "when methods of matrix 'dup' and 'transpose' are executed" do
  before(:all) do
    MPFR.set_default_prec(256)
    @data = [[[1, 2], [3, 3]],
             [[-1, 0.8], ['1.2342', 99]],
             [[36.237582938, 7122], ['-1e7', 8237]],
             [[-2.2738, 8.23, 9.237482], [11, '8237.2374', '1239']],
             [[MPFI.new('23'), MPFI.new('123.23')], [MPFI.new(-3.2378), MPFI.new('1.2837')]]]
    @matrixes = @data.map{ |a| MPFI::Matrix.new(a) }
  end

  it "should have MPFI instances as elements." do
    @matrixes.each do |m|
      m.each { |a| a.should be_an_instance_of MPFI }
    end
  end
  
  it "should create matrix which has same values as original and different object id." do
    @matrixes.each do |m|
      new = m.dup
      m.object_id.should_not eql new.object_id
      (0...(m.row_size)).each do |i|
        (0...(m.column_size)).each { |j| m[i, j].should eql new[i, j] }
      end
      m.should == new
    end
  end
  
  it "should has transposed values." do
    @matrixes.each do |m|
      id = m.object_id
      old = m.dup
      new = m.transpose
      m.transpose!
      m.should_not == old
      m.should == new
      m.object_id.should == id
      m.object_id.should_not == new.object_id
      m.column_size.should == old.row_size
      m.row_size.should == old.column_size
    end
  end
end

describe "when methods of vector 'dup' and 'transpose' are executed" do
  before(:all) do
    MPFR.set_default_prec(768)
    @data = [[1, 2, 3, 3],
             [-1, 0.8, '1.2342', 99],
             [36.237582938, 7122, '-1e7', 8237],
             [-2.2738, 8.23, 9.237482, '8237.2374', '1239'],
             [MPFI.new('23'), MPFI.new('123.23'), MPFI.new(-3.2378), MPFI.new('1.2837')]]
    @column = @data.map{ |a| MPFI::ColumnVector.new(a) }
    @row = @data.map{ |a| MPFI::RowVector.new(a) }
  end

  it "should create matrix which has same values as original and different object id." do
    (@column + @row).each do |m|
      new = m.dup
      m.object_id.should_not == new.object_id
      m.should == new
    end
  end

  it "should change row vector to column vector and make the opposite changes" do
    (@column + @row).each do |m|
      id = m.object_id
      old = m.dup
      new = m.transpose
      m.transpose!
      m.should_not == old
      m.should == new
      m.object_id.should == id
      m.object_id.should_not == new.object_id
      m.column_size.should == old.row_size
      m.row_size.should == old.column_size
    end
  end
  
end
