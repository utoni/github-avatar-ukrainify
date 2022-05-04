CXX=g++
PKGCONFIG=pkg-config

CXXFLAGS+=-Wall -Wextra -Dcimg_use_jpeg=1 -Dcimg_display=0 -Dcimg_plugin='"jpeg_buffer.h"' -g $(shell $(PKGCONFIG) --cflags libcurl) $(shell $(PKGCONFIG) --cflags libjpeg)
LIBS+=$(shell $(PKGCONFIG) --libs libcurl) $(shell $(PKGCONFIG) --libs libjpeg)

ifneq ($(strip $(ENABLE_SANITIZER)),)
CXXFLAGS+=-fsanitize=address -fsanitize=leak -fsanitize=undefined
endif

APP:=github-avatar-ukrainify
HEADERS:=GithubAPI.hpp ImageManipulation.hpp
SOURCES:=GithubAPI.cpp ImageManipulation.cpp main.cpp

all: $(APP)

$(APP): $(HEADERS) $(SOURCES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCES) -o $@ $(LIBS)

clean:
	rm -f $(APP)
