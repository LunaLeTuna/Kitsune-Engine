CFLAGS = -std=c++20 -O2
LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -Wall
V8FLAGS = -I/home/spotwich/v8/ -I/home/spotwich/v8/include -lv8_monolith -L/home/spotwich/v8/out.gn/x64.release.sample/obj/ -DV8_COMPRESS_POINTERS

KitsuneEngine: main.cpp
	g++ $(CFLAGS) -o KitsuneEngine main.cpp stb_image.h -I/home/spotwich/v8/include $(LDFLAGS) $(V8FLAGS)

.PHONY: test clean

test: KitsuneEngine
	./KitsuneEngine

clean:
	rm -f KitsuneEngine
