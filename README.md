# Cryptographic Accumulators
This project is a library providing an implementation of RSA Accumulators (as described by Baric and Pfitzmann) and Bilinear-Map Accumulators (as described by Lan Nguyen). A description of both types of accumulators, as well as some insight into my implementation decisions, can be found in [my undergraduate thesis](http://cs.brown.edu/research/pubs/theses/ugrad/2013/tremel.pdf).

## Dependencies
This project depends on a few C libraries that are packaged for most Linux distributions:
 * [FLINT](http://flintlib.org/) 2.5.2, which in turn depends on GMP >= 5.1.1 and MPFR >= 3.0
 * [Crypto++](http://cryptopp.com) 5.6.4, which is the version in the Ubuntu repositories

If you're running a recent Ubuntu system, these can all be installed automatically with

    sudo apt-get install libflint-dev libcrypto++-dev

It also depends on the [DCLXVI](https://cryptojedi.org/crypto/) library, for the elliptic-curve operations needed by the Bilinear Map Accumulator. This library is not in any package manager, however, and must be downloaded and installed manually from the author's website. For convenience, I've bundled it in the `ext` directory, and my Makefiles default to searching for DCLXVI in that directory instead of from the system-library directories.
