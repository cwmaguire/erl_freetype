render_nif: render.c render_nif.c
	gcc -Wall \
		-Werror \
		-include allheads.h \
		-o render_nif.so \
		-fPIC \
		-v \
		-I /home/lulu/dev/erlang_kerl/21.0/usr/include/ \
		-I /usr/local/include/freetype2 \
		-L/usr/local/lib \
		-lfreetype \
		-shared \
		render.c \
		render_nif.c
