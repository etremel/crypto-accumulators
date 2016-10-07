TOPDIR=..

include $(TOPDIR)/rule.mk

BINS=bilinearspeedtest rsaspeedtest generate_random suffixtest flinttest #libtest libtest1 libdirecttest 
CFLAGS+=$(DCLXVI_INC) $(CRYPTOPP_INC) 
LIBS=$(ACCUMLIB_FLG) $(DCLXVI_LIB_FLG) $(CRYPTOPP_LIB_FLG) $(GMP_LIB_FLG) -lcrypto -lflint -lmpfr
all:	$(BINS)

generate_random: generate_random.o
	$(CPP) $(CFLAGS) -o generate_random generate_random.o $(LIBS)

flinttest: flinttest.o
	$(CPP) $(CFLAGS) -o flinttest flinttest.o $(LIBS)

bilinearspeedtest: bilinearspeedtest.o $(ACCUMLIB)
	$(CPP) $(CFLAGS) -o bilinearspeedtest bilinearspeedtest.o $(LIBS)

rsaspeedtest: rsaspeedtest.o $(ACCUMLIB)
	$(CPP) $(CFLAGS) -o rsaspeedtest rsaspeedtest.o $(LIBS)

suffixtest: suffixtest.o $(ACCUMLIB)
	$(CPP) $(CFLAGS) -o suffixtest suffixtest.o $(LIBS)

libtest: libtest.o $(ACCUMLIB)
	$(CPP) $(CFLAGS) -o libtest libtest.o $(LIBS)

libtest1: libtest1.o $(ACCUMLIB)
	$(CPP) $(CFLAGS) -o libtest1 libtest1.o $(LIBS)

libdirecttest: libdirecttest.o $(ACCUMLIB)
	$(CPP) $(CFLAGS) -o libdirecttest libdirecttest.o $(LIBS)

clean:
	rm -f *.o $(BINS)
