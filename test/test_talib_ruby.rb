#
# filename: test_talib_ruby.rb
#
#

# Unit Test Lib.
require 'helper'
require 'test/unit'

# required lib.
#require 'something'

# Testee.
require "talib_ruby"


# Test Cases.
class TestTaLibRuby < Test::Unit::TestCase

  def setup
    @testee = TaLib::Function.new("MA")
  end

  def teardown

  end

  ####

  #
  def test_defined_constants
    ret = TaLib.constants
    exp = [
      :TA_Input_Price,
      :TA_Input_Real,
      :TA_Input_Integer,
        :TA_IN_PRICE_OPEN, :TA_IN_PRICE_HIGH,
        :TA_IN_PRICE_LOW, :TA_IN_PRICE_CLOSE,
        :TA_IN_PRICE_VOLUME,
        :TA_IN_PRICE_OPENINTEREST,
        :TA_IN_PRICE_TIMESTAMP,

      :TA_OptInput_RealRange,
      :TA_OptInput_RealList,
      :TA_OptInput_IntegerRange,
      :TA_OptInput_IntegerList,
        :TA_OPTIN_IS_PERCENT, :TA_OPTIN_IS_DEGREE,
        :TA_OPTIN_IS_CURRENCY, :TA_OPTIN_ADVANCED,

      :TA_Output_Real,
      :TA_Output_Integer,
        :TA_OUT_LINE,
        :TA_OUT_DOT_LINE,
        :TA_OUT_DASH_LINE,
        :TA_OUT_DOT,
        :TA_OUT_HISTO,
        :TA_OUT_PATTERN_BOOL,
        :TA_OUT_PATTERN_BULL_BEAR,
        :TA_OUT_PATTERN_STRENGTH,
        :TA_OUT_POSITIVE,
        :TA_OUT_NEGATIVE,
        :TA_OUT_ZERO,
        :TA_OUT_UPPER_LIMIT,
        :TA_OUT_LOWER_LIMIT,

      :TA_MAType_SMA,
        :TA_MAType_EMA,
        :TA_MAType_WMA,
        :TA_MAType_DEMA,
        :TA_MAType_TEMA,
        :TA_MAType_TRIMA,
        :TA_MAType_KAMA,
        :TA_MAType_MAMA,
        :TA_MAType_T3,
        :Function,
    ]
    assert_equal( exp, ret )

  end

  # test for Function.{groups,functions}, and Function#name.
  #
  #
  def test_new
    exp = [ "Math Operators",
            "Math Transform",
            "Overlap Studies",
            "Volatility Indicators",
            "Momentum Indicators",
            "Cycle Indicators",
            "Volume Indicators",
            "Pattern Recognition",
            "Statistic Functions",
            "Price Transform" ]
    assert_equal( exp.size, TaLib::Function.groups.size )
    assert_equal( exp, TaLib::Function.groups )

    #
    ret = TaLib::Function.functions
    exp.each do |grp|
      assert_equal( true, ret[grp].size > 0 )
    end

    #
    assert_equal( "MA", @testee.name )

  end


end


#### endof filename: test_talib_ruby.rb
