include $(TOP)/build/header.mk

products_$(d) := unit

unit_sources_$(d) += \
    test.cpp

unit_precompiled_header_$(d) := precompiled.hpp

unit_target_dir_$(d) := bin
unit_cxx_flags_$(d)  := -g -std=c++14 -I$(TOP)/include
unit_ld_flags_$(d)   :=

include $(TOP)/build/footer.mk
