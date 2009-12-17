module GenerateComplex

  def self.float_arguments(number, max = 100)
    ret = Array.new(number){ |j| Array.new(2){ |i| rand(max) - rand } }
    for i in 0...(ret.size)
      case i % 4
      when 1
        ret[i].map!{ |a| -a }
      when 2, 3
        ret[i].map!{ |a| (rand > 0.5 ? -a : a) }
      end
    end
    ret
  end

  MAX_NUMBER = 1000
  
  def self.string_arguments(number)
    prec = MPFR.get_default_prec
    max = 2 ** prec
    Array.new(number) do |i|
      sign1 = ((rand > 0.5 ? '-' : ''))
      sign2 = ((rand > 0.5 ? '-' : ''))
      ["#{sign1}#{rand(MAX_NUMBER)}.#{rand(max)}", "#{sign2}#{rand(MAX_NUMBER)}.#{rand(max)}"]
    end
  end
  

end
