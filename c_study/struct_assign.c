#include <stdio.h>

struct point {
    int x;
    int y;
};

struct point makepoint(int x, int y)
{
    struct point temp;
    temp.x = x;
    temp.y = y;
    return temp;
}

struct point addpoint(struct point p1, struct point p2)
{
    p1.x += p2.x;
    p1.y += p2.y;
    return p1;
}

int main(int argc, char *argv[])
{
    struct point A = {3, 4};
    struct point B;
    struct point C;

    B = makepoint(8, 10);
    C = addpoint(A, B);

    printf("A=(%d, %d)\n", A.x, A.y);
    printf("B=(%d, %d)\n", B.x, B.y);
    printf("C=(%d, %d)\n", C.x, C.y);

    return 0;
}
