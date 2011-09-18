
CXXC=g++
CXXFLAGS=-g -Wall

BUNDLE = ankh-distortion.lv2
INSTALL_DIR = /usr/local/lib/lv2

$(BUNDLE): manifest.ttl ankh-distortion.ttl ankh-distortion.so
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp manifest.ttl ankh-distortion.ttl ankh-distortion.so $(BUNDLE)

ankh-distortion.so: ankh-distortion.o
	$(CXXC) $(CXXFLAGS) -shared -fPIC -DPIC LV2Plugin.o ankh-distortion.o -o ankh-distortion.so

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
	rm -rf $(BUNDLE) ankh-distortion.so ankh-distortion.o LV2Plugin.o

pack:
	zip ankh-distortion.zip manifest.ttl ankh-distortion.ttl ankh-distortion.cpp LV2Plugin.cpp LV2Plugin.hpp LV2Extensions.hpp lv2_types.hpp LICENSE.txt Makefile distortion.cpp
