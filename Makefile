PREFIX = /usr

CXX	?= g++

src	= src/versionparser.cpp \
	src/buildsparser.cpp \
	src/archive.cpp \
	src/network.cpp \
	src/debug.cpp \
	src/installinfo.cpp \
	src/patcher.cpp \
	src/tar.cpp \
	src/main.cpp \
	src/ziparchive.cpp \
	src/toolsdownloader.cpp \
	src/logging/combinedlogstrategy.cpp \
	src/logging/terminallogstrategy.cpp \
	src/logging/streamlogstrategy.cpp \
	src/logging/logstrategy.cpp \
	src/unlocker_lnx.cpp

obj	= $(src:.cpp=.o)

INCLUDE		= -Iinclude
CXXFLAGS	= -Wall -std=c++17 $(INCLUDE)

LIBS	+= -lcurl -lzip -lstdc++fs

auto-unlocker:	$(obj)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	rm -f $(obj) auto-unlocker

depend: $(src)
	makedepend -- $(INCLUDE) $^

.PHONY: install
install: auto-unlocker
	install -D $< $(DESTDIR)$(PREFIX)/bin/auto-unlocker
#	mkdir -p $(DESTDIR)$(PREFIX)/bin
#	cp $< $(DESTDIR)$(PREFIX)/bin/auto-unlocker

.PHONY: uninstall
uninstall: auto-unlocker
	-rm -f $(DESTDIR)$(PREFIX)/bin/auto-unlocker
