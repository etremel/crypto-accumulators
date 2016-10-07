# Cryptographic Accumulators
This project is a library providing an implementation of RSA Accumulators (as described by Baric and Pfitzmann) and Bilinear-Map Accumulators (as described by Lan Nguyen). A description of both types of accumulators, as well as some insight into my implementation decisions, can be found in [my undergraduate thesis](http://cs.brown.edu/research/pubs/theses/ugrad/2013/tremel.pdf). 

## Dependencies
This project depends on [FLINT](http://flintlib.org/) 2.5.2, which in turn depends on GMP >= 5.1.1 and MPFR >= 3.0. If you're running a recent Ubuntu system, these can all be installed automatically with

    sudo apt-get install libflint-dev
    
It also depends on the [DCLXVI](https://cryptojedi.org/crypto/) library, for the elliptic-curve operations needed by the Bilinear Map Accumulator. Since this is not in any package manager, I've bundled it in the `ext` directory.
