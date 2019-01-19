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

// Fret names 
typedef enum {
  FRET_E = 0, FRET_F, FRET_Fd, FRET_G, FRET_Gd, FRET_A, FRET_Ad, FRET_B, FRET_C, FRET_Cd, FRET_D, FRED_Dd
} Fret_E;

int string2frets[NUMSTRINGS] = {
  0, 5, 10, 15, 19, 24
};

// Frets on the bass E string corresponding to notes from 'A' to 'G'
int roots[] = {
  FRET_A, FRET_B, FRET_C, FRET_D, FRET_E, FRET_F, FRET_G
};

// Useful unicode chars: 𝄫, ♭, 𝄪

// Characters per chord, e.g., "G#m" => 4 chars '\x0' included
#define CHORDSIZE 8

// sharp-altered frets
char basefrets_d[12][CHORDSIZE] = {
  "E", "F", "F#", "G", "G#", "A", "A#", "B", "C", "C#", "D", "D#"
};

// flat-altered frets
char basefrets_b[12][CHORDSIZE]= {
  "E", "F", "Gb", "G", "Ab", "A", "Bb", "B", "C", "Db", "D", "Eb"
};

char basefrets[12][CHORDSIZE];

char frets[(NUMFRETS+12*2)*CHORDSIZE];

#define chk_exit(cond, fmt, args...) do {                    \
    if (!(cond)) {                                           \
      printf("Error: " fmt "\n", ##args);                    \
      exit(1);                                               \
    }                                                        \
  } while (0)

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
  int scale_b = 0;
  int chord_root = -1;
  int chord_maj = 1;
  char scale_ch;

  argc--;  ++argv;
  while (argc > 0) {
    if (strcmp(argv[0], "-h") == 0 || strcmp(argv[0], "--help") == 0) {
      printf("Usage: fretboard [-h|--help] [-c|--chord [A..G][#|b][m|M]] [-s|--scale [A..G][#|b][m|M]]\n");
      exit(0);
    } else if (strcmp(argv[0], "-c") == 0 || strcmp(argv[0], "--chord") == 0) {
      chk_exit(argc > 1, "Missing argument to -c option!");
      chk_exit(argv[1][0] >= 'A' && argv[1][0] <= 'G', "Wrong argument to -c option!");
      chord_root = roots[argv[1][0] - 'A'];
      int i = 1;
      if (argv[1][i] == '#') {
        chord_root = (chord_root + 1) % 12;
        i++;
      } else if (argv[1][i] == 'b') {
        chord_root = (chord_root + 11) % 12;
        i++;
      }
      chk_exit(argv[1][i] == 0 || argv[1][i] == 'm', "Wrong argument to -c option!");
      chord_maj = argv[1][i] == 'm' ? 0 : 1;
      argc--;  argv++;
    } else if (strcmp(argv[0], "-s") == 0 || strcmp(argv[0], "--scale") == 0) {
      chk_exit(argc > 1, "Missing argument to -s option!");
      chk_exit(argv[1][0] >= 'A' && argv[1][0] <= 'G', "Wrong argument to -s option!");
      scale_ch = argv[1][0];
      scale_root = roots[scale_ch - 'A'];
      int i = 1;
      if (argv[1][i] == '#') {
        scale_root = (scale_root + 1) % 12;
        i++;
      } else if (argv[1][i] == 'b') {
        scale_root = (scale_root + 11) % 12;
        scale_b = 1;
        i++;
      }
      chk_exit(argv[1][i] == 0 || argv[1][i] == 'm', "Wrong argument to -s option!");
      scale_maj = argv[1][i] == 'm' ? 0 : 1;
      argc--;  argv++;
    } else {
      printf("Unknown option: %s\n", argv[0]);
      exit(1);
    }
    argc--;  ++argv;
  }

  if (scale_b)
    memcpy(basefrets, basefrets_b, sizeof(basefrets));
  else
    memcpy(basefrets, basefrets_d, sizeof(basefrets));

  if (scale_root != -1) {
    int grade = 0;
    for (int i = 0; i < 12; ++i) {
      if ((scale_maj && in_scale_maj(i)) || (!scale_maj && in_scale_min(i))) {
        char exp_ch = 'A' + ((scale_ch - 'A' + grade) % 7);
        int j;
        //printf("i=%d, exp_ch=%c\n", i, exp_ch);
        for (j = -2; j <= 2; j++)
          if (basefrets_d[(scale_root+i+j)%12][0] == exp_ch && basefrets_d[(scale_root+i+j)%12][1] == 0)
            break;
        //printf("j=%d\n", j);
        assert(j >= -2 && j <= 2);
        switch (j) {
        case -2:
          sprintf(basefrets[(scale_root+i)%12], "%c𝄪", exp_ch);
          break;
        case -1:
          sprintf(basefrets[(scale_root+i)%12], "%c#", exp_ch);
          break;
        case 0:
          sprintf(basefrets[(scale_root+i)%12], "%c", exp_ch);
          break;
        case 1:
          sprintf(basefrets[(scale_root+i)%12], "%cb", exp_ch);
          break;
        case 2:
          sprintf(basefrets[(scale_root+i)%12], "%c𝄫", exp_ch);
          break;
        }
        //printf("%s\n", basefrets[(scale_root+i)%12]);
        grade++;
      }
    }
  }

  printf(BGBLK "scale: %s%c, chord: %s%c\n", scale_root == -1 ? "-" : scale_b ? basefrets_b[scale_root] : basefrets_d[scale_root], scale_maj ? ' ' : 'm', chord_root == -1 ? "-" : basefrets_d[chord_root], chord_maj ? ' ' : 'm');
  printf("%s", BGBLK);
  for (int i = 0; i < sizeof(frets)/sizeof(frets[0])/CHORDSIZE; ++i)
    sprintf(frets + i*CHORDSIZE, "%s%d", basefrets[i % 12], 2 + (4+i)/12);
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
        printf("%s%3s" KGRN, col, &frets[absfret*CHORDSIZE]);
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
