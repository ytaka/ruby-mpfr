require 'mpfi/matrix.so'

class MPFI
  class Matrix
    def inspect
      tmp = str_ary_for_inspect2
      tmp.map!{ |a| "['" + a.join("', '") + "']"}
      sprintf("#<%s:%x, [%s]>", self.class, __id__, tmp.join(', '))
    end
    
    def pretty_print(pp)
      ary = str_ary_for_inspect2
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
          end
          pp.text("]")
        end
      end
    end
    
    def to_s(delimiter = ' ')
      str_ary_for_inspect.join(delimiter)
    end

    def self.create(a)
      case a
      when MPFI::Vector
        if self === a
          a.dup
        else
          self.new(a.to_a)
        end
      when MPFI::Matrix
        if a.column_size == a.row_size
          if MPFI::SquareMatrix === a
            a.dup
          else
            MPFI::SquareMatrix.new(a.to_a)
          end
        else
          a.dup
        end
      when Array
        if Array == a[0] && a.size == a[0].size
          MPFI::SquareMatrix.new(a)
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

    def self.interval(ary)
      if Array === ary && ary.all?{ |a| Array === a }
        row = ary.size
        column = ary[0].size
        if ary.all?{ |a| a.size == column }
          ret = self.new(row, column)
          (0...row).each do |i|
            (0...column).each do |j|
              case ary[i][j]
              when Array
                ret[i, j] = MPFI.interval(*ary[i][j])
              when String
                ret[i, j] = MPFI.interval(*(ary[i][j].split))
              when MPFI
                ret[i, j] = ary[i][j]
              else
                raise ArgumentError, "Invalid class for argument"
              end
            end
          end
        else
          ret = nil
        end
      end
      ret
    end
    
  end

  module Vector
    def inspect
      sprintf("#<%s:%x, ['%s']>", self.class, __id__, str_ary_for_inspect.join("', '"))
    end

    def pretty_print(pp)
      ary = str_ary_for_inspect
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

    def to_s(delimiter = ' ')
      str_ary_for_inspect.join(delimiter)
    end

    def subdivision_by_size(size)
      division = []
      each { |el| division << el.subdivision((el.diam_abs / size).ceil) }
      ary_size = division.inject(1) { |mul, a| mul * a.size }
      ary = Array.new(ary_size) { Array.new }
      num = ary_size
      division.each do |div|
        j = 0
        repeat = ary_size / num
        num = num / div.size
        repeat.times do
          div.size.times do |i|
            num.times do
              ary[j] << div[i]
              j += 1
            end
          end
        end
      end
      ary.map! { |a| self.class.new(a) }
    end
    
    def self.inner_product(a, b)
      a.inner_product(b)
    end

    def self.distance(a, b)
      a.distance_from(b)
    end

  end

  class ColumnVector
    def self.interval(ary)
      if Array === ary && ary.all?{ |a| Array === a }
        self.new(ary.map { |a| MPFI.interval(*a) })
      else
        nil
      end
    end
  end

  class RowVector
    def self.interval(ary)
      if Array === ary && ary.all?{ |a| Array === a }
        self.new(ary.map { |a| MPFI.interval(*a) })
      else
        nil
      end
    end
  end
  
end

