ACLOCAL_AMFLAGS = -I m4
AM_LDFLAGS = -fopenmp -lstdc++fs
rambo_CPPFLAGS = -Wall -fopenmp -std=c++17 -O3 -pedantic -Wall -Wextra -Wimplicit-fallthrough=0 -mavx -march=native -lstdc++fs $(INCLUDE_FLAGS)
bin_PROGRAMS = rambo
rambo_SOURCES =  bitArray.cpp main.cpp MyBloom.cpp  Rambo_construction.cpp utils.cpp
bindir = $(abs_top_srcdir)/bin
#AC_PREFIX_DEFAULT(${bindir})
