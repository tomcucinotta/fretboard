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
#define KWHT  "\x1B[39m"

#define KGRY  "\x1B[38;2;128;128;128m"
#define KGRY2 "\x1B[37;2m"

#define KBLK  "\x1B[38;2;0;0;0m"
#define KDEF  "\x1B[39m"
#define BGBLK "\x1B[48;2;0;0;0m"
#define BGWHT "\x1B[48;2;255;255;255m"
#define BGDEF "\x1B[49m"

#define KNRMFACE  "\x1B[22m"
#define KBOLD "\x1B[1m"

#define MAX_NUMSTRINGS 6
#define MAX_NUMFRETS 30

typedef enum {
      GUITAR = 0,
      UKULELE,
      NUM_INSTR
} Instrument;

int num_strings[NUM_INSTR] = { 6, 4 };
int num_frets[NUM_INSTR] = { 25, 20 };

// Fret names 
typedef enum {
  FRET_E = 0, FRET_F, FRET_Fd, FRET_G, FRET_Gd, FRET_A, FRET_Ad, FRET_B, FRET_C, FRET_Cd, FRET_D, FRED_Dd
} Fret_E;

int string2frets[NUM_INSTR][MAX_NUMSTRINGS] = {
  { 0, 5, 10, 15, 19, 24 },                    // EADGBE (Mi La Re Sol Si Mi), Standard tuning
  { 12*2+3, 12*2-4, 12*2, 12*2+5, -1, -1 }
};

// Alternate tuning for guitar:
//  { 0, 5, 10, 15, 19, 24 },                    // EADGBE (Mi La Re Sol Si Mi), Standard tuning
//  { -2, 5, 10, 15, 19, 24 },                   // DADGBE (Re La Re Sol Si Mi), Dropped D
//  { -2, 3, 10, 15, 19, 22 },                   // DGDGBD (Re Sol Re Sol Si Re), Open G

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

int instr = GUITAR;

char basefrets[12][CHORDSIZE];

char frets[(MAX_NUMFRETS+12*3)*CHORDSIZE];

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

#define CHORD_FRETS 4

typedef struct {
  char *name;
  int frets[CHORD_FRETS];
} chord_t;

chord_t chords[] = {
  { "",  { 0, 4, 7, -1 } },
  { "m", { 0, 3, 7, -1 } },
  { "7", { 0, 4, 7, 10 } },
  { "m7", { 0, 3, 7, 10 } },
  { "maj7", { 0, 4, 7, 11 } },
  { "5", { 0, 7, -1, -1 } },
  { "6", { 0, 4, 7, 9 } },
  { "m6", { 0, 3, 7, 9 } },
  { "dim", { 0, 3, 6, -1 } },
  { "sus2", { 0, 2, 7, -1 } },
  { "sus4", { 0, 5, 7, -1 } },
  { "aug",  { 0, 4, 8, -1 } },
  { "aug7",  { 0, 4, 8, 10 } },
};

int scale_root = -1;
int scale_maj = 1;
int scale_b = 0;
int chord_root = -1;
chord_t *p_chord = NULL;
char scale_ch;
int piano_mode = 0;

chord_t *find_chord(const char *chord_name) {
  for (int i = 0; i < sizeof(chords) / sizeof(chords[0]); i++)
    if (strcmp(chords[i].name, chord_name) == 0)
      return &chords[i];
  return NULL;
}

int in_chord(int f, chord_t *p_chord) {
  for (int i = 0; i < CHORD_FRETS && p_chord->frets[i] != -1; i++)
    if (f == p_chord->frets[i])
      return 1;
  return 0;
}

int num_octs = 2;

char black_keys[12] = {
  0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0
};

int is_black(int k) {
  return black_keys[k % 12];
}

int is_white(int k) {
  return !is_black(k);
}

// 0: not in scale
// 1: in scale
int keyb_in_scale(int i) {
  if (scale_root == -1)
    return 0;
  int k = (24 - (scale_root + 4) + i) % 12;
  return ((scale_maj && in_scale_maj(k)) || (!scale_maj && in_scale_min(k)));
}

// 0: not in chord
// 1: in chord
// 2: base chord note
int keyb_in_chord(int i) {
  if (chord_root == -1)
    return 0;
  int k = (24 - (chord_root + 4) + i) % 12;
  return (k == 0 ? 2 : in_chord(k, p_chord) ? 1 : 0);
}

void draw_keyboard() {
  for (int oct = 0; oct < num_octs; oct++)
    printf(",___________________________");
  printf(", ");
  printf("\n");
  for (int r = 0; r < 6; r++) {
    char sp = (r == 5 ? '_' : ' ');
    printf("|%c", sp);
    for (int oct = 0; oct < num_octs; oct++) {
      for (int i = 0; i < 12; i++) {
        char * chp =
          keyb_in_chord(i) == 2 ? KBOLD KCYN "#" KDEF KNRMFACE :
          keyb_in_chord(i) ? KGRN "#" KDEF :
          keyb_in_scale(i) ? KGRY "o" KDEF :
          " ";
        if ((is_black(i) && r == 2) || (is_white(i) && r == 5))
          chp = KDEF "_";
        if (r < 3) {
          if (is_white(i) && is_white(i+1))
            printf("  ");
          else {
            if (is_black(i))
              printf("%s|", chp);
            else
              printf("%c|", sp);
          }
        } else {
          if (is_white(i))
            printf("%s%c", chp, sp);
          else
            printf("|%c", sp);
        }
        if (is_white(i) && is_white(i+1))
          printf("|%c", (r == 5 && i == 11 && oct == num_octs-1 ? ' ' : sp));
      }
    }
    printf("\n");
  }
  printf("\n");
}

void draw_fretboard() {
  for (int i = 0; i < sizeof(frets)/sizeof(frets[0])/CHORDSIZE; ++i)
    sprintf(frets + i*CHORDSIZE, "%s%d", basefrets[i % 12], 1 + (4+i)/12);
  for (int s = num_strings[instr] -1; s >=0; --s) {
    for (int f = 0; f < num_frets[instr]; ++f) {
      int absfret = 12 + string2frets[instr][s] + f;
      int scale_fret = (absfret + 24 - scale_root) % 12;
      char *col = KBLK;
      if (scale_root != -1)
        col = (scale_maj ? in_scale_maj(scale_fret) : in_scale_min(scale_fret)) ? (scale_fret == 0 ? KGRY2 : KGRY) : col;
      if (chord_root != -1) {
        int chord_fret = (absfret + 12 - chord_root) % 12;
        col = chord_fret == 0 ? KWHT : in_chord(chord_fret, p_chord) ? KGRN : col;
      }
      if (scale_root == -1 && chord_root == -1)
        col = KGRY;
      if (strcmp(col, KBLK) == 0)
        printf(KGRY "---");
      else
        printf("%s%3s" KGRN, col, &frets[absfret*CHORDSIZE]);
      printf(KGRY "%c", f == 0 ? '|' : ' ');
    }
    printf("\n");
  }
  for (int f = 0; f < num_frets[instr]; ++f)
    printf("~~~%s", f == 0 ? "+" : "~");
  printf("\n");
  for (int f = 0; f < num_frets[instr]; ++f)
    if (f%12 == 5 || f%12 == 7 || f%12 == 0)
      printf("%s%3d ",  KWHT, f);
    else
      printf("%s%3d ", KGRY, f);
  printf("\n");
}

int main(int argc, const char *argv[]) {
  argc--;  ++argv;
  while (argc > 0) {
    if (strcmp(argv[0], "-h") == 0 || strcmp(argv[0], "--help") == 0) {
      printf("Usage: fretboard [-h|--help] [-u|--ukulele] [-g|--guitar] [-p|--piano] [-c|--chord [A..G][#|b][m|m7|5|6|m6|dim|sus2|sus4|aug|aug7] [-s|--scale [A..G][#|b][m|M]]\n");
      exit(0);
    } else if (strcmp(argv[0], "-u") == 0 || strcmp(argv[0], "--ukulele") == 0) {
      instr = UKULELE;
    } else if (strcmp(argv[0], "-g") == 0 || strcmp(argv[0], "--guitar") == 0) {
      instr = GUITAR;
    } else if (strcmp(argv[0], "-p") == 0 || strcmp(argv[0], "--piano") == 0) {
      piano_mode = 1;
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
      p_chord = find_chord(&argv[1][i]);
      chk_exit(p_chord != NULL, "Unknown chord type: %s", argv[1]);
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

  printf("scale: %s%c, chord: %s%s\n", scale_root == -1 ? "-" : scale_b ? basefrets_b[scale_root] : basefrets_d[scale_root], scale_maj ? ' ' : 'm', chord_root == -1 ? "-" : basefrets_d[chord_root], p_chord == NULL ? "" : p_chord->name);

  if (piano_mode)
    draw_keyboard();
  else
    draw_fretboard();
}
