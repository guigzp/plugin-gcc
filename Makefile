GCCDIR = /home/guigzp/Computacao/Faculdade/Compiladores/trabalho4/gcc-install/bin

C = $(GCCDIR)/g++

PLUGINDIR=$(shell $(C) -print-file-name=plugin)

plugin.so: plugin.o
	$(C) -shared -o $@ $<

plugin.o : plugin.cpp
	$(C) -fno-rtti -I$(PLUGINDIR)/include -fPIC -O0 -c -o $@ $<

clean:
	rm -f plugin.o plugin.so test.o

check: plugin.so test.c
	$(C) -fplugin=./plugin.so -c test.c 
