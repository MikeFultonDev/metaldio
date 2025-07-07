MACDIR=core/maclib
SRCDIR=core/src
BLDDIR=core/obj
DEPDIR=core/src/deps
ARDIR=core/lib
archive=libbpamiocore.a

csource := $(shell find $(SRCDIR) -type f -name *.c)
asource := $(shell find $(SRCDIR) -type f -name *.s)
cobjs   := $(patsubst $(SRCDIR)/%,$(BLDDIR)/%,$(csource:.c=.o))
aobjs   := $(patsubst $(SRCDIR)/%,$(BLDDIR)/%,$(asource:.s=.o))
objects := $(cobjs) $(aobjs)
cdeps   := $(patsubst $(SRCDIR)/%,$(DEPDIR)/%,$(csource:.c=.d))

all-core: $(ARDIR)/$(archive)

$(ARDIR)/$(archive): $(objects)
	mkdir -p $(ARDIR)
	ar -r $@ $^

clean:
	rm -rf $(BLDDIR) $(DEPDIR) $(ARDIR)

check:
	cd test; \
	gmake -f Makefile; \
	checkcore

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	mkdir -p $(DEPDIR) ; \
	rm -f $@; \
	clang -MM -MT $(BLDDIR)/$(*F).o $(CORE_CPPFLAGS) $< > $@;

include $(cdeps)

$(BLDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BLDDIR) ;
	$(CC) $(CORE_CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(BLDDIR)/%.o: $(SRCDIR)/%.s
	mkdir -p $(BLDDIR) ;
	$(AS) $(ASFLAGS) -I$(MACDIR) -o $@ $< >$(@D)/$*.lst

.PHONY: all clean check
