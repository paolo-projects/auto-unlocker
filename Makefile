CXX	?= g++

src	= $(wildcard src/*.cpp)
obj	= $(src:.cpp=.o)

CXXFLAGS	= -Wall -std=c++17 -Iinclude

override LIBS	+= -lcurl -larchive -lpthread

auto-unlocker:	$(obj)
	echo $(INCLUDE)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	rm -f $(obj) auto-unlocker

depend: $(src)
	makedepend -- $(INCLUDE) $^
