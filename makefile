CFLAGS = -std=c++23 -O2
LDFLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -Wall -lfreetype
FTFLAGS = -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-4
V8FLAGS = -I/v8/ -I/v8/include  -L/v8/out.gn/x64.release.sample/obj/ -lv8_monolith -DV8_COMPRESS_POINTERS
BulletFLAGS = -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -I/usr/include/bullet

KitsuneEngine: main.cpp
	g++ $(CFLAGS) -o KitsuneEngine main.cpp stb_image.hpp ft2build.hpp $(LDFLAGS) $(FTFLAGS) $(V8FLAGS) $(BulletFLAGS)

.PHONY: test clean

test: KitsuneEngine
	./KitsuneEngine

clean:
	rm -f KitsuneEngine
