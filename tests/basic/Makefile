PKG_CONFIG=#`pkg-config --cflags --libs cairo`
CXXFLAGS=-O2 -std=c++17 -I../../include -L../../build -Wl,-rpath=../../build

default:
	$(CC) -c xdg-shell.c -o xdg-shell.o
	$(CXX) $(CXXFLAGS) main.cpp xdg-shell.o -lwayland-client -lvulkan $(PKG_CONFIG) -lprimer -lprime-vulkan

wayland:
	wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell.h
	wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell.c

shaders:
	glslc shader.vert -o vert.spv
	glslc shader.frag -o frag.spv

clean:
	rm xdg-shell.c xdg-shell.h
	rm *.o
	rm *.spv
	rm a.out

