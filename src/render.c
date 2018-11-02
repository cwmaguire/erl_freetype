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


/* origin is the upper left corner */
/*unsigned char image[HEIGHT][WIDTH];*/
int size = HEIGHT * WIDTH;

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
                 unsigned char *image,
                 FT_Int      pen_x,
                 FT_Int      bitmap_top) {
  FT_Int  i, j, p, q;

  // where we are in our binary
  FT_Int  x = pen_x >> 6;

  // translated from cartesian (y is up) to bitmap (y is down)
  FT_Int  y = bitmap_top - 25;
  // the size of the glyph bitmap
  FT_Int  w = bitmap->width;
  FT_Int  h = bitmap->rows;
  // Where we should end up in our binary
  FT_Int  x_max = x + w;
  FT_Int  y_max = y + h;

  FT_Int  index;

  // x and y come from our "pen" vector which we update
  // after every character to the start of the next
  // character.

  // Here we're filling in our image starting at the current
  // x and y coordinates from our pen but we're starting at
  // 0,0 when referencing the current bitmap
  // (In other words, it seems as if the bitmap translation does nothing)
  printf("Writing to image: (%d, %d) to (%d, %d)\r\n", x, y, x_max, y_max);
  for ( j = y, q = 0; j < y_max; j++, q++ )
  {
    for ( i = x, p = 0; i < x_max; i++, p++ )
    {
      // Clip off at edges
      if ( i < 0 || j < 0 || i >= WIDTH || j >= HEIGHT )
        continue;

      index = j * WIDTH + i;

      // Print out all the details of each pixel as we draw it.
      // Be prepared to scroll!
      /*int bitmap_value = bitmap->buffer[q * bitmap->width + p];*/
      //printf("image[%d * %d + %d = %d] |= bitmap->buffer[%2d * %2d + %2d]: %3d (%2d)",
      //       j, WIDTH, i,
      //       index,
      //       q,
      //       bitmap->width,
      //       p,
      //       bitmap_value,
      //       //bitmap->buffer[q * bitmap->width + p],
      //       image[index]);

      // The console was falling behind and screwing up
      // When there was too much printf output
      // usleep(2500);

      image[index] = bitmap->buffer[q * w + p];
    }

  }
}

int* render_chars(char *text, int num_chars, unsigned char *image){
  FT_Library    library;
  FT_Face       face;

  FT_GlyphSlot  slot;
  FT_Vector     pen;      /* untransformed origin  */
  FT_Error      error;

  /* Vim might be using menlo */
  char*         filename = "/Library/Fonts/Courier New.ttf";
  int           target_height;
  int           n;
  int           max_height = 0;

  target_height = HEIGHT;

  printf("\r\n\r\nSTARTING\r\n\r\n");

  /* initialize library */
  error = FT_Init_FreeType(&library);
  printf("Init FreeType error %d\r\n", error);
  /* error handling omitted */

  error = FT_New_Face(library, filename, 0, &face);
  printf("New face error %d\r\n", error);
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
  printf("Set char size error %d\r\n", error);
  /* error handling omitted */

  printf("after FT_Set_Char_Size\r\n");

  /* cmap selection omitted;                                        */
  /* for simplicity we assume that the font contains a Unicode cmap */

  slot = face->glyph;

  /* the pen position in 26.6 cartesian space coordinates; */
  // shift << 6 or multiply by 64 if these aren't 0
  pen.x = 0;
  pen.y = 0;

  printf("num_chars: %d\r\n", num_chars);
  for (n = 0; n < num_chars; n++){
    // transform only seems to change the metrics, not
    // where in memory the bitmap is
    FT_Set_Transform(face, 0, &pen);
    printf("after FT_Set_Transform\r\n");

    // load glyph image into the slot (erase previous one)
    error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
    printf("FT_Load_Char error %d\r\n", error);
    if (error){
      printf("FT_Load_Char error %d\r\n", error);
      /* ignore errors */
      continue;
    }
    printf("after FT_Load_Char\r\n");

    printf("Max height: %d\r\n", max_height);
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

    // Copy the bitmap into the provided char array
    // slot->bitmap_top is how high the top of the bitmap is off the
    // baseline.
    draw_bitmap(&slot->bitmap, image, pen.x, HEIGHT - slot->bitmap_top);

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
    usleep(2500);

    // Increment where we are in the target image
    pen.x += slot->advance.x;
    // We're using a horizontal font so this won't move
    // Unless we're doing multiple lines of text
    pen.y += slot->advance.y;

    if((slot->metrics.height >> 6) > max_height){
      printf("Updating max height from %d to %d\r\n",
             max_height, (int) slot->metrics.height >> 6);
      max_height = slot->metrics.height >> 6;
    }
  }

  static int dimension[2];
  dimension[0] = pen.x >> 6;
  dimension[1] = max_height;
  return dimension;

  FT_Done_Face    (face);
  FT_Done_FreeType(library);
}
