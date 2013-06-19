require 'mkmf'

dir_config("talib")

have_library("ta_lib", "TA_Initialize")
find_header("ta_abstract.h", "/usr/local/include/ta-lib")
create_makefile("talib")
