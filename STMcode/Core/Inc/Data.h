#ifndef __DATA_h__
#define __DATA_h__

#include<stdint.h>
#include<stdbool.h>
#include "config.h"                             // file containing look up table for automate

typedef struct Product Product;
typedef struct Automat Automat;

struct Product{
    uint8_t slot_ID;                            // ID of slot where product is located in automate
    char name[NAME_MAXLENGTH];                  // name of the product
    uint8_t price;                              // product price
    uint8_t quantity;                           // quantity of product in observed slot
    uint8_t capacity;                           // percentage of products left in slot in comparison to when slot is full
};

void init_Product(Product*, uint8_t, char*, uint8_t, uint8_t);           // create new product

void print_Product(Product);                                             // printing product info, help with debugging

struct Automat{
    uint8_t products_Number;                    // number of different product in our machine
    Product products[NUMBER_OF_PRODUCTS];       // products machine contains
    uint8_t capacity;                           // percentage of products left in automate in comparison to when machine is full
    bool door_opened;                           // indicator for door being opened
    bool turned_on;                             // indicator for machine being turned on
    bool stand_by;                              // indicator for machine being in stand by mode
    int8_t current_Temperature;                 // value of the current temperature inside of the machine chamber
    uint8_t current_Humidity;                   // value of the current humidity inside of the machine chamber
    int8_t wanted_Temperature;                  // value of ideal temperature inside of the automat
    uint8_t light_Intensity;                    // percentage of light intensity in the automat
    uint8_t selected_Product;                   // ID of currently selected slot/product
    uint16_t current_Profit;                    // sum of the current profit since last time we emptied the machine
    bool overheating;                           // indicator for machine overheating
    bool freezing;                              // indicator for machine freezing
    uint8_t temperature_offset_tolerance;       // machine is overheating or freezing if current_temperature
                                                // is outside of [wanted_temp - temperature_offset_tolerance, wanted_temp + temperature_offset_tolerance]
};

void init_Automat(Automat*);                    // initialize Automate with default info from "config.h" file

bool open_Door(Automat*);                       // opens automate door

bool close_Door(Automat*);                      // closes automate door

void refill(Automat*);                          // refills products on default capacity and empties money container

void collect_Money(Automat*);                   // collects profit from money container

void change_Selection(Automat*, int8_t);        // change currently selected product

bool buy_Product(Automat*);                     // buy currently selected product (if possible)

void update_Automat_Capacity(Automat*);         // update automate capacity

void update_Product_Capacity(Product*);         // update procut capacity

void turn_Off(Automat*);                        // turn automate of

void turn_On(Automat*);                         // turn automate on

void update_Temperature_and_Humidity(Automat *, int8_t, uint8_t); // update temperature and humidity

void update_Wanted_Temperature(Automat*, int8_t); // update automate ideal temperature

void update_Light_Intensity(Automat*, uint8_t);   // update automate chamber light intensity

void print_Automat(Automat);                    // print automate state, help with debugging


#endif
