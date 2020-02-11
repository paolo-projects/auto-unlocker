PREFIX = /usr

CXX	?= g++

src	= $(wildcard src/*.cpp)
obj	= $(src:.cpp=.o)

INCLUDE		= -Iinclude
CXXFLAGS	= -Wall -std=c++17 $(INCLUDE)

LIBS	+= -lcurl -larchive -lpthread -lstdc++fs

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
