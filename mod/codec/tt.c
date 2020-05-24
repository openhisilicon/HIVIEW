#include<stdio.h>

struct bit_order{
    unsigned char a: 2,
                  b: 3,
                  c: 3;

};

int main(int argc, char *argv[])
{
    unsigned char ch = 0x79;
    struct bit_order *ptr = (struct bit_order *)&ch;

    printf("bit_order->a : %u\n", ptr->a);
    printf("bit_order->b : %u\n", ptr->b);
    printf("bit_order->c : %u\n", ptr->c);

    return 0;

}
