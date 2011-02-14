class MPFR
  @@error = MPFR('1e-12')

  def self.set_error_of_test(err)
    @error = err
  end

  def self.get_error_of_test
    @error
  end

  def check_error(other, error = nil)
    (self - other).abs < (error || @@error)
  end

  class Matrix
    def check_error(other, error = nil)
      column_size.times.all? do |i|
        row_siize.times.all? do |j|
          self[i][j].check_error(other[i][j], error)
        end
      end
      err = error || @error
      column
    end
  end

  module Vector
    def check_error(other, error = nil)
      size.times.all? do |i|
        self[i].check_error(other[i], error)
      end
    end
  end
end
