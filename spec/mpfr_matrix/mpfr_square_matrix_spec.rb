require File.expand_path(File.join(File.dirname(__FILE__), "../spec_helper.rb"))

describe MPFR::SquareMatrix, "when calculating LU decomposition" do
  before(:all) do
    MPFR.set_default_prec(128)
    @dim = 7
    @matrixies = GenerateNumber.float_matrix_arguments(100, @dim, @dim).map{ |a| MPFR::SquareMatrix.new(a) }
    @error = MPFR.new('1e-30')
  end

  it "should have lu decomposition" do
    @matrixies.each do |mat|
      l, u, indx = mat.lu_decomp
      if l
        mul = l * u
        (0...(indx.size)).to_a.reverse.each do |i|
          if i != indx[i]
            row = mul.row(i)
            (0...(mul.column_size)).each do |j|
              mul.set_element(i, j, mul[indx[i], j])
              mul.set_element(indx[i], j, row[j])
            end
          end
        end
        (0...(mat.row_size)).each do |i|
          (0...(mat.column_size)).each do |j|
            (mat[i, j] - mul[i, j]).abs.should < @error
          end
        end
      end
    end
  end
end

describe MPFR::SquareMatrix, "when calculating determinant" do
  before(:all) do
    MPFR.set_default_prec(512)
    @error = MPFR.new('1e-30')
  end

  it "should have the same values of determinant by formula and lu decomposition" do
    [2, 3].each do |dim|
      GenerateNumber.float_matrix_arguments(100, dim, dim).each do |src|
        m = MPFR::SquareMatrix.new(src)
        l, u, idx = m.lu_decomp
        if l
          det = MPFR.new(1)
          (0...(u.row_size)).each { |i| det *= u[i, i] }
          count = 0
          idx.each_with_index { |ind, i| count += (ind != i ? 1 : 0) }
          det = -det if count % 2 == 1
          (det - m.determinant).should < @error
        end
      end
    end
  end

  it "should have the same values of determinant by formula and lu decomposition" do
    [4, 5, 6].each do |dim|
      GenerateNumber.float_matrix_arguments(100, dim, dim).each do |src|
        m = MPFR::SquareMatrix.new(src)
        l, u, idx = m.lu_decomp
        if l
          det = MPFR.new(1)
          (0...(u.row_size)).each { |i| det *= u[i, i] }
          count = 0
          idx.each_with_index { |ind, i| count += (ind != i ? 1 : 0) }
          det = -det if count % 2 == 1
          (det - m.determinant).should < @error
        end
      end
    end
  end

end
