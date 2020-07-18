# Must init the autoconf setup
# The first parameter is project name
# second is version number
# third is bug report address
AC_INIT([rambo], [1.0])

INCLUDE_FLAGS="-I../include/ -I../ext/CLI11/include -I../ext/spdlog/include -I../ext/bit-algs/include -I../ext/bit-algs/ext/bit -I../ext/xsimd/include"
AC_SUBST(INCLUDE_FLAGS)

# Store the auxiliary build tools (e.g., install-sh, config.sub, config.guess)
AC_CONFIG_AUX_DIR([config])
LT_INIT
AC_CONFIG_MACRO_DIRS([m4])

# Safety checks in case user overwritten --srcdir
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_MAKE_SET

# Checks for libraries.

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([arpa/inet.h fcntl.h malloc.h netdb.h stdint.h stdlib.h string.h sys/socket.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([clock_gettime dup2 floor localtime_r memchr memmove memset mkdir pow rint select setenv socket sqrt strerror strrchr strstr])


# Init automake, and specify this program use relaxed structures.
# i.e. this program doesn't follow the gnu coding standards, and doesn't have
# ChangeLog, COPYING, AUTHORS, INSTALL, README etc. files.
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Check for C compiler
AC_PROG_CC
# We can add more checks in this section

# Tells automake to create a Makefile
# See https://www.gnu.org/software/automake/manual/html_node/Requirements.html
AC_CONFIG_FILES([Makefile src/Makefile])

# Generate the output
AC_OUTPUT
