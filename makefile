CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lGL -lX11 -lXrandr -Wl,--no-as-needed,-ldl -lpthread -lXi -Wall -lfreetype
FTFLAGS = -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-4
V8FLAGS = -I./liba/v8/ -I./liba/v8/include  -L./liba/v8/out.gn/x64.release.sample/obj/ -lv8_monolith -DV8_COMPRESS_POINTERS
BulletFLAGS = -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -I./liba/bullet3/src/

KitsuneEngine: main.cpp
	g++ $(CFLAGS) -o KitsuneEngine main.cpp stb_image.h ft2build.h $(LDFLAGS) $(FTFLAGS) $(V8FLAGS) $(BulletFLAGS)

.PHONY: test clean

test: KitsuneEngine
	./KitsuneEngine

clean:
	rm -f KitsuneEngine
