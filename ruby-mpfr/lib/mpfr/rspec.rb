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
end
