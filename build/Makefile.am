bin_PROGRAMS = haperf
AUTOMAKE_OPTIONS = foreign subdir-objects

#AM_CPPFLAGS = -I$(top_srcdir)/config.h

haperf_SOURCES = \
  $(top_srcdir)/../src/main.cpp \
  $(top_srcdir)/../src/functions.cpp \
  $(top_srcdir)/../src/cli_arguments.cpp \
  $(top_srcdir)/../src/http/server/server.cpp \
  $(top_srcdir)/../src/http/server/server_ssl.cpp

haperf_CXXFLAGS = \
  -I$(top_srcdir)/../src \
  -I$(top_srcdir)/../src/http/server \
  $(OPENSSL_CFLAGS) \
  -std=c++11 \
  -pthread

haperf_LDADD = $(OPENSSL_LIBS)
