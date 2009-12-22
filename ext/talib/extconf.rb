require 'mkmf'

dir_config("talib")
have_library("ta_lib", "TA_Initialize")
create_makefile("talib")
