require 'talib_ruby'

# init input data
a = Array.new
10.times { |i| a.push i.to_f }

10.times do |k|
	b = Array.new(10)
	l = TaLib::Function.new("MA")
	# setup input parameter
	l.in_real(0,a);
	# setup optional parameter
	l.opt_int(0,k+2);
	# setup output parameter
	l.out_real(0,b);
	p l.call(0,9)
	p "k=#{k+2}"
	p b
end
