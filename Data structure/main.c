#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdint.h>

#include "Data.h"
#include "Data.c"

Product Products[NUMBER_OF_PRODUCTS];
Automat My_Automat;

int main(void)
{
    init_Automat(&My_Automat);

    print_Automat(My_Automat);

    // char *Products_Names[NUMBER_OF_PRODUCTS] = {"Snickers", "Twix", "Leon", "Leo", "Bueno", "Milka"};


    for(int i = 0; i <= 5; i++)
    {
        buy_Product(&My_Automat);
        change_Selection(&My_Automat, -1);
        print_Automat(My_Automat);
    }

    return 0;
}
