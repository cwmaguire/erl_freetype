-module(render_char).
-on_load(init/1).
-export([render_char/1]).

on_load(_) ->
    ok = erlang:load_nif("./render_nif", 0).

render_char(Char) ->
    render_nif:render(1, [Char]).

%% This will be replaced when the NIF is loaded to
%% override this function
render(_Char) ->
    exit(nif_library_not_loaded).
