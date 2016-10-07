CC=cc -std=c99
CPP=g++
AR=ar
CFLAGS=
CFLAGS+=-g
#CFLAGS+=-O3
CFLAGS+=-Wall
CFLAGS+=-std=c++14
CFLAGS+=-pthread
CFLAGS+=-D_REENTRANT
CFLAGS+=-I$(TOPDIR)/inc

include $(TOPDIR)/ext/rule.mk

ACCUMLIB_DIR=$(TOPDIR)/lib
ACCUMLIB=$(ACCUMLIB_DIR)/libaccum.a
ACCUMLIB_FLG=-L$(ACCUMLIB_DIR) -laccum

#If crypto++ is not a system library, set this var to include the location of its headers
CRYPTOPP_INC=
CRYPTOPP_LIB_FLG=-lcryptopp
GMP_LIB_FLG=-lgmp

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CPP) $(CFLAGS) -c $<
