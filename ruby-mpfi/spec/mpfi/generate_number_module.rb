module GenerateNumber

  def self.interval_float_arguments(number, max = 100)
    ret = Array.new(number){ |j| self.float(2).sort }
  end
  
  def self.float_arguments(size, number, max = 100)
    ret = Array.new(number){ |j| Array.new(size){ |i| rand(max) - rand } }
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

  def self.float(num, max = 100, &block)
    if block_given?
      for i in 0...num
        a = rand(max) - rand
        a = -a if rand > 0.5
        yield(a)
      end
    else
      ary = Array.new(num){ |i| rand(max) - rand }
      ary.map!{ |a| (rand > 0.5 ? -a : a) }
    end
  end

  def self.string(number)
    prec = MPFR.get_default_prec
    max = 2 ** prec
    Array.new(number) do |i|
      sign = ((rand > 0.5 ? '-' : ''))
      "#{sign}#{rand(max)}.#{rand(max)}"
    end
  end
  
  def self.mpfr_args(number)
    ret = self.string(number)
    ret.map!{ |a| MPFR.new(a) }
  end
  

end
