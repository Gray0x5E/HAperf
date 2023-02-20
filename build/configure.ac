AC_PREREQ([2.69])
AC_INIT([HAperf], [1.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([../src/main.cpp])
AC_CONFIG_HEADERS([config.h])

# Check for OpenSSL and set its location
AC_ARG_WITH([openssl],
	AS_HELP_STRING([--with-openssl=DIR], [Specify location of OpenSSL installation]),
	[
		with_openssl="$withval"
		OPENSSL_CFLAGS="-I$with_openssl/include"
		OPENSSL_LIBS="-L$with_openssl/lib -lssl -lcrypto"
		USE_SSL="yes"
	],
	[
		AC_MSG_NOTICE([Searching for OpenSSL in standard locations])
		AC_CHECK_LIB(
			[ssl], [SSL_new],
			[
				with_openssl="/usr"
				OPENSSL_CFLAGS="-I$with_openssl/include"
				OPENSSL_LIBS="-L$with_openssl/lib -lssl -lcrypto"
				AC_MSG_NOTICE([OpenSSL found in standard location])
				USE_SSL="yes"
			],
			[
				AC_MSG_NOTICE([OpenSSL not found in standard locations. For HTTPS support configure using --with-openssl=DIR])
				USE_SSL="no"
			]
		)
	]
)

AC_SUBST([OPENSSL_CFLAGS])
AC_SUBST([OPENSSL_LIBS])

# Check for C++
AC_PROG_CXX

# Set environment variable if SSL support was not found
if test "x$USE_SSL" = "xyes"; then
    AC_DEFINE([SSL_SUPPORT], [1], [OpenSSL found; SSL support is available])
    AC_MSG_NOTICE([SSL_SUPPORT=1])
elif test "x$USE_SSL" = "xno"; then
    AC_DEFINE([SSL_SUPPORT], [0], [OpenSSL not found; no SSL support])
    AC_MSG_NOTICE([SSL_SUPPORT=0])
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
