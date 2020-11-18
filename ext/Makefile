TOPDIR=..

include rule.mk

LIBS=$(DCLXVI_LIB)

all: $(LIBS)

$(DCLXVI_LIB): $(DCLXVI_DIR)
	cd $(DCLXVI_DIR); $(MAKE)
$(DCLXVI_DIR):
	tar xvjf $(DCLXVI_PKG); \
	cp -f dclxvi-patch/Makefile dclxvi-patch/*.c $(DCLXVI_DIR)

