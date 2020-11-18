
SUBDIRS=utils bilinear algorithms suffix flint

.PHONY:	asclib $(SUBDIRS)

all:
	$(MAKE) asclib

asclib: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	rm -f *.a
	@for i in $(SUBDIRS); \
	do \
		(cd $$i ; $(MAKE) clean ;)\
	done;
