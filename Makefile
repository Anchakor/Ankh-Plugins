
CXXC=g++
CXXFLAGS=-g -Wall

BUNDLE = ankh-plugins.lv2
INSTALL_DIR = /usr/local/lib/lv2

$(BUNDLE): manifest.ttl ankh-3band-distortion.ttl ankh-3band-distortion.so ankh-distortion.ttl ankh-distortion.so
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp manifest.ttl ankh-3band-distortion.ttl ankh-3band-distortion.so ankh-distortion.ttl ankh-distortion.so $(BUNDLE)

ankh-3band-distortion.so: ankh-3band-distortion.o
	$(CXXC) $(CXXFLAGS) -shared -fPIC -DPIC LV2Plugin.o ankh-3band-distortion.o -o ankh-3band-distortion.so

ankh-distortion.so: ankh-distortion.o
	$(CXXC) $(CXXFLAGS) -shared -fPIC -DPIC LV2Plugin.o ankh-distortion.o -o ankh-distortion.so

ankh-3band-distortion.o: LV2Plugin.o
	$(CXXC) -c $(CXXFLAGS) ankh-3band-distortion.cpp -o ankh-3band-distortion.o

ankh-distortion.o: LV2Plugin.o
	$(CXXC) -c $(CXXFLAGS) ankh-distortion.cpp -o ankh-distortion.o

LV2Plugin.o:
	$(CXXC) -c $(CXXFLAGS) LV2Plugin.cpp -o LV2Plugin.o

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(INSTALL_DIR)

uninstall:
	rm -rf $(INSTALL_DIR)/$(BUNDLE)

clean:
	rm -rf $(BUNDLE) ankh-distortion.so ankh-distortion.o LV2Plugin.o ankh-3band-distortion.o ankh-3band-distortion.so

pack:
	zip ankh-distortion.zip manifest.ttl ankh-distortion.ttl ankh-distortion.cpp LV2Plugin.cpp LV2Plugin.hpp LV2Extensions.hpp lv2_types.hpp LICENSE.txt Makefile distortion.cpp biquad.h primitives.h ankh-3band-distortion.ttl ankh-3band-distortion.cpp
