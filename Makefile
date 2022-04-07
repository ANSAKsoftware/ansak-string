SHELL=/bin/sh

###########################################################################
##
## Copyright (c) 2022, Arthur N. Klassen
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions are met:
##
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
## LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
## CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
## SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
## INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
## CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
## POSSIBILITY OF SUCH DAMAGE.
##
###########################################################################
##
## 2022.02.20 - First version
##
##    May you do good and not evil.
##    May you find forgiveness for yourself and forgive others.
##    May you share freely, never taking more than you give.
##
###########################################################################
##
## Makefile -- Implement a minimal Makefile to suport building, installing
##             uninstalling ansakString. Targets are
##             all (default), test, install, uninstall and a few other things.
##             For more, see
##
##             make help
##
###########################################################################

-include config.make
ifneq ($(strip $(CXX)),)
	CXXSELECT = CXX=$(CXX)
endif
ifneq ($(strip $(CONFIG)),)
	CONFIG_SELECT = "--config $(CONFIG)"
	C_SELECT = "-C $(CONFIG)"
	PRODUCT_DIR = build/$(CONFIG)
else
	PRODUCT_DIR = build
endif
-include build/deps

all: build/all.touch

build/all.touch: config.make $(SRCS)
	@cd build ; $(CXXSELECT) cmake ../ -DANSAK_UNICODE_SUPPORT=$(UNICODE_VERSION) -G "$(GENERATOR)"
	@cmake --build build $(CONFIG_SELECT) --target ansakString
	@touch build/all.touch


test: build/test.touch

build/test.touch: all config.make $(SRCS)
	@cmake --build build $(CONFIG_SELECT) --target ansakStringTest
	@cd build; ctest . "$(C_SELECT)"
	@touch build/test.touch


install: build/all.touch
	$(SUDO) mkdir -p $(PREFIX)/include/ansak
	$(SUDO) cp interface/*hxx $(PREFIX)/include/ansak
	$(SUDO) mkdir -p $(PREFIX)/lib
	$(SUDO) cp $(PRODUCT_DIR)/libansakString.a $(PREFIX)/lib


uninstall:
	$(SUDO) rm $(PREFIX)/include/ansak/string.hxx $(PREFIX)/include/ansak/string_trim.hxx $(PREFIX)/include/ansak/string_splitjoin.hxx
	$(SUDO) rm $(PREFIX)/lib/libansakString.a
	if test -d $(PREFIX)/include/ansak; then $(SUDO) rmdir $(PREFIX)/include/ansak; fi
	if [ "$(SUDO)" = "" ]; then \
		if test -d $(PREFIX)/include; then rmdir $$HOME/include; fi ;\
		if test -d $(PREFIX)/lib; then rmdir $$HOME/lib; fi ;\
	fi


package: test
	cd build ; make package


rpm: test
	@if [ $(BUILDS_RPM) -eq 0 ]; then \
		echo RPM building requires presence of the 'rpm' tool ;\
	else \
		cd build ;\
		cpack . -G rpm; \
	fi


deb: test
	@if [ $(BUILDS_DEB) -eq 0 ]; then \
		echo Debian package building requires presence of the 'dpkg-deb' tool ;\
	else \
		cd build ;\
		cpack . -G deb; \
	fi


arch: deb
	@if [ $(BUILDS_ARCH) -eq 0 ]; then \
		echo ZST package building requires presence of the 'debtap' tool ;\
	else \
		cd build ;\
		echo "Arthur N. Klassen" > arch-pkg-info ;\
		echo "BSD 2-clause License" >> arch-pkg-info ;\
		echo  >> arch-pkg-info ;\
		cpack . -G deb; \
		for pkg in *deb; do \
			debtap $$pkg < arch-pkg-info;\
		done ;\
	fi


cmake-install:
	@CMAKE_ROOT=`cmake --system-information | grep ^CMAKE_ROOT | awk '{print $$2;}' | tr -d '"'` export CMAKE_ROOT ;\
	$(SUDO) cp FindANSAK.cmake $$CMAKE_ROOT/Modules


cmake-uninstall:
	@CMAKE_ROOT=`cmake --system-information | grep ^CMAKE_ROOT | awk '{print $$2;}' | tr -d '"'` export CMAKE_ROOT ;\
	$(SUDO) rm -f $$CMAKE_ROOT/Modules/FindANSAK.cmake


clean:
	@cd build; \
	rm -rf *


scrub:
	rm -rf build 2>/dev/null
	rm -f config.make


help:
	@echo Makfile targets for ease-of-deployment on Unix-like systems: macOS, MinGW,
	@echo "  Cygwin, Linux etc.:"
	@echo "    * help: this message"
	@echo "    * all: (default target) compile of the libraries (Release)"
	@echo "    * test: compile and run of all tests"
	@echo "    * install: deploy headers and libraries compiled in \"all:\""
	@echo "    * uninstall: remove the headers and libraries installed in \"install:\""
	@echo "    *** PACKAGE TARGETS (package, rpm, deb, arch) UNAFFECTED BY PREFIX ****"
	@echo "    * package: build whatever package is native to the CMake-generated Makefile"
	@echo "    * rpm: build one or more RPM packages for this project"
	@echo "    * deb: build one or more Debian packages for this project"
	@echo "    * arch: build one or more Arch zst packages for this project"
	@echo "    * cmake-install: deploy FindANSAK.cmake to CMAKE_ROOT/modules"
	@echo "    * cmake-uninstall: remove FindANSAK.cmake from CMAKE_ROOT/modules"
	@echo "    * clean: empty out the build directory"
	@echo "    * scrub: remove the build directory and the config file."
	@echo "     "
	@echo Run ./configure before running make. There are some important settings to be
	@echo determined there.
