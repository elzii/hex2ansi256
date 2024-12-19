// Convert RGB24 to xterm-256 8-bit value
// For simplicity, assume RGB space is perceptually uniform.
// There are 5 places where one of two outputs needs to be chosen when the
// input is the exact middle:
// - The r/g/b channels and the gray value: the higher value output is chosen.
// - If the gray and color have same distance from the input - color is chosen.
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hex_to_rgb(const char *hex_str, uint8_t *r, uint8_t *g, uint8_t *b) {
  // Check if the hex string is valid
  if (hex_str[0] == '#' && strlen(hex_str) == 7) {
    // Convert the hex string to an integer
    unsigned int hex_value;
    sscanf(hex_str + 1, "%x", &hex_value);

    // Extract the red, green, and blue components
    *r = (hex_value >> 16) & 0xFF;
    *g = (hex_value >> 8) & 0xFF;
    *b = hex_value & 0xFF;
  } else {
    // Handle invalid hex string
    fprintf(stderr, "Invalid hex color string: %s\n", hex_str);
    *r = 0;
    *g = 0;
    *b = 0;
  }
}

static int rgb_to_x256(uint8_t r, uint8_t g, uint8_t b)
{
    // Calculate the nearest 0-based color index at 16 .. 231
#   define v2ci(v) (v < 48 ? 0 : v < 115 ? 1 : (v - 35) / 40)
    int ir = v2ci(r), ig = v2ci(g), ib = v2ci(b);   // 0..5 each
#   define color_index() (36 * ir + 6 * ig + ib)  /* 0..215, lazy evaluation */

    // Calculate the nearest 0-based gray index at 232 .. 255
    int average = (r + g + b) / 3;
    int gray_index = average > 238 ? 23 : (average - 3) / 10;  // 0..23

    // Calculate the represented colors back from the index
    static const int i2cv[6] = {0, 0x5f, 0x87, 0xaf, 0xd7, 0xff};
    int cr = i2cv[ir], cg = i2cv[ig], cb = i2cv[ib];  // r/g/b, 0..255 each
    int gv = 8 + 10 * gray_index;  // same value for r/g/b, 0..255

    // Return the one which is nearer to the original input rgb value
#   define dist_square(A,B,C, a,b,c) ((A-a)*(A-a) + (B-b)*(B-b) + (C-c)*(C-c))
    int color_err = dist_square(cr, cg, cb, r, g, b);
    int gray_err  = dist_square(gv, gv, gv, r, g, b);
    return color_err <= gray_err ? 16 + color_index() : 232 + gray_index;
}

int main(int argc, char **argv) {
  int i;
  uint8_t r, g, b;
  int c256;

  fprintf(stderr, "ARG COUNT                = %d\n", argc);
  /* printf("ARG COUNT = %d\n",argc); */
  
  if ( argc >= 2 ) {
    for( i=1; i<argc; i++ ) {
      hex_to_rgb(argv[i], &r, &g, &b);
      c256 = rgb_to_x256(r,g,b);
      fprintf(stderr, "HEX_TO_RGB[%s]      = %u,%u,%u\n",argv[i], r,g,b);
      fprintf(stderr, "RGB_TO_X256[%3u,%3u,%3u] = %d\n",r,g,b, c256);
      printf("256_TO_ANSI[%3d]         = \033[38;5;%im%s\033[0m", c256, c256, "FOOBAR");
    }
  }
  return 0;
}
