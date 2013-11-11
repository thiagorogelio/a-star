#include "uthash.h"
#define sizex 4
#define sizey 4
#define N 0
#define S 1
#define L 2
#define O 3

typedef struct Vertex{
    int id;
    struct Vertex *father;
    int table[sizex][sizey];
    int mvposx, mvposy;
    int f,g,h;
    int lastmov;
} Vertex;

typedef struct listVertex{
    Vertex *vertex;
    struct listVertex *next;
} listVertex;

typedef struct my_struct {
    int id;                    /* key */
    UT_hash_handle hh;         /* makes this structure hashable */
} my_struct;
