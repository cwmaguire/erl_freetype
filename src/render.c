/* render.c
 *
 * Portions of this software are copyright © 2018 The FreeType Project
 * (www.freetype.org). All rights reserved.
 *
 * This comes from the FreeType2 example at
 * https://www.freetype.org/freetype2/docs/tutorial/example1.c
 *
 * I've modified it to be a shared object library for an Erlang
 * nif. It takes in some characters to render, the number of characters
 * and a char array to put the rendered bytes into.
 *
 */


#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ft2build.h>
#include FT_FREETYPE_H


#define WIDTH   640
#define HEIGHT  480


/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];

void copy_bitmap(FT_Bitmap *bitmap,
                 unsigned char *dst){
    for(int i = 0; i < HEIGHT * WIDTH; i++){
      dst[i] = bitmap->buffer[i];
    }
}

void render_chars(char *text, int num_chars, unsigned char *image){
  FT_Library    library;
  FT_Face       face;

  FT_GlyphSlot  slot;
  FT_Vector     pen;      /* untransformed origin  */
  FT_Error      error;

  char*         filename = "/Library/Fonts/Courier New.ttf";
  int           target_height;
  int           n;


  target_height = HEIGHT;

  error = FT_Init_FreeType(&library);              /* initialize library */
  /* error handling omitted */

  error = FT_New_Face(library, filename, 0, &face);
  printf("Error %d\r\n", error);
  /* error handling omitted */

  /* use 50pt at 100dpi */
  error = FT_Set_Char_Size(face, 50 * 64, 0, 100, 0);
  /* error handling omitted */

  printf("after FT_Set_Char_Size\r\n");

  /* cmap selection omitted;                                        */
  /* for simplicity we assume that the font contains a Unicode cmap */

  slot = face->glyph;

  /* the pen position in 26.6 cartesian space coordinates; */
  /* start at (300,200) relative to the upper left corner  */
  pen.x = 0 * 64;
  pen.y = (target_height - 200) * 64;

  for (n = 0; n < num_chars; n++){
    FT_Set_Transform(face, 0, &pen);

    /* load glyph image into the slot (erase previous one) */
    error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
    if (error)
      printf("FT_Load_Char error %d\r\n", error);
      /* ignore errors */
      continue;

    /* increment pen position */
    pen.x += slot->advance.x;
    pen.y += slot->advance.y;
  }

  // Copy the bitmap into the provided char array
  copy_bitmap(&slot->bitmap, image);

  FT_Done_Face    (face);
  FT_Done_FreeType(library);
}
