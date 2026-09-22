all:
	make FSC_correlation_plotter
	make ZDC_plotter
	make FSC_ZDC_comparator

clean:
	-rm -f *.exe
	-rm -f *.o

FSC_correlation_plotter: FSC_correlation_plotter.cpp
	g++ FSC_correlation_plotter.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o FSC_correlation_plotter.exe

ZDC_plotter: ZDC_plotter.cpp
	g++ ZDC_plotter.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o ZDC_plotter.exe

FSC_ZDC_comparator: FSC_ZDC_comparator.cpp
	g++ FSC_ZDC_comparator.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o FSC_ZDC_comparator.exe
