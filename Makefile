CXX	?= g++

src	= $(wildcard src/*.cpp)
obj	= $(src:.cpp=.o)

INCLUDE		= -Iinclude
CXXFLAGS	= -Wall -std=c++17 $(INCLUDE)

override LIBS	+= -lcurl -larchive -lpthread

auto-unlocker:	$(obj)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	rm -f $(obj) auto-unlocker

depend: $(src)
	makedepend -- $(INCLUDE) $^
