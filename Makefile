SUBDIRS_CLEAN=lib test

SUBDIRS=ext $(SUBDIRS_CLEAN)

.PHONY: all $(SUBDIRS)

all: $(SUBDIRS)
	@echo Done

$(SUBDIRS):
	make -C $@

clean:
	@for i in $(SUBDIRS_CLEAN); \
	do \
		(cd $$i ; make clean ;)\
	done;
