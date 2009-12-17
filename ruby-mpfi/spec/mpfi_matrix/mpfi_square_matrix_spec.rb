require File.dirname(__FILE__) + '/spec_helper.rb'

describe MPFI::SquareMatrix, "when calculating LU decomposition" do
  before(:all) do
    MPFR.set_default_prec(128)
    @dim = 7
    @mat = GenerateNumber.float_matrix_arguments(100, @dim, @dim).map{ |a| MPFI::SquareMatrix.new(a) }
    @mat_fr = @mat.map { |a| a.mid_interval }
  end

  it "should include lu decomposition of MPFR::SquareMatrix" do
    @mat.each_with_index do |m, i|
      l, u, indx = m.lu_decomp
      l_fr, u_fr, indx_fr = @mat_fr[i].lu_decomp
      l.include?(l_fr).should be_true
      u.include?(u_fr).should be_true
    end
  end
end

describe MPFI::SquareMatrix, "when calculating determinant" do
  before(:all) do
    MPFR.set_default_prec(512)
    @error = MPFR.new('1e-30')
  end

  it "should include value of determinant of MPFR::SquareMatrix" do
    (2..6).each do |dim|
      GenerateNumber.float_matrix_arguments(100, dim, dim).each do |src|
        m = MPFI::SquareMatrix.new(src)
        m_fr = MPFR::SquareMatrix.new(src)
        m.determinant.include?(m_fr.determinant).should be_true
      end
    end
  end

end
