../ebin/render_nif.so: render.c render_nif.c
	clang -Wall \
		-Werror \
		-include allheads.h \
		-o ../ebin/render_nif.so \
		-fPIC \
		-v \
		-undefined dynamic_lookup \
		-dynamiclib \
		-I /usr/local/Cellar/erlang/20.2.2/lib/erlang/usr/include/ \
    -I /usr/local/Cellar/erlang/20.2.2/lib/erlang/usr/include/ \
    -I /usr/local/opt/freetype/include/freetype2 \
   	-L/usr/local/lib \
		-lfreetype \
		-shared \
		render.c \
		render_nif.c
