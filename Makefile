SUBDIRS_CLEAN=lib test

SUBDIRS=ext $(SUBDIRS_CLEAN)

.PHONY: all $(SUBDIRS)

all: $(SUBDIRS)
	@echo Done

test: lib

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	@for i in $(SUBDIRS_CLEAN); \
	do \
		(cd $$i ; $(MAKE) clean ;)\
	done;
