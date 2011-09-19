###
NAMEPREFIX = ankh-
NAME = $(NAMEPREFIX)plugins
PLUGINS = distortion 3band-distortion
HEADERS = *.hpp *.h
MISC = LICENSE.txt Makefile
###

CXXC=g++
CXXFLAGS=-g -Wall

BDIR = build
SDIR = src

SRCS = $(addprefix $(NAMEPREFIX), $(addsuffix .cpp, $(PLUGINS)))
LIBS = $(addprefix $(NAMEPREFIX), $(addsuffix .so, $(PLUGINS)))
TTLS = manifest.ttl $(addprefix $(NAMEPREFIX), $(addsuffix .ttl, $(PLUGINS)))
COMPONENTS = $(addprefix $(BDIR)/, $(LIBS) $(TTLS))

BUNDLE = $(NAME).lv2
INSTALL_DIR = /usr/local/lib/lv2

$(BUNDLE): $(BDIR) $(COMPONENTS)
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp $(BDIR)/*.so $(BDIR)/*.ttl $(BUNDLE)

$(BDIR):
	rm -rf $(BDIR)
	mkdir -p $(BDIR)

$(BDIR)/%.so: $(BDIR)/%.o $(BDIR)/LV2Plugin.o
	$(CXXC) $(CXXFLAGS) -shared -fPIC -DPIC $< -o $@

$(BDIR)/%.o: $(SDIR)/%.cpp
	$(CXXC) $(CFLAGS) -c $< -o $@

$(BDIR)/%.ttl: $(SDIR)/%.ttl
	cp $< $@

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(INSTALL_DIR)

uninstall:
	rm -rf $(INSTALL_DIR)/$(BUNDLE)

clean:
	rm -rf $(BUNDLE) $(BDIR)

pack:
	zip $(NAME).zip $(MISC) $(addprefix $(SDIR)/, $(TTLS) $(SRCS) $(HEADERS))
