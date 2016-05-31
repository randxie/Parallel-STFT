CXX := mpic++
ifneq ($(DEBUG_MODE),yes)
	CFLAGS := -O3
else
	CFLAGS := -g
endif
LDFLAGS := -L/usr/lib
LDLIBS := -lm -fopenmp -lpng
