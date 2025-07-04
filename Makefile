CXX=g++
PKGCONFIG=pkg-config

CXXFLAGS+=-Wall -Wextra -Dcimg_use_jpeg=1 -Dcimg_use_png=1 -Dcimg_display=0 -Dcimg_plugin='"jpeg_buffer.h"' -g

CXXFLAGS_DOWNLOAD:= \
	$(shell $(PKGCONFIG) --cflags libcurl)

LIBS_DOWNLOAD:= \
	$(shell $(PKGCONFIG) --libs libcurl)

CXXFLAGS_LOCAL:= \
	$(shell $(PKGCONFIG) --cflags libjpeg) \
	$(shell $(PKGCONFIG) --cflags libpng)

LIBS_LOCAL:= \
	$(shell $(PKGCONFIG) --libs libjpeg) \
	$(shell $(PKGCONFIG) --libs libpng)

CXXFLAGS_DEFAULT:= \
	$(CXXFLAGS_DOWNLOAD) \
	$(CXXFLAGS_LOCAL)

LIBS_DEFAULT+= \
	$(LIBS_DOWNLOAD) \
	$(LIBS_LOCAL)

ifneq ($(strip $(ENABLE_SANITIZER)),)
CXXFLAGS+=-fsanitize=address -fsanitize=leak -fsanitize=undefined
endif

APP:=github-avatar-ukrainify
HEADERS_DOWNLOAD:=GithubAPI.hpp
SOURCES_DOWNLOAD:=GithubAPI.cpp
HEADERS_LOCAL:=ImageManipulation.hpp
SOURCES_LOCAL:=ImageManipulation.cpp
HEADERS_DEFAULT:=$(HEADERS_DOWNLOAD) $(HEADERS_LOCAL)
SOURCES_DEFAULT:=$(SOURCES_DOWNLOAD) $(SOURCES_LOCAL)

all: $(APP) $(APP)-download $(APP)-local

$(APP): $(HEADERS_DEFAULT) $(SOURCES_DEFAULT) main.cpp
	$(PKGCONFIG) --exist libcurl || false
	$(PKGCONFIG) --exist libjpeg || false
	$(PKGCONFIG) --exist libpng || false
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_DEFAULT) \
		$(LDFLAGS) $(SOURCES_DEFAULT) main.cpp -o $@ \
		$(LIBS_DEFAULT)

$(APP)-download: $(HEADERS_DOWNLOAD) $(SOURCES_DOWNLOAD) main.cpp
	$(PKGCONFIG) --exist libcurl || false
	$(CXX) $(CXXFLAGS) -DDOWNLOAD_ONLY=1 $(CXXFLAGS_DOWNLOAD) \
		$(LDFLAGS) $(SOURCES_DOWNLOAD) main.cpp -o $@ \
		$(LIBS_DOWNLOAD)

$(APP)-local: $(HEADERS_LOCAL) $(SOURCES_LOCAL) main.cpp
	$(PKGCONFIG) --exist libjpeg || false
	$(PKGCONFIG) --exist libpng || false
	$(CXX) $(CXXFLAGS) -DLOCAL_FILE=1 $(CXXFLAGS_LOCAL) \
		$(LDFLAGS) $(SOURCES_LOCAL) main.cpp -o $@ \
		$(LIBS_LOCAL)

clean:
	rm -f $(APP) $(APP)-download $(APP)-local
