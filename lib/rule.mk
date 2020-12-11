include $(TOPDIR)/rule.mk

CFLAGS+=$(DCLXVI_INC)

LIBRARY=$(ACCUMLIB)

$(LIBRARY): $(OBJS)
	$(AR) -r $(LIBRARY) $(OBJS)
	@sleep 1 ## avoid wrong lib timestamp

clean:
	rm -f *.o
