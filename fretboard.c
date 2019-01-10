#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define KGRY  "\x1B[38;2;64;64;64m"
#define KGRY2  "\x1B[38;2;128;128;128m"

#define KBLK  "\x1B[38;2;0;0;0m"
#define BGBLK "\x1B[48;2;0;0;0m"

#define KNRMFACE  "\x1B[22m"
#define KBOLD "\x1B[1m"

#define NUMSTRINGS 6
#define NUMFRETS 25

int string2frets[NUMSTRINGS] = {
  0, 5, 10, 15, 19, 24
};

// Frets on the bass E string corresponding to notes from 'A' to 'G'
int roots[] = {
  5, 7, 8, 10, 0, 1, 3
};

char *basefrets[12] = {
  "E", "F", "F#", "G", "G#", "A", "A#", "B", "C", "C#", "D", "D#"
};

char frets[(NUMFRETS+12*2)*4];

// natural minor scale, e.g., A, B, C, D, E, F, G
int in_scale_min(int f) {
  return f == 0 || f == 2 || f == 3 || f == 5 || f == 7 || f == 8 || f == 10;
}

// major scale, e.g., C, D, E, F, G, A, B
int in_scale_maj(int f) {
  return f == 0 || f == 2 || f == 4 || f == 5 || f == 7 || f == 9 || f == 11;
}

int main(int argc, const char *argv[]) {
  int scale_root = -1;
  int scale_maj = 1;
  int chord_root = -1;
  int chord_maj = 1;
  argc--;  ++argv;
  while (argc > 0) {
    if (strcmp(argv[0], "-h") == 0 || strcmp(argv[0], "--help") == 0) {
      printf("Usage: fretboard [-h|--help] [-c|--chord [A..G][#|b][m|M]] [-s|--scale [A..G][#|b][m|M]]\n");
      exit(0);
    } else if (strcmp(argv[0], "-c") == 0 || strcmp(argv[0], "--chord") == 0) {
      assert(argc > 1);
      assert(argv[1][0] >= 'A' && argv[1][0] <= 'G');
      chord_root = roots[argv[1][0] - 'A'];
      int i = 1;
      if (argv[1][i] == '#') {
        chord_root = (chord_root + 1) % 12;
        i++;
      } else if (argv[1][i] == 'b') {
        chord_root = (chord_root + 11) % 12;
        i++;
      }
      chord_maj = argv[1][i] == 'm' ? 0 : 1;
      argc--;  argv++;
    } else if (strcmp(argv[0], "-s") == 0 || strcmp(argv[0], "--scale") == 0) {
      assert(argc > 1);
      assert(argv[1][0] >= 'A' && argv[1][0] <= 'G');
      scale_root = roots[argv[1][0] - 'A'];
      int i = 1;
      if (argv[1][i] == '#') {
        scale_root = (scale_root + 1) % 12;
        i++;
      } else if (argv[1][i] == 'b') {
        scale_root = (scale_root + 11) % 12;
        i++;
      }
      scale_maj = argv[1][i] == 'm' ? 0 : 1;
      argc--;  argv++;
    } else {
      printf("Unknown option: %s\n", argv[0]);
      exit(1);
    }
    argc--;  ++argv;
  }
  printf(BGBLK "scale: %s%c, chord: %s%c\n", scale_root == -1 ? "-" : basefrets[scale_root], scale_maj ? ' ' : 'm', chord_root == -1 ? "-" : basefrets[chord_root], chord_maj ? ' ' : ' ');
  printf("%s", BGBLK);
  for (int i = 0; i < sizeof(frets)/sizeof(frets[0])/4; ++i)
    sprintf(frets + i*4, "%s%d", basefrets[i % 12], 2 + (4+i)/12);
  for (int s = NUMSTRINGS -1; s >=0; --s) {
    for (int f = 0; f < NUMFRETS; ++f) {
      int absfret = string2frets[s] + f;
      int scale_fret = (absfret + 12 - scale_root) % 12;
      char *col = KBLK;
      if (scale_root != -1)
        col = (scale_maj ? in_scale_maj(scale_fret) : in_scale_min(scale_fret)) ? (scale_fret == 0 ? KGRY2 : KGRY) : col;
      int chord_fret = (absfret + 12 - chord_root) % 12;
      if (chord_root != -1)
        col = chord_fret == 0 ? KWHT : (chord_fret == (chord_maj?4:3) || chord_fret == 7) ? KGRN : col;
      if (strcmp(col, KBLK) == 0)
        printf(KGRY "---");
      else
        printf("%s%3s" KGRN, col, &frets[absfret*4]);
      printf(KGRY "%c", f == 0 ? '|' : ' ');
    }
    printf("\n");
  }
  for (int f = 0; f < NUMFRETS; ++f)
    printf("---%c", f == 0 ? '+' : '-');
  printf("\n");
  for (int f = 0; f < NUMFRETS; ++f)
    if (f%12 == 5 || f%12 == 7 || f%12 == 0)
      printf("%s%3d ",  KWHT, f);
    else
      printf("%s%3d ", KGRY, f);
  printf("\n");
}
