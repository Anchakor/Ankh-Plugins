
CXXC=g++
CXXFLAGS=-g -Wall

NAME = ankh-plugins

BUNDLE = $(NAME).lv2
INSTALL_DIR = /usr/local/lib/lv2

$(BUNDLE): *.so *.ttl
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp *.so *.ttl $(BUNDLE)

%.so: %.o LV2Plugin.o
	$(CXXC) $(CXXFLAGS) -shared -fPIC -DPIC $< -o $@

%.o : %.cpp
	$(CXXC) $(CFLAGS) -c $< -o $@

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(INSTALL_DIR)

uninstall:
	rm -rf $(INSTALL_DIR)/$(BUNDLE)

clean:
	rm -rf $(BUNDLE) *.so *.o

pack:
	zip $(NAME).zip *.ttl *.cpp *.hpp *.h LICENSE.txt Makefile
