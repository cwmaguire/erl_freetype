/* Compile command:
 * clang -o my_nif.so -fPIC -v -undefined dynamic_lookup -dynamiclib -I /usr/local/Cellar/erlang/20.2.2/lib/erlang/usr/include/ -shared x.c my_nif.c
 *
 * I don't know what most of this does
 */
#include <erl_nif.h>
#include <unistd.h>

extern int* render_chars(char *chars,
                         int num_chars,
                         unsigned char *image);

// ERL_NIF_API_FUNC_DECL(int,
//                       enif_get_string,
//                       (ErlNifEnv*,
//                        ERL_NIF_TERM list,
//                        char* buf,
//                        unsigned len,
//                        ErlNifCharEncoding));
static ERL_NIF_TERM render_nif(ErlNifEnv* env,
                               int argc,
                               const ERL_NIF_TERM argv[]) {

    // TODO figure out a way to calculate this for the
    // font we choose
    int pixelWidth = 110;
    int pixelHeight = 75;
    int pixelSize = pixelWidth * pixelHeight;

    printf("render_nif.c - render_nif\r\n");
    /*unsigned char *bitmap;*/
    int num_chars = 0;

    printf("Declare ErlNifBinary *bin\r\n");
    // A binary that can be managed by Erlang
    ErlNifBinary bin = {.data = (unsigned char *) NULL};
    bin.size = sizeof(char) * pixelSize;

    printf("Declare char *chars\r\n");
    char *chars = NULL;

    if (!enif_get_int(env, argv[0], &num_chars)) {
        return enif_make_badarg(env);
    }
    printf("Got num_chars: %d\r\n", num_chars);

    chars = (char*) malloc(sizeof(char) * (num_chars + 1));
    int result = enif_get_string(env, argv[1], chars, num_chars + 1, ERL_NIF_LATIN1);
    printf("Result of enif_get_string: %d\r\n", result);

    if (!result) {
        printf("Failed to get chars\r\n");
        return enif_make_badarg(env);
    }
    printf("Got chars: %s\r\n", chars);

    // TODO Figure out if I need to keep creating a new
    // environment so I can keep creating new ERL_NIF_TERMS, copying
    // the old list to the new environment and clearing the old terms.
    // I don't know if, after I've done enif_make_X if I need to release
    // the object or if Erlang will GC it because it's an enif_make call.

    ErlNifEnv temp_env = enif_alloc_env();

    ERL_NIF_TERM rendered_chars = enif_make_list(env, 0);

    for(int i = 0; i < num_chars; i++){

        printf("Allocating Erlang binary\r\n");
        // allocate a Erlang binary to hold alpha pixels
        size_t s = sizeof(char) * pixelSize;
        enif_alloc_binary(s, &bin);
        printf("Allocated Erlang binary\r\n");

        printf("Clearing bin.data, size %d\r\n", (int) bin.size);
        for(int i = 0; i < bin.size; i++){
          bin.data[i] = 0;
        }
        printf("Cleared\r\n");

        printf("Rendering chars\r\n");
        int* vector;
        vector = render_chars(chars, num_chars, bin.data);
        free(chars);


        ERL_NIF_TERM width = enif_make_int(env, vector[0]);
        ERL_NIF_TERM height = enif_make_int(env, vector[1]);
        ERL_NIF_TERM pixel_data = enif_make_binary(env, &bin);
        ERL_NIF_TERM tuple = enif_make_tuple3(env,
                                              pixel_data,
                                              width,
                                              height);

        enif_make_list_cell(temp_env, tuple
    }

    // transfer ownership of bin to Erlang and return
    return tuple;
}

/*
typedef struct {
    const char* name;
    unsigned arity;
    // Function pointer
    ERL_NIF_TERM (*fptr)(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
    unsigned flags;
} ErlNifFunc;
*/
static ErlNifFunc nif_funcs[] = {
    {"render", 2, render_nif},
};

// Module, function array, load, NULL, upgrade, unload
ERL_NIF_INIT(render_nif, nif_funcs, NULL, NULL, NULL, NULL)
