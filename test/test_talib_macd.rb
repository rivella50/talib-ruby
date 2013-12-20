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

    @testee_2 = TaLib::Function.new("MACDEXT")

    #@data   = [ 1.0, 2.0, 3.0, 4.0, 5.0 ]
    @data   = [ 1.0, 2.0, 3.0, 4.0, 5.0, 0.0, 0.0 ]
    @data_2 = [ 1.0, 2.0, 3.0, 4.0, 4.2,  1.0, 2.0, 3.0, 4.0, 4.2,  ]

    @opt_fast = 2
    @opt_slow = 3
    @opt_sig  = 1
    #@opt_matype = TaLib::TA_MAType_SMA  # 0.
    @opt_matype = TaLib::TA_MAType_EMA  # 1.

    @out_macd   = Array.new( 10 )
    @out_signal = Array.new( 10 )
    @out_hist   = Array.new( 10 )

  end

  def teardown

  end

  ####

  def test_config
    assert_equal( 1, @opt_matype )
  end

  def test_result
    assert_equal( [], @testee_2.result )
  end

  #
  #
  #
  def macdext_testitself( testobj )


    #
    testobj.in_real(0, @data)

    #
    testobj.opt_int(0, @opt_fast)
    testobj.opt_int(1, @opt_matype)
    testobj.opt_int(2, @opt_slow)
    testobj.opt_int(3, @opt_matype)
    testobj.opt_int(4, @opt_sig)
    testobj.opt_int(5, @opt_matype)

    #
    testobj.out_real(0, @out_macd)
    testobj.out_real(1, @out_signal)
    testobj.out_real(2, @out_hist)

    # [out_start(begidx), out_num(num_elems)] is returned.
    #
    ret = testobj.call(0,6)
    assert_equal( [2, 5], ret )

    #       0    1    2    3     4
    exp = [ 0.5, 0.5, 0.5, -0.5, -0.5 ]
    assert_equal( exp[0..(ret[1]-1)], @out_macd[0..(ret[1]-1)] )

    #
    assert_equal( @out_macd.size, testobj.result[0].size )
    assert_equal( testobj.result[0][0..(ret[1]-1)],
                  @out_macd[0..(ret[1]-1)] )
    assert_equal( exp[0..(ret[1]-1)],
                  testobj.result[0][0..(ret[1]-1)] )

  end

  def test_macdext_1
    macdext_testitself( @testee_2 )
  end

  # EMA has state so dont' do this.
  def test_repeat
    10.times do |i|
      print i
      #test_macdext_1
      macdext_testitself( TaLib::Function.new("MACDEXT") )
    end

  end

  #
  #
  #
  def test_macdext_2

    #
    @testee_2.in_real(0, @data_2)

    #
    @testee_2.opt_int(0, @opt_fast)
    @testee_2.opt_int(1, @opt_matype)
    @testee_2.opt_int(2, @opt_slow)
    @testee_2.opt_int(3, @opt_matype)
    @testee_2.opt_int(4, @opt_sig)
    @testee_2.opt_int(5, @opt_matype)

    #
    @testee_2.out_real(0, @out_macd)
    @testee_2.out_real(1, @out_signal)
    @testee_2.out_real(2, @out_hist)

    # [out_start(begidx), out_num(num_elems)] is returned.
    #
    ret = @testee_2.call(0,4)
    assert_equal( [2, 3], ret )

    #        0    1   2    3     4
    exp = [ 0.5, 0.5, 0.3666666666666667 ]
    #assert_equal( @testee.result[0][0..4], @out_macd[0..4] )
    assert_equal( @out_macd.size, @testee_2.result[0].size )

    # result[[], ..., []] is set for MACD, MACD_signal, MACD_hist.

    # outside of [ret[0], ret[1]] is undefined. we cannot test them.
    #
    assert_equal( @testee_2.result[0][0..(ret[1]-1)],
                  @out_macd[0..(ret[1]-1)] )
    assert_equal( exp[0..(ret[1]-1)],
                  @testee_2.result[0][0..(ret[1]-1)] )
    # -0.5 is also at result[0][5], but this value is not stable.
    # Sometimes, we get very strange value at result[0][5].
    #

    # Concering the behavior of MACDEXT(SMA),
    # the index of result (output) arrays always start from 0.
    # so valid output data is [ 0.0, 0.5, 0.5, 0.5 ]. And this time,
    # the number of output data is equals to ``ret[1]``, which means NBElem.
    #
    # ``ret[0]`` (BegIdx) has only meaning against input array.
    #
    assert_equal( exp[0..(ret[1]-1)], @testee_2.result[0][0..(ret[1]-1)] )

  end



end


#### endof filename: test_talib_macd.rb
