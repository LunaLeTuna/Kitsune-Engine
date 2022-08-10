CFLAGS = -std=c++20 -O2
LDFLAGS = -lglfw -lGL -lX11 -lXrandr -Wl,--no-as-needed -ldl -lpthread -Wall -lfreetype
FTFLAGS = -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-4
V8FLAGS = -I/home/luna/v8/ -I/home/luna/v8/include  -L/home/luna/v8/out.gn/arm64.release.sample/obj/ -lv8_monolith -DV8_COMPRESS_POINTERS
BulletFLAGS = -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -I/usr/include/bullet

KitsuneEngine: main.cpp
	g++ $(CFLAGS) -o KitsuneEngine main.cpp stb_image.h ft2build.h $(LDFLAGS) $(FTFLAGS) $(V8FLAGS) $(BulletFLAGS)

.PHONY: test clean

test: KitsuneEngine
	./KitsuneEngine

clean:
	rm -f KitsuneEngine
