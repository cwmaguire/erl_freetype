-module(render_nif).

-export([render/2]).

-on_load(init/0).

init() ->
    io:format("Loading render_nif"),
    ok = erlang:load_nif("./render_nif", 0).

%% This will be replaced when the NIF is loaded to
%% override this function
render(_NumChars, _Chars) ->
    exit(nif_library_not_loaded).
