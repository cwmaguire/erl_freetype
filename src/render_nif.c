/* Compile command:
 * clang -o my_nif.so -fPIC -v -undefined dynamic_lookup -dynamiclib -I /usr/local/Cellar/erlang/20.2.2/lib/erlang/usr/include/ -shared x.c my_nif.c
 *
 * I don't know what most of this does
 */
#include <erl_nif.h>
#include <unistd.h>

extern int* render_char(char *char_,
                        char *fontPath,
                        unsigned char *image);

static ERL_NIF_TERM render_char_nif(ErlNifEnv* env,
                                    int argc,
                                    const ERL_NIF_TERM argv[]) {
    // TODO figure out a way to calculate this for the
    // font we choose
    int pixelWidth = 110;
    int pixelHeight = 75;
    int pixelSize = pixelWidth * pixelHeight;

    // A binary that can be managed by Erlang
    ErlNifBinary bin = {.data = (unsigned char *) NULL};
    bin.size = sizeof(char) * pixelSize;
    /*printf("Initialized ErlNifBinary\r\n");*/

    // TODO make this an int
    char *char_ = NULL;
    char_ = (char*) malloc(sizeof(char) * 2);
    int result = enif_get_string(env, argv[0], char_, 2, ERL_NIF_LATIN1);
    if (!result) {
        printf("Failed to get char_: %d\r\n", result);
        return enif_make_badarg(env);
    }
    /*printf("Alloced char_\r\n");*/

    int *fontPathSize = NULL;
    fontPathSize = (int*) calloc(sizeof(int), 0);
    result = enif_get_int(env, argv[1], fontPathSize);
    if (!result) {
        printf("Failed to get font path size: %d\r\n", result);
        return enif_make_badarg(env);
    }
    /*printf("Font path string size is %d\r\n", *fontPathSize);*/

    char *fontPath = NULL;
    fontPath = (char*) malloc(sizeof(char) * (*fontPathSize + 1));
    result = enif_get_string(env, argv[2], fontPath, *fontPathSize + 1, ERL_NIF_LATIN1);
    if (!result) {
        printf("Failed to get font path: %d\r\n", result);
        return enif_make_badarg(env);
    }
    /*printf("Font path: >>%s<<\r\n", fontPath);*/

    // allocate a Erlang binary to hold alpha pixels
    size_t s = sizeof(char) * pixelSize;
    enif_alloc_binary(s, &bin);

    for(int i = 0; i < bin.size; i++){
      bin.data[i] = 0;
    }

    int* dimensions;
    dimensions = render_char(char_, fontPath, bin.data);

    /*printf("Got dimensions from rendered char\r\n");*/
    free(char_);
    /*printf("Freed char_\r\n");*/
    free(fontPathSize);
    /*printf("Freed fontPathSize\r\n");*/
    free(fontPath);
    /*printf("Freed fontPath\r\n");*/


    ERL_NIF_TERM width = enif_make_int(env, dimensions[0]);
    /*printf("Created erl term for width: %d\r\n", dimensions[0]);*/
    ERL_NIF_TERM height = enif_make_int(env, dimensions[1]);
    /*printf("Created erl term for height: %d\r\n", dimensions[1]);*/
    ERL_NIF_TERM top = enif_make_int(env, dimensions[2]);
    /*printf("Created erl term for top: %d\r\n", dimensions[2]);*/
    ERL_NIF_TERM bin_width = enif_make_int(env, pixelWidth);
    /*printf("Created erl term for bin_width: %d\r\n", pixelWidth);*/
    ERL_NIF_TERM pixel_data = enif_make_binary(env, &bin);
    /*printf("Got pixel data\r\n");*/
    ERL_NIF_TERM tuple = enif_make_tuple5(env,
                                          pixel_data,
                                          width,
                                          height,
                                          top,
                                          bin_width);
    /*printf("Created tuple\r\n");*/
    return tuple;
}

static ErlNifFunc nif_funcs[] = {
    {"render_char", 3, render_char_nif},
};

// Module, function array, load, NULL, upgrade, unload
ERL_NIF_INIT(render_char, nif_funcs, NULL, NULL, NULL, NULL)
