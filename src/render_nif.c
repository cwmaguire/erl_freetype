/* Compile command:
 * clang -o my_nif.so -fPIC -v -undefined dynamic_lookup -dynamiclib -I /usr/local/Cellar/erlang/20.2.2/lib/erlang/usr/include/ -shared x.c my_nif.c
 *
 * I don't know what most of this does
 */
#include <erl_nif.h>

extern void render_chars(char *chars,
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

    printf("render_nif.c - render_nif\r\n");
    /*unsigned char *bitmap;*/
    int num_chars = 0;

    printf("Declare ErlNifBinary *bin\r\n");
    // A binary that can be managed by Erlang
    ErlNifBinary bin = {.data = (unsigned char *) NULL};
    bin.data = malloc(sizeof(char) * 640 * 480);
    bin.size = sizeof(char) * 640 * 480;

    printf("Declare char *chars\r\n");
    char *chars = NULL;

    /*bitmap = (unsigned char*) malloc(sizeof(char) * 640 * 480);*/

    printf("Allocating Erlang binary\r\n");

    size_t s = sizeof(char) * 640 * 480;

    // allocate a binary
    /*enif_alloc_binary(sizeof(char) * 640 * 480, bin);*/
    enif_alloc_binary(s, &bin);

    printf("Allocated Erlang binary\r\n");

    if (!enif_get_int(env, argv[0], &num_chars)) {
        return enif_make_badarg(env);
    }
    printf("Got num_chars: %d\r\n", num_chars);

    // prints 2
    // int size = sizeof(char) * (num_chars + 1);
    // printf("Size of char array: %d\r\n", size);

    chars = (char*) malloc(sizeof(char) * (num_chars + 1));
    int result = enif_get_string(env, argv[1], chars, num_chars + 1, ERL_NIF_LATIN1);
    printf("Result of enif_get_string: %d\r\n", result);

    if (!result) {
        printf("Failed to get chars\r\n");
        return enif_make_badarg(env);
    }

    // works
    /*char* foo = "a";*/
    /*printf("Can print chars: %s\r\n", foo);*/

    printf("Got chars: %s\r\n", chars);

    printf("Rendering chars\r\n");
    render_chars(chars, num_chars, bin.data);
    free(chars);

    // transfer ownership of bin to Erlang and return
    return enif_make_binary(env, &bin);
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
