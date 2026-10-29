all:
	make FSC_correlation_plotter
	make ZDC_plotter
	make FSC_ZDC_comparator
	make ZDC_test
	make FSC_selector
	make track_test
	make rho_analysis.cpp

clean:
	-rm -f *.exe
	-rm -f *.o

FSC_correlation_plotter: FSC_correlation_plotter.cpp
	g++ FSC_correlation_plotter.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o FSC_correlation_plotter.exe

ZDC_plotter: ZDC_plotter.cpp
	g++ ZDC_plotter.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o ZDC_plotter.exe

FSC_ZDC_comparator: FSC_ZDC_comparator.cpp rho_selector.cpp
	g++ FSC_ZDC_comparator.cpp rho_selector.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o FSC_ZDC_comparator.exe

ZDC_test: ZDC_test.cpp
	g++ ZDC_test.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o ZDC_test.exe

FSC_selector: FSC_selector.cpp
	g++ FSC_selector.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o FSC_selector.exe

track_test: track_test.cpp
	g++ track_test.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o track_test.exe

rho_analysis: rho_analysis.cpp rho_selector.cpp
	g++ rho_analysis.cpp rho_selector.cpp $$(root-config --glibs --cflags --libs) -L/home/jarno/libs -o rho_analysis.exe
