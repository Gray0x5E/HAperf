cd build
autoreconf --install
./configure --with-openssl=<openssl-path>
make
make install
haperf --help
