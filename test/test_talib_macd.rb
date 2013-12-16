#
# filename: test_talib_macd.rb
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
class TestTaLibMACD < Test::Unit::TestCase

  def setup
    @testee = TaLib::Function.new("MACD")

    @data = [ 1.0, 2.0, 3.0, 4.0, 5.0 ]
    @opt_fast = 2
    @opt_slow = 3
    @opt_sig  = 1

    @out_macd   = Array.new( 10 )
    @out_signal = Array.new( 10 )
    @out_hist   = Array.new( 10 )

  end

  def teardown

  end

  ####

  def test_result
    assert_equal( [], @testee.result )
  end

  #
  #
  #
  def test_macd_result

    #
    @testee.in_real(0, @data)

    #
    @testee.opt_int(0, @opt_fast)
    @testee.opt_int(1, @opt_slow)
    @testee.opt_int(2, @opt_sig)

    #
    @testee.out_real(0, @out_macd)
    @testee.out_real(1, @out_signal)
    @testee.out_real(2, @out_hist)

    # [out_start(begidx), out_num(num_elems)] is returned.
    #
    ret = @testee.call(0,6)
    assert_equal( [1, 4], ret )

    #        0    1   2    3     4
    exp = [ 0.0, 0.5, 0.5, 0.5, -0.5 ]
    #assert_equal( @testee.result[0][0..4], @out_macd[0..4] )
    assert_equal( @out_macd.size, @testee.result[0].size )

    # result[[], ..., []] is set.
    # outside of [ret[0], ret[1]] is undefined. we cannot test them.
    #
    assert_equal( @testee.result[0][ret[0]..ret[1]],
                  @out_macd[ret[0]..ret[1]] )
    assert_equal( exp[1..4], @testee.result[0][ret[0]..ret[1]] )

  end

  # test for out_start and out_num.
  #
  #
  def test_repeat_result
    10.times do |i|
      @out_macd   = Array.new( 10 )
      @out_signal = Array.new( 10 )
      @out_hist   = Array.new( 10 )

      puts "i: #{i+1}"
      self.test_macd_result
    end
  end



end


#### endof filename: test_talib_macd.rb
