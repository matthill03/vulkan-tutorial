CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

vulkan-tutorial: main.cpp
	g++ $(CFLAGS) -o vulkan-tutorial main.cpp $(LDFLAGS)

.PHONY: test clean

test: vulkan-tutorial
	./vulkan-tutorial

clean:
	rm -f vulkan-tutorial
