include $(TOP)/build/header.mk

products_$(d) := unit perf

unit_sources_$(d) += \
    test.cpp

unit_precompiled_header_$(d) := 
unit_target_dir_$(d) := bin
unit_cxx_flags_$(d)  := -g -std=c++14 -I$(TOP)/include
unit_ld_flags_$(d)   := 

perf_sources_$(d) += \
    perf.cpp

perf_precompiled_header_$(d) := 
perf_target_dir_$(d) := bin
perf_cxx_flags_$(d)  := -O3 -std=c++14 -I$(TOP)/include -I/usr/local/include
perf_ld_flags_$(d)   := 

include $(TOP)/build/footer.mk
