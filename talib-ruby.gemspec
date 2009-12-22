# -*- encoding: utf-8 -*-

Gem::Specification.new do |s|
  s.name = %q{talib-ruby}
  s.version = "0.0.2"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Valentin Treu"]
  s.date = %q{2009-12-22}
  s.description = %q{Ruby wrapper for the Technical Analysis Library ta-lib}
  s.email = ["rivella50@gmail.com"]
  s.extensions = ["ext/talib/extconf.rb"]
  s.extra_rdoc_files = ["History.txt", "Manifest.txt", "PostInstall.txt"]
  s.files = ["History.txt", "Manifest.txt", "PostInstall.txt", "README.rdoc", "Rakefile", "ext/talib/talib.bundle", "lib/talib_ruby.rb", "test/test_helper.rb", "test/test_talib_ruby.rb", "test/test_talib_extn.rb", "ext/talib/extconf.rb"]
  s.homepage = %q{http://github.com/#{github_username}/#{project_name}}
  s.post_install_message = %q{PostInstall.txt}
  s.rdoc_options = ["--main", "README.rdoc"]
  s.require_paths = ["lib", "ext/talib", "ext/talib", "ext/talib/extconf.rb", "ext/talib/include", "ext/talib/include/ta_abstract.h", "ext/talib/include/ta_common.h", "ext/talib/include/ta_defs.h", "ext/talib/include/ta_func.h", "ext/talib/include/ta_libc.h", "ext/talib/lib", "ext/talib/lib/libta_lib.0.0.0.dylib", "ext/talib/lib/libta_lib.0.dylib", "ext/talib/lib/libta_lib.a", "ext/talib/lib/libta_lib.dylib", "ext/talib/lib/libta_lib.la", "ext/talib/Makefile", "ext/talib/mkmf.log", "ext/talib/talib.bundle", "ext/talib/talib.c", "ext/talib/talib.o"]
  s.rubyforge_project = %q{talib-ruby}
  s.rubygems_version = %q{1.3.5}
  s.summary = %q{Ruby wrapper for ta-lib}
  s.test_files = ["test/test_helper.rb", "test/test_talib_extn.rb", "test/test_talib_ruby.rb"]

  if s.respond_to? :specification_version then
    current_version = Gem::Specification::CURRENT_SPECIFICATION_VERSION
    s.specification_version = 3

    if Gem::Version.new(Gem::RubyGemsVersion) >= Gem::Version.new('1.2.0') then
      s.add_development_dependency(%q<hoe>, [">= 2.4.0"])
    else
      s.add_dependency(%q<hoe>, [">= 2.4.0"])
    end
  else
    s.add_dependency(%q<hoe>, [">= 2.4.0"])
  end
end
