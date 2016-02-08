include build/core.mk

CC  := clang
CXX := clang++

$(eval $(call SUBDIR, test))

include build/targets.mk
