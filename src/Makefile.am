ACLOCAL_AMFLAGS = -I m4
AM_LDFLAGS = "-fopenmp"
rambo_CPPFLAGS = -Wall -fopenmp -std=c++17 -O1 -pedantic -Wall -Wextra -Wimplicit-fallthrough=0 -mavx -march=native -lstdc++fs $(INCLUDE_FLAGS)
bin_PROGRAMS = rambo
rambo_SOURCES = ArrayOfBF.cpp  bitArray.cpp main.cpp  Makefile.am  MurmurHash3.cpp  MyBloom.cpp  Rambo_construction.cpp utils.cpp
bindir = $(abs_top_srcdir)/bin
#AC_PREFIX_DEFAULT(${bindir})
