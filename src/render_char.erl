-module(render_char).
-on_load(on_load/0).
-export([render_char/1]).

on_load() ->
    ok = erlang:load_nif("./ebin/render_nif", 0).

%% This will be replaced when the NIF is loaded to
%% override this function
render_char(_) ->
    exit(nif_library_not_loaded).
