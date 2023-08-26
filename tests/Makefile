PKG_CONFIG=`pkg-config --cflags --libs cairo`
default:
	wayland-scanner client-header /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell.h
	wayland-scanner private-code /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml xdg-shell.c
	$(CC) -c xdg-shell.c -o xdg-shell.o
	$(CXX) -I../include main.cpp xdg-shell.o -lwayland-client -lwayland-egl -lEGL -lGLESv2 -lvulkan $(PKG_CONFIG) -lprimer -L../build -lprime-vulkan -Wl,-rpath=../build

shaders:
	glslc shader.vert -o vert.spv
	glslc shader.frag -o frag.spv

