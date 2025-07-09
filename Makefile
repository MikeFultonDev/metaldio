# Top-level Makefile for the metaldio project

# Default installation prefix.
# This can be overridden from the command line
PREFIX?=/usr/local

CORE_MAKEFILE     = core/Makefile
SERVICES_MAKEFILE = services/Makefile
SAMPLE_MAKEFILE   = sample/Makefile

# Install files
CORE_LIB          = core/lib/libbpamiocore.a
SERVICES_LIB      = services/lib/libbpamiosvcs.a

.PHONY: all clean install

all:
	@echo "--- Building core library ---"
	@$(MAKE) -f $(CORE_MAKEFILE)
	@echo "--- Building services library ---"
	@$(MAKE) -f $(SERVICES_MAKEFILE)
	@echo "--- Building samples ---"
	@$(MAKE) -f $(SAMPLE_MAKEFILE)

install: all
	@echo "--- Installing libraries, headers, and binaries to $(PREFIX) ---"
	@mkdir -p $(PREFIX)/lib
	@mkdir -p $(PREFIX)/include/metaldio
	@mkdir -p $(PREFIX)/bin
	# Copy the compiled library archives from their known locations.
	cp $(CORE_LIB) $(PREFIX)/lib/
	cp $(SERVICES_LIB) $(PREFIX)/lib/
	# Copy public header files into the 'metaldio' subdirectory.
	cp services/include/*.h $(PREFIX)/include/metaldio/
	# Copy the sample binaries.
	@echo "--- Installation complete ---"
	@echo "Libraries installed in $(PREFIX)/lib"
	@echo "Headers installed in $(PREFIX)/include/metaldio"
	@echo "Binaries installed in $(PREFIX)/bin"

clean:
	@echo "--- Cleaning core ---"
	@$(MAKE) -f $(CORE_MAKEFILE) clean
	@echo "--- Cleaning services ---"
	@$(MAKE) -f $(SERVICES_MAKEFILE) clean
	@echo "--- Cleaning sample ---"
	@$(MAKE) -f $(SAMPLE_MAKEFILE) clean
	@echo "--- Clean complete ---"
