-module(render_char).
-on_load(on_load/0).
-export([render_char/4]).

on_load() ->
    NifPath = os:getenv("NIF_PATH", "./ebin/render_nif"),
    io:format("loading render_nif from ~p~n", [NifPath]),
    ok = erlang:load_nif(NifPath, 0).

%% This will be replaced when the NIF is loaded to
%% override this function
render_char(_, _, _, _) ->
    exit(nif_library_not_loaded).
