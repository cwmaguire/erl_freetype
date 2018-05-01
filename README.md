# erl_freetype
An Erlang NIF that calls some modified FreeType2 sample code to render characters. The modified example code is in src/render.c.

I used the [freetype sample code](https://www.freetype.org/freetype2/docs/tutorial/example1.c) as a start and built an Erlang NIF around it.


Portions of this software are copyright © 2018 The FreeType Project (www.freetype.org).  All rights reserved.

## Running

    make render_nif
    erlc render.erl
    ./run
