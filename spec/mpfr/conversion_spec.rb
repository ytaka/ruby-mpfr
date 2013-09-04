require File.expand_path(File.join(File.dirname(__FILE__), "../spec_helper.rb"))

MPFR.set_default_prec(256)

describe MPFR do
  context "when converting to float" do
    it "should be transformed to float" do
      num = GenerateNumber.float(1000, 100) + [0.0]
      num.each do |a|
        b = MPFR.new(a).get_d(MPFR::RNDZ)
        a.should == b
      end
    end
  end

  context "when rounding" do
    it "should round a positive number." do
      a = MPFR('1.3')
      a.round.should == 1
    end

    it "should round a positive number to a bigger number." do
      a = MPFR('1.5')
      a.round.should == 2
    end

    it "should round a negative number." do
      a = MPFR('-1.3')
      a.round.should == -1
    end

    it "should round a negative number to a smaller number." do
      a = MPFR('-1.5')
      a.round.should == -2
    end

    it "should round a big number." do
      a = MPFR('1.3e+50')
      a.round.should == 1.3e+50
    end

    it "should raise MPFRDomainError." do
      lambda do
        MPFR.nan.round
      end.should raise_error MPFRDomainError
    end
  end

  context "when flooring" do
    it "should floor a positive number." do
      a = MPFR('1.3')
      a.floor.should == 1
    end

    it "should floor a positive number to a bigger number." do
      a = MPFR('1.5')
      a.floor.should == 1
    end

    it "should floor a negative number." do
      a = MPFR('-1.3')
      a.floor.should == -2
    end

    it "should floor a negative number to a smaller number." do
      a = MPFR('-1.5')
      a.floor.should == -2
    end

    it "should floor a big number." do
      a = MPFR('1.3e+50')
      a.floor.should == 1.3e+50
    end

    it "should raise MPFRDomainError." do
      lambda do
        MPFR.nan.floor
      end.should raise_error MPFRDomainError
    end
  end

  context "when ceiling" do
    it "should ceil a positive number." do
      a = MPFR('1.3')
      a.ceil.should == 2
    end

    it "should ceil a positive number to a bigger number." do
      a = MPFR('1.5')
      a.ceil.should == 2
    end

    it "should ceil a negative number." do
      a = MPFR('-1.3')
      a.ceil.should == -1
    end

    it "should ceil a negative number to a smaller number." do
      a = MPFR('-1.5')
      a.ceil.should == -1
    end

    it "should ceil a big number." do
      a = MPFR('1.3e+50')
      a.ceil.should == 1.3e+50
    end

    it "should raise MPFRDomainError." do
      lambda do
        MPFR.nan.ceil
      end.should raise_error MPFRDomainError
    end
  end

  context "when truncating" do
    it "should truncate a positive number." do
      a = MPFR('1.3')
      a.truncate.should == 1
    end

    it "should truncate a positive number to a bigger number." do
      a = MPFR('1.5')
      a.truncate.should == 1
    end

    it "should truncate a negative number." do
      a = MPFR('-1.3')
      a.truncate.should == -1
    end

    it "should truncate a negative number to a smaller number." do
      a = MPFR('-1.5')
      a.truncate.should == -1
    end

    it "should truncate." do
      a = MPFR('1.3e+50')
      a.truncate.should == 1.3e+50
    end

    it "should raise MPFRDomainError." do
      lambda do
        MPFR.nan.truncate
      end.should raise_error MPFRDomainError
    end
  end

  context "when converting by to_i" do
    it "should to_i a positive number." do
      a = MPFR('1.3')
      a.to_i.should == 1
    end

    it "should to_i a positive number to a bigger number." do
      a = MPFR('1.5')
      a.to_i.should == 1
    end

    it "should to_i a negative number." do
      a = MPFR('-1.3')
      a.to_i.should == -1
    end

    it "should to_i a negative number to a smaller number." do
      a = MPFR('-1.5')
      a.to_i.should == -1
    end

    it "should to_i." do
      a = MPFR('1.3e+50')
      a.to_i.should == 1.3e+50
    end

    it "should raise MPFRDomainError." do
      lambda do
        MPFR.nan.to_i
      end.should raise_error MPFRDomainError
    end
  end
end
