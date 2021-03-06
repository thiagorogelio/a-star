#include <stdio.h>
#include <stdlib.h>
#include "include/astar.h"

int ctvert;
int h;
listVertex *topo;
listVertex *lists[15];
my_struct *hsh = NULL;    /* important! initialize to NULL */

void add_hsh(int id) {
    struct my_struct *s;
    s = malloc(sizeof(struct my_struct));
    s->id = id;
    HASH_ADD_INT( hsh, id, s );  /* id: name of key field */
}

my_struct *find_hsh(int id) {
    my_struct *s;
    HASH_FIND_INT( hsh, &id, s );  /* s: output pointer */
    return s;
}

void h1 (Vertex *vertex){
    int i,j,k=1;
    vertex->h=0;
    for(i=0;i<4;i++)
        for(j=0;j<4;j++)
            if(vertex->table[i][j] != k++)
                vertex->h++;
    if(vertex->table[3][3] == 0)
        vertex->h--;
    vertex->f = vertex->g+vertex->h;
}

void h2 (Vertex *vertex){
    int i,j,k=1;
    vertex->h=0;
    for(i=0;i<4;i++){
        for(j=0;j<3;j++)
            if(vertex->table[i][j]+1 != vertex->table[i][j+1])
                vertex->h++;
        if(i<3)
            if(vertex->table[i][3]+1 != vertex->table[i+1][0])
                vertex->h++;
    }
    vertex->h--;
    vertex->f = vertex->g+vertex->h;
}

void h3 (Vertex *vertex){
    int i,j,k=1;
    vertex->h=0;
    for(i=0;i<4;i++)
        for(j=0;j<4;j++)
            if((vertex->table[i][j] != k++)&&(vertex->table[i][j] != 0)){
                if((vertex->table[i][j]/4)-i >= 0)
                    vertex->h+=(vertex->table[i][j]/4)-i;
                else
                    vertex->h+=i-(vertex->table[i][j]/4);
                if(((vertex->table[i][j]-(vertex->table[i][j]/4)*4)-j) >= 0)
                    vertex->h+=(vertex->table[i][j]-(vertex->table[i][j]/4)*4)-j;
                else
                    vertex->h+=j-(vertex->table[i][j]-(vertex->table[i][j]/4)*4);
            }
    vertex->h+=3-vertex->mvposx;
    vertex->h+=3-vertex->mvposy;
    vertex->f = vertex->g+vertex->h;
}

void h4 (Vertex *vertex){
    int buff = 0;
    h1(vertex);
    buff += vertex->h;
    h2(vertex);
    buff += vertex->h;
    h3(vertex);
    buff += vertex->h;
    vertex->h = buff;
    vertex->f = vertex->g + buff;
}

void h5 (Vertex *vertex){
    int r1,r2,r3;
    h1(vertex);
    r1 = vertex->h;
    h2(vertex);
    r2 = vertex->h;
    h3(vertex);
    r3 = vertex->h;
    if((r1>=r2)&&(r1>=r3))
        vertex->h = r1;
    else if((r2>=r1)&&(r2>=r3))
        vertex->h = r2;
    else if((r3>=r1)&&(r3>=r2))
        vertex->h = r3;
    vertex->f = vertex->g + vertex->h;
}

void calculateGH(Vertex *vertex){
    vertex->g++;
    switch(h){
        case 1: h1(vertex);break;
        case 2: h2(vertex);break;
        case 3: h3(vertex);break;
        case 4: h4(vertex);break;
        case 5: h5(vertex);break;
    }
}

Vertex *importaArq(char nomearq[]){
    Vertex *vertex = malloc(sizeof (Vertex));
    char c;
    FILE* fd;
    if ((fd=fopen(nomearq,"rb")) == NULL) {
       printf("\nArquivo não encontrado! :(");
    }else{
        int i = 0, j = 0, buffct = 0;
        c = getc(fd);
        char buff[2];
        while(c != EOF){
            if(c == ' '){
                j++;
            }
            else if(c == '\n'){
                j = 0;
                i++;
            } else {
                buff[buffct++] = c;
            }
            c = getc(fd);
            if(((c == ' ')||(c == '\n')) && (buffct > 0)){
                buffct = 0;
                vertex->table[i][j] = atoi(buff);
                if(vertex->table[i][j] == 0){
                    vertex->mvposx = i;
                    vertex->mvposy = j;
                }
                buff[1] = '\0';
            }
        }
    }
    vertex->lastmov = 4;
    vertex->g = 0;
    calculateGH(vertex);
    vertex->g--;
    vertex->id = genid(vertex);
    return vertex;
}

void imprimeVertex(Vertex *vertex){
    int i, j;
    printf("\n");
    for(i=0;i<4;i++){
        printf("\n");
        for(j=0;j<4;j++){
            printf("%d\t",vertex->table[i][j]);
        }
    }
}

int imprimeCaminho(Vertex *vertex){
    if(vertex->father == NULL){
        imprimeVertex(vertex);
        return 0;
    }
    int ct = imprimeCaminho(vertex->father) +1;
    imprimeVertex(vertex);
    return ct;
}

void initialize(){
    ctvert = -1;
    int i;
    for(i=0;i<16;i++)
        (lists[i]) = NULL;
    topo = NULL;
}

int vertexExist(Vertex *vertex){
    //my_struct *aux = find_hsh(vertex->id);
    //if(aux != NULL)
        //return 1;
    return 0;
}

int genid(Vertex *vertex){/*
    int i, j, soma;
    char buff[10];
    buff[0] = '\0';
    for(i=0;i<3;i++){
        soma = 0;
        for(j=0;j<3;j++)
            soma= soma + vertex->table[i][j] + vertex->table[j][i];
        sprintf(buff,"%s%d",buff,soma);
    }*/
    return 10;
}

void insertVertex(Vertex *vertex){
    ctvert++;
    listVertex *newlist = malloc(sizeof (listVertex));
    newlist->vertex = vertex;
    if(topo == NULL)
        topo = newlist;
    else {
        listVertex *tp = topo;
        listVertex *ant = topo;
        if(topo->vertex->f >= vertex->f){
            newlist->next = topo;
            topo = newlist;
        } else {
            while(tp->next != NULL && tp->vertex->f < vertex->f){
                    ant = tp;
                    tp = tp->next;
            }
            newlist->next = tp;
            ant->next = newlist;
        }
    }
    //add_hsh(vertex->id);
}

Vertex *cpyVertex(Vertex *vertex, int x, int y){
    Vertex *newvertex = malloc(sizeof (Vertex));
    *newvertex = *vertex;
    newvertex->father = vertex;
    newvertex->table[newvertex->mvposx][newvertex->mvposy] = newvertex->table[newvertex->mvposx+x][newvertex->mvposy+y];
    newvertex->table[newvertex->mvposx+x][newvertex->mvposy+y] = 0;
    newvertex->mvposy+=y;
    newvertex->mvposx+=x;
    calculateGH(newvertex);
    newvertex->id = genid(newvertex);
    return newvertex;
}

void generateChilds(Vertex *vertex){
    topo = topo->next;
    Vertex *newvertex;
    // Move the 0 up
    if((vertex->lastmov != S)&&(vertex->mvposy > 0)){
        newvertex = cpyVertex(vertex, 0, -1);
        newvertex->lastmov = N;
        if(vertexExist(newvertex) != 1)
            insertVertex(newvertex);
    }
    // Move the 0 down
    if((vertex->lastmov != N)&&(vertex->mvposy < 3)){
        newvertex = cpyVertex(vertex, 0, 1);
        newvertex->lastmov = S;
        if(vertexExist(newvertex) != 1)
            insertVertex(newvertex);
    }
     // Move the 0 to right
    if((vertex->lastmov != O)&&(vertex->mvposx < 3)){
        newvertex = cpyVertex(vertex, 1, 0);
        newvertex->lastmov = L;
        if(vertexExist(newvertex) != 1)
            insertVertex(newvertex);
    }
     // Move the 0 to left
    if((vertex->lastmov != L)&&(vertex->mvposx > 0)){
        newvertex = cpyVertex(vertex, -1, 0);
        newvertex->lastmov = O;
        if(vertexExist(newvertex) != 1)
            insertVertex(newvertex);
    }
}

void Astar(){
    while(topo->vertex->h != 0)
        generateChilds(topo->vertex);
}

int main( int argc, char *argv[] )
{
    initialize();
    if ( argc != 3 )
    {
        printf( "usage: %s <filename> <heuristic_num 0-5>\n", argv[0] );
    }
    else
    {
        h = atoi(argv[2]);
	if(h < 1 || h > 6){
		printf( "Please choose heuristic 1 to 5\n");
		return 0;	
	}
 	printf("Running heuristic h(%s) in %s",argv[2],argv[1]);
        Vertex *vertex = importaArq(argv[1]);
        insertVertex(vertex);
        //printf("vertex h %d",vertex->h);
        Astar();
        printf("\n\n%d Moves",imprimeCaminho(topo->vertex));
        printf("\n%d Vertex(es) Generated\n",ctvert);
    }
    return 0;
}
