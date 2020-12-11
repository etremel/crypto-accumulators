
SUBDIRS=utils bilinear algorithms suffix flint

.PHONY:	accumlib $(SUBDIRS)

all:
	$(MAKE) accumlib

accumlib: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	rm -f *.a
	@for i in $(SUBDIRS); \
	do \
		(cd $$i ; $(MAKE) clean ;)\
	done;
