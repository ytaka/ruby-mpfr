require 'mpfr/matrix.so'

class MPFR
  class Matrix
    def str_ary2_2(format)
      ary = str_ary(format)
      tmp = []
      (0...row_size).each { tmp << [] }
      i = 0
      while i < size
        for j in 0...row_size
          tmp[j] << ary[i + j]
        end
        i += row_size
      end
      tmp
    end
    
    def str_ary3(format)
      ary = str_ary(format)
      tmp = []
      i = 0
      while i < size
        tmp << ary[i...(i + row_size)]
        i += row_size
      end
      tmp
    end
    
    def inspect
      tmp = str_ary2("%.Re")
      tmp.map!{ |a| "['" + a.join("', '") + "']" }
      sprintf("#<%s:%x, [%s]>", self.class, __id__, tmp.join(', '))
    end
    
    def pretty_print(pp)
      ary = str_ary2("%.Re")
      pp.object_group(self) do
        pp.text(sprintf(':%x, ', __id__))
        pp.breakable
        pp.text("[")
        pp.nest(1) do
          for j in 0...row_size
            pp.breakable if j > 0
            pp.text(%|["#{ary[j][0]}"|)
            pp.nest(1) do
              for i in 1...column_size
                pp.comma_breakable
                pp.text(%|"#{ary[j][i]}"|)
              end
            end
            pp.text("]")
            pp.comma_breakable if j < row_size - 1
          end
          pp.text("]")
        end
      end
    end
    
    def to_s(f = "%.Ff", delimiter = ' ')
      str_ary(f).join(delimiter)
    end

    def self.create(a)
      case a
      when MPFR::Vector
        if self === a
          a.dup
        else
          self.new(a.to_a)
        end
      when MPFR::Matrix
        if a.column_size == a.row_size
          if MPFR::SquareMatrix === a
            a.dup
          else
            MPFR::SquareMatrix.new(a.to_a)
          end
        else
          a.dup
        end
      when Array
        if Array == a[0] && a.size == a[0].size
          MPFR::SquareMatrix.new(a)
        else
          self.new(a)
        end
      else
        self.new(a)
      end

    end
    
    # ary is two-dimensional Array.
    def self.suitable_matrix_from_ary(ary)
      if ary.size == 1
        RowVector.new(ary[0])
      elsif ary[0].size == 1
        ColumnVector.new(ary.inject([]){ |res, val| res << val[0] })
      elsif ary[0].size == ary.size
        SquareMatrix.new(ary)
      else
        Matrix.new(ary)
      end
    end

  end

  module Vector
    def inspect
      sprintf("#<%s:%x, ['%s']>", self.class, __id__, str_ary("%.Re").join("', '"))
    end

    def pretty_print(pp)
      ary = str_ary("%.Re")
      pp.object_group(self) do
        pp.text(sprintf(':%x, ', __id__))
        pp.breakable
        pp.text(%|["#{ary[0]}"|)
        pp.nest(1) do
          for i in 1...dim
            pp.comma_breakable
            pp.text(%|"#{ary[i]}"|)
          end
        end
        pp.text("]")
      end
    end

    def to_s(format = "%.Re", delimiter = ' ')
      str_ary(format).join(delimiter)
    end

    def self.inner_product(a, b)
      a.inner_product(b)
    end

    def self.distance(a, b)
      a.distance_from(b)
    end

  end

end

