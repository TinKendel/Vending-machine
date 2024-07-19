#include "config.h"
#include "Data.h"
#include <stdio.h>
#include <string.h>

typedef struct Product Product;
typedef struct Automat Automat;


void init_Product(Product* pr, uint8_t id, char* product_name, uint8_t product_price, uint8_t product_quant)
{
    pr->slot_ID = id;
    strcpy(pr->name, product_name);
    pr->price = product_price;
    pr->quantity = product_quant;
    pr->capacity = 100;
}

void print_Product(Product pr)
{
    printf("Slot %d: %s %d$\n", pr.slot_ID, pr.name, pr.price);
    printf("Quantity: %d, %d%% left\n", pr.quantity, pr.capacity);
}

 void init_Automat(Automat* a)
{
    a->products_Number = NUMBER_OF_PRODUCTS;

    for(int i = 0; i < a->products_Number; ++i)
    {
        init_Product(&a->products[i], i, Products_Names[i], Products_Prices[i], START_QUANTITY);
    }

    a->door_opened = false;
    a->stand_by = false;
    a->turned_on = true;
    a->selected_Product = 0;
    a->current_Profit = 0;
    a->temperature_offset_tolerance = TEMPERATURE_OFFSET_TOLERANCE;

    update_Automat_Capacity(a);
    update_Light_Intensity(a, DEFAULT_LIGHT_INTENSITY);
    update_Wanted_Temperature(a, DEFAULT_WANTED_TEMPERATURE);
    update_Temperature_and_Humidity(a);
}

void open_Door(Automat* A)
{
    if(!A->door_opened && A->turned_on && !A->stand_by)
    {
        A->door_opened = true;
        A->stand_by = true;
    }
}

void close_Door(Automat* A)
{
    if(A->door_opened && A->turned_on && A->stand_by)
    {
        A->door_opened = false;
        A->stand_by = false;
    }
}

void collect_Money(Automat* A)
{
    if(A->door_opened && A->stand_by && A->turned_on)
    {
        A->current_Profit = 0;
    }
}

void refill(Automat* A)
{
    open_Door(A);

    for(int i = 0; i < NUMBER_OF_PRODUCTS; i++)
    {
        A->products[i].quantity = START_QUANTITY;
        A->products[i].capacity = 100;
    }

    update_Automat_Capacity(A);
    collect_Money(A);

    close_Door(A);
}

void change_Selection(Automat* a, int8_t direction)
{
    if(!a->turned_on || a->stand_by)
    {
        return;
    }

    switch(direction)
    {
        case -1:
        {
            a->selected_Product -= 1;
            a->selected_Product += (NUMBER_OF_PRODUCTS);
            a->selected_Product %= (NUMBER_OF_PRODUCTS);
            break;
        }
        case 1:
        {
            a->selected_Product += 1;
            a->selected_Product %= (NUMBER_OF_PRODUCTS);
            break;
        }
    }
}

void update_Automat_Capacity(Automat* a)
{
    if(!a->turned_on)
    {
        return;
    }

    uint16_t products_Quantity = 0;

    for(int i = 0; i < NUMBER_OF_PRODUCTS; ++i)
    {
        products_Quantity += a->products[i].quantity;
    }

    a->capacity = (products_Quantity * 100) / (START_QUANTITY * NUMBER_OF_PRODUCTS);
}

void update_Product_Capacity(Product* p)
{
    p->capacity = (p->quantity * 100) / START_QUANTITY;
}

void buy_Product(Automat* a)
{
    if(!a->turned_on || a->stand_by)
    {
        return;
    }

    uint8_t slot_ID = a->selected_Product;
    Product buying_Prod = a->products[slot_ID];

    if(buying_Prod.quantity > 0)
    {
        printf("Buying product: %s\n", buying_Prod.name);
        printf("PLEASE WAIT...\n");
        a->products[slot_ID].quantity -= 1;
        update_Product_Capacity(&a->products[slot_ID]);
        a->current_Profit += buying_Prod.price;
        update_Automat_Capacity(a);
    }
    else
    {
        printf("PRODUCT OUT OF STOCK!\n");
        printf("PLEASE TRY AGAIN!\n");
    }
}

void turn_Off(Automat* a)
{
    if(a->turned_on)
    {
        a->light_Intensity = 0;
        a->turned_on = false;
    }
}

void turn_On(Automat* a)
{
    if(!a->turned_on)
    {
        init_Automat(a);
    }
}

void update_Temperature_and_Humidity(Automat *a)
{
    // citanje temperature i vlage sa senzora i spremanje u a->current_Temperature i a->current_Humidity

    if(!a->turned_on)
    {
        return;
    }

    if(a->wanted_Temperature > a->current_Temperature + a->temperature_offset_tolerance)
    {
        a->freezing = true;
        a->overheating = false;
    }
    else if(a->wanted_Temperature < a->current_Temperature - a->temperature_offset_tolerance)
    {
        a->overheating = true;
        a->freezing = false;
    }
}

void update_Wanted_Temperature(Automat* a, int8_t wanted_temp)
{
    if(!a->turned_on)
    {
        return;
    }

    a->wanted_Temperature = wanted_temp;
    update_Temperature_and_Humidity(a);
}

void update_Light_Intensity(Automat* a, uint8_t value)
{
    if(!a->turned_on)
    {
        return;
    }

    a->light_Intensity = value;
}


void print_Automat(Automat a)
{
    printf("Proizvodi u automatu: \n");

    if(a.selected_Product == 0)
    {
        printf("*************************************\n");
    }
    else
    {
        printf("-------------------------------------\n");
    }

    for(int i = 0; i < NUMBER_OF_PRODUCTS; ++i)
    {
        print_Product(a.products[i]);
        if(a.selected_Product == i + 1 || a.selected_Product == i)
        {
            printf("*************************************\n");
        }
        else
        {
            printf("-------------------------------------\n");
        }
    }

    a.turned_on ? printf("MACHINE TURNED ON\n") : printf("MACHINE TURNED OFF\n");
    a.stand_by ? printf("MACHINE ON STAND BY\n") : printf("MACHINE READY\n");
    printf("CAPACITY: %d%%\n", a.capacity);
    a.door_opened ? printf("DOOR OPENED\n") : printf("DOOR CLOSED\n");
    printf("WANTED TEMPERATURE: %d\n", a.wanted_Temperature);
    printf("LIGHT INTENSITY: %d%%\n", a.light_Intensity);
    printf("CURRENT PROFIT: %d$\n", a.current_Profit);
}




