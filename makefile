all:
	make correlation_plotter
	make ZDC_plotter

clean:
	-rm -f *.exe
	-rm -f *.o

correlation_plotter: correlation_plotter.cpp
	g++ correlation_plotter.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o correlation_plotter.exe

ZDC_plotter: ZDC_plotter.cpp
	g++ ZDC_plotter.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o ZDC_plotter.exe
