require 'rubygems'
require 'talib_ruby'

# init input data
a = Array.new
10.times { |i| a.push i.to_f }

h = (7..17).inject([]) { |list, i| list << i }
l = (3..15).inject([]) { |list, i| list << i }
c = (6..16).inject([]) { |list, i| list << i }

5.times do |k|

  b = Array.new(10)
  ta = TaLib::Function.new("ADX")
  # setup input price
  # open = nil, volume = nil, open_interest = nil
  ta.in_price(0, nil, h, l, c, nil, nil);
	# setup optional parameter
	ta.opt_int(0,k+2);
  # setup output parameter
  ta.out_real(0,b);
  ta.call(0,9)
	p "k=#{k+2}"
  p b
end
