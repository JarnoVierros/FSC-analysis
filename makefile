all:
	make root_reader

clean:
	-rm -f *.exe
	-rm -f *.o

root_reader: root_reader.cpp
	g++ root_reader.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o root_reader.exe
