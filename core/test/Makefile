SRCDIR=src
DEPDIR=src/deps
BLDDIR=obj
ARDIR=../lib
BINDIR=bin
archive=libbpamiocore.a

csource := $(shell find $(SRCDIR) -type f -name *.c)
cobjs   := $(patsubst $(SRCDIR)/%,$(BLDDIR)/%,$(csource:.c=.o))
objects := $(cobjs)
cdeps   := $(patsubst $(SRCDIR)/%,$(DEPDIR)/%,$(csource:.c=.d))
pgms    := $(BINDIR)/basiccreate $(BINDIR)/basicread $(BINDIR)/basicrddir $(BINDIR)/basicalloc $(BINDIR)/basicenq $(BINDIR)/basicmem
all: $(pgms)

clean:
	rm -rf $(objects) $(pgms) $(DEPDIR)

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	mkdir -p $(DEPDIR) ; \
	rm -f $@; \
	clang -MM -MP $(CORE_CPPFLAGS) $< > $@;

include $(cdeps)

INCDIR=include

$(BINDIR)/basicrddir: $(BLDDIR)/basicrddir.o $(ARDIR)/$(archive)
	mkdir -p $(BINDIR) ;
	$(LD) $(LDFLAGS) -o $@ $^ >$@.llst

$(BINDIR)/basicread: $(BLDDIR)/basicread.o $(ARDIR)/$(archive)
	mkdir -p $(BINDIR) ;
	$(LD) $(LDFLAGS) -o $@ $^ >$@.llst

$(BINDIR)/basiccreate: $(BLDDIR)/basiccreate.o $(ARDIR)/$(archive)
	mkdir -p $(BINDIR) ;
	$(LD) $(LDFLAGS) -o $@ $^ >$@.llst

$(BINDIR)/basicenq: $(BLDDIR)/basicenq.o $(ARDIR)/$(archive)
	mkdir -p $(BINDIR) ;
	$(LD) $(LDFLAGS) -o $@ $^ >$@.llst

$(BINDIR)/basicmem: $(BLDDIR)/basicmem.o $(ARDIR)/$(archive)
	mkdir -p $(BINDIR) ;
	$(LD) $(LDFLAGS) -o $@ $^ >$@.llst

$(BINDIR)/basicalloc: $(BLDDIR)/basicalloc.o $(ARDIR)/$(archive)
	mkdir -p $(BINDIR) ;
	$(LD) $(LDFLAGS) -o $@ $^ >$@.llst

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BLDDIR) ;
	$(CC) $(CORE_CPPFLAGS) $(CFLAGS) -c -o $@ $<

.PHONY: all clean
