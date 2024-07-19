#include "config.h"
#include "Data.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct Product Product;
typedef struct Automat Automat;

//extern bool power_flag_up;
//extern bool door_flag_up;
//extern bool refilled_flag_up;
//extern bool ideal_temperature_flag_up;
//extern int8_t ideal_temperature;
//extern bool received_data_read;

char *Products_Names[NUMBER_OF_PRODUCTS] = {"Coca-Cola", "Pipi", "Hidra", "Cetina", "Cedevita", "Red Bull"};
float Products_Prices[NUMBER_OF_PRODUCTS] = {3, 2, 4, 1, 3, 5};

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
    update_Temperature_and_Humidity(a, 0, 0);
}

bool open_Door(Automat* A)
{
    if(!A->door_opened)
    {
        A->door_opened = true;
        A->stand_by = true;
        return true;
    }
    else
    {
    	return false;
    }
}

bool close_Door(Automat* A)
{
    if(A->door_opened)
    {
        A->door_opened = false;
        A->stand_by = false;
        return true;
    }
    else
    {
    	return false;
    }
}

void collect_Money(Automat* A)
{
    if(A->turned_on)
    {
        A->current_Profit = 0;
    }
}

void refill(Automat* A)
{
    if(A->door_opened || !A->turned_on)
    {
    	return;
    }

    for(int i = 0; i < NUMBER_OF_PRODUCTS; i++)
    {
        A->products[i].quantity = START_QUANTITY;
        A->products[i].capacity = 100;
    }

    update_Automat_Capacity(A);
    collect_Money(A);
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
    if(!a->turned_on || a->stand_by)
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

bool buy_Product(Automat* a)
{
    if(!a->turned_on || a->stand_by)
    {
        return false;
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
        return true;
    }
    else
    {
    	return false;
//        printf("PRODUCT OUT OF STOCK!\n");
//        printf("PLEASE TRY AGAIN!\n");
    }
}

void turn_Off(Automat* a)
{
    if(a->turned_on)
    {
        a->turned_on = false;
    }
}

void turn_On(Automat* a)
{
    if(!a->turned_on)
    {
        a->turned_on = true;
    }
}

void update_Temperature_and_Humidity(Automat *a, int8_t temperature, uint8_t humidity)
{
    // citanje temperature i vlage sa senzora i spremanje u a->current_Temperature i a->current_Humidity

    if(!a->turned_on)
    {
        return;
    }

    a->current_Temperature = temperature;
    a->current_Humidity = humidity;

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
    else
    {
        a->overheating = false;
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
    update_Temperature_and_Humidity(a, a->current_Temperature, a->current_Humidity);
}

void update_Light_Intensity(Automat* a, uint8_t value)
{
    if(!a->turned_on) //|| !a->door_opened
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




