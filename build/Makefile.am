bin_PROGRAMS = haperf
AUTOMAKE_OPTIONS = foreign subdir-objects

AM_CPPFLAGS = \
  -I$(top_srcdir)/../src \
  -I$(top_srcdir)/../src/http/http_server \
  -I$(top_srcdir)/../src/replay \
  $(OPENSSL_CFLAGS)

haperf_SOURCES = \
  $(top_srcdir)/../src/main.cpp \
  $(top_srcdir)/../src/functions.cpp \
  $(top_srcdir)/../src/cli_arguments.cpp \
  $(top_srcdir)/../src/http/http_server/http_server.cpp

haperf_CXXFLAGS = \
  -I$(top_srcdir)/../src \
  -I$(top_srcdir)/../src/http/http_server \
  $(OPENSSL_CFLAGS) \
  -std=c++11 \
  -pthread

haperf_LDADD = $(OPENSSL_LIBS) -lssl -lcrypto
