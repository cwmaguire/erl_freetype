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
#include <unistd.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define WIDTH   110
#define HEIGHT  75
#define SIZE    8250

// can't use HEIGHT * WIDTH for some reason
unsigned char image[SIZE];
/*unsigned char image[HEIGHT * WIDTH];*/

//From an example online
// https://www.freetype.org/freetype2/docs/tutorial/example1.c
//
//    This is the call in the example from within the loop that
//    renders the characters:
//
//    /* now, draw to our target surface (convert position) */
//    draw_bitmap( &slot->bitmap,
//                 slot->bitmap_left,
//                 target_height - slot->bitmap_top );
//
void draw_bitmap(FT_Bitmap*  bitmap,
                 unsigned char *image) {
  FT_Int  w = bitmap->width;
  FT_Int  h = bitmap->rows;
  FT_Int  index;

  for (int y = 0; y < h; y++){
    for (int x = 0; x < w; x++){
      // Clip off at edges
      if ( y < 0 || x < 0 || x >= WIDTH || y >= HEIGHT )
        continue;

      index = y * WIDTH + x;
      image[index] = bitmap->buffer[y * w + x];
    }

  }
}

int* render_char(char *text, unsigned char *image){
  FT_Library    library;
  FT_Face       face;
  FT_GlyphSlot  slot;
  /*FT_Vector     pen;      [> untransformed origin  <]*/
  FT_Error      error;

  /* Vim might be using menlo */
  char* filename = "/Library/Fonts/Courier New.ttf";
  static int error_dimensions[3] = {-1, -1, -1};

  /* initialize library */
  error = FT_Init_FreeType(&library);
  if(error){
    printf("Init FreeType error %d\r\n", error);
    return error_dimensions;
  }

  error = FT_New_Face(library, filename, 0, &face);
  if(error){
    printf("New face error %d\r\n", error);
    return error_dimensions;
  }
  /* error handling omitted */

  /* When I render an 'S' I'm getting a 30 pixel wide
   * letter.
   * DPI is pixels per inch
   * pt is how many points, where there are 72 points
   * per inch
   * 50pt / 72 points per inch = ~0.6944... inches
   * At 100 pixels per inch, that's 69.4 pixels
   *
   * Where is 30 pixels coming from?!
   * The font?
   */

  /* Set the char size in 26.6 fractional points */
  /* 26 bits for integer part, 6 bits for fraction part */
  /* The " * 64 " is to shift the specified point size left
   * so that it isn't a fraction (i.e. shift 6 places) */
  /* use 50pt at 100dpi */
  error = FT_Set_Char_Size(face, 50 * 64, 0, 100, 0);
  if (error){
    printf("FT_Set_Char_Size error %d\r\n", error);
    return error_dimensions;
  }

  /* cmap selection omitted;                                        */
  /* for simplicity we assume that the font contains a Unicode cmap */

  slot = face->glyph;

  // load glyph image into the slot (erase previous one)
  error = FT_Load_Char(face, text[0], FT_LOAD_RENDER);
  if (error){
    printf("FT_Load_Char error %d\r\n", error);
    return error_dimensions;
  }

  /*
  printf("pen.x = %lu\r\n", pen.x >> 6);
  printf("pen.y = %lu\r\n", pen.y >> 6);
  printf("slot->advance.x = %lu\r\n", slot->advance.x >> 6);
  printf("slot->advance.y = %lu\r\n", slot->advance.y >> 6);
  printf("slot->bitmap.width = %d\r\n", slot->bitmap.width);
  printf("slot->bitmap.rows = %d\r\n", slot->bitmap.rows);
  printf("slot->bitmap_left = %d\r\n", slot->bitmap_left);
  printf("slot->bitmap_top = %d\r\n", slot->bitmap_top);
  // See this for descriptions of what these fields are
  // https://www.freetype.org/freetype2/docs/tutorial/step2.html
  printf("slot->metrics.width = %lu\r\n", slot->metrics.width >> 6);
  printf("slot->metrics.height = %lu\r\n", slot->metrics.height >> 6);
  printf("slot->metrics.horiBearingX = %lu\r\n", slot->metrics.horiBearingX >> 6);
  printf("slot->metrics.horiBearingY = %lu\r\n", slot->metrics.horiBearingY >> 6);
  printf("slot->metrics.horiAdvance = %lu\r\n", slot->metrics.horiAdvance >> 6);
  printf("slot->metrics.vertBearingX = %lu\r\n", slot->metrics.vertBearingX >> 6);
  printf("slot->metrics.vertBearingY = %lu\r\n", slot->metrics.vertBearingY >> 6);
  printf("slot->metrics.vertAdvance = %lu\r\n", slot->metrics.vertAdvance >> 6);
  */

  // Copy the bitmap into the provided char array
  // slot->bitmap_top is how high the top of the bitmap is off the
  // baseline.
  draw_bitmap(&slot->bitmap, image);

  /*
  int val;
  printf("\r\n\r\n");
  usleep(2500);
  for(int y = 0; y < HEIGHT; y++){
      printf("%3d", y + 1);
      for(int x = 0; x < WIDTH; x++){
        val = (int)(image[y * WIDTH + x] / 2.6);
        if(val){
          printf("%2d", val);
        }else{
          printf("  ");
        }
      usleep(50);
    }
    printf("\r\n");
  }
  printf("\r\n");
  */

  static int dimension[3];
  dimension[0] = slot->bitmap.width;
  dimension[1] = slot->bitmap.rows;
  dimension[2] = slot->bitmap_top;
  return dimension;

  FT_Done_Face    (face);
  FT_Done_FreeType(library);
}
