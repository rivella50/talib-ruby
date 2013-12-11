#
# filename: test-fs_array.rb
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


#### endof filename: test-fs_array.rb
