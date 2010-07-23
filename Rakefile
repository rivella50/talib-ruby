begin
  require 'jeweler'
  Jeweler::Tasks.new do |gemspec|
    gemspec.name = "talib_ruby"
    gemspec.summary = "Ruby Wrapper for ta-lib"
    gemspec.description = "Ruby Wrapper for the Technical Analysis Library ta-lib"
    gemspec.email = "rivella50@gmail.com"
    gemspec.homepage = "http://github.com/rivella50/talib-ruby"
    gemspec.authors = ["Valentin Treu"]
    gemspec.extensions << 'ext/talib/extconf.rb'
    gemspec.version = '1.0.0'
    gemspec.files = FileList['example/ma.rb','ext/talib/talib.c', 'README.rdoc','lib/**/*.rb']
    gemspec.test_files = []
  end
  Jeweler::GemcutterTasks.new
rescue LoadError
  puts "Jeweler not available. Install it with: sudo gem install jeweler"
end
