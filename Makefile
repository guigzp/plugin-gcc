GCCDIR = /home/guigzp/Computacao/Faculdade/Compiladores/trabalho4/gcc-install/bin
BENCHSDIR = $(shell pwd)/benchmarks

C = $(GCCDIR)/gcc
CC = $(GCCDIR)/g++

PLUGINDIR=$(shell $(C) -print-file-name=plugin)

plugin.so: plugin.o
	$(CC) -shared -o $@ $<

plugin.o : plugin.c
	$(CC) -fno-rtti -I$(PLUGINDIR)/include -fPIC -O0 -c -o $@ $<

clean:
	rm -f *.o plugin.so

execute: plugin.so 
	$(CC) -fplugin=./plugin.so -c $(BENCHSDIR)/ASC_Sequoia/CrystalMk/main.c -w > logs/crystal.log
	$(CC) -fplugin=./plugin.so -c $(BENCHSDIR)/Shootout/matrix/matrix.c  -w > logs/matrix.log
	$(C) -fplugin=./plugin.so -c $(BENCHSDIR)/BitBench/uuencode/uuencode.c -w > logs/uuencode.log
	$(C) -fplugin=./plugin.so -c $(BENCHSDIR)/Misc/flops/flops.c -w > logs/flops.log
	rm -f *.o plugin.so