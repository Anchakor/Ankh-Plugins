# Copyright 2011 Jiří Procházka <ojirio@gmail.com>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 01222-1307  USA

###
NAMEPREFIX = ankh-
NAME = $(NAMEPREFIX)plugins
PLUGINS = distortion 3band-distortion 4band-compressor
OTHER = LV2Plugin SimpleEnvelope SimpleComp

HEADERS = *.hpp *.h
MISC = LICENSE.txt Makefile
###

CXXC=g++
CXXFLAGS=-g -Wall -O3

BDIR = build
SDIR = src

SRCS = $(addprefix $(NAMEPREFIX), $(addsuffix .cpp, $(PLUGINS)))
LIBS = $(addprefix $(NAMEPREFIX), $(addsuffix .so, $(PLUGINS)))
TTLS = manifest.ttl $(addprefix $(NAMEPREFIX), $(addsuffix .ttl, $(PLUGINS)))
COMPONENTS = $(addprefix $(BDIR)/, $(LIBS) $(TTLS))

OTHEROS = $(addprefix $(BDIR)/, $(addsuffix .o, $(OTHER)))
#OTHERSRCS = $(addprefix $(SDIR), $(addsuffix .cpp, $(OTHER)))

BUNDLE = $(NAME).lv2
INSTALL_DIR = /usr/lib/lv2

$(BUNDLE): $(BDIR) $(COMPONENTS)
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp $(BDIR)/*.so $(BDIR)/*.ttl $(BUNDLE)

$(BDIR):
	rm -rf $(BDIR)
	mkdir -p $(BDIR)

$(BDIR)/%.so: $(BDIR)/%.o $(OTHEROS)
	$(CXXC) $(CXXFLAGS) -shared -fPIC -DPIC $< $(OTHEROS) -o $@

$(BDIR)/%.o: $(SDIR)/%.cpp
	$(CXXC) $(CXXFLAGS) -c $< -o $@

$(BDIR)/%.ttl: $(SDIR)/%.ttl
	cp $< $@

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(DESTDIR)/$(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(DESTDIR)/$(INSTALL_DIR)

uninstall:
	rm -rf $(DESTDIR)/$(INSTALL_DIR)/$(BUNDLE)

clean:
	rm -rf $(BUNDLE) $(BDIR)

pack:
	zip $(NAME).zip $(MISC) $(addprefix $(SDIR)/, $(TTLS) $(SRCS) $(HEADERS))
