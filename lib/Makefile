
SUBDIRS=utils bilinear algorithms suffix flint

.PHONY:	asclib $(SUBDIRS)

all:
	make asclib

asclib: $(SUBDIRS)

$(SUBDIRS):
	make -C $@

clean:	
	rm -f *.a
	@for i in $(SUBDIRS); \
	do \
		(cd $$i ; make clean ;)\
	done;
