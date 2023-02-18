#!/bin/bash
rm -f .DS_Store
rm -f aclocal.m4
rm -rf autom4te.cache/
rm -f compile
rm -f config.h
rm -f config.h.in
rm -f config.status
rm -f configure
rm -f configure~
rm -f depcomp
rm -f install-sh
rm -f main.cpp
rm -f Makefile
rm -f missing
rm -rf src/.deps/
rm -rf src/.dirstamp
rm -rf src/http/HttpServer/
rm -f Makefile.in
rm -f config.log
rm -rf src/http/http_server/.deps/
rm -rf src/http/http_server/.dirstamp
rm -f stamp-h1
find . -name .DS_Store -type f -delete
