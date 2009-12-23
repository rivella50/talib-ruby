$:.unshift(File.dirname(__FILE__)) unless
  $:.include?(File.dirname(__FILE__)) || $:.include?(File.expand_path(File.dirname(__FILE__)))

#require File.dirname(__FILE__) + '/../ext/talib/talib'
require 'talib'

module TalibRuby
  VERSION = '0.0.4'
end