#ifndef __CONFIG_h__
#define __CONFIG_h__

#define NAME_MAXLENGTH 20
#define START_QUANTITY 10
#define NUMBER_OF_PRODUCTS 6
#define DEFAULT_WANTED_TEMPERATURE 20
#define DEFAULT_LIGHT_INTENSITY 70
#define TEMPERATURE_OFFSET_TOLERANCE 2

char *Products_Names[NUMBER_OF_PRODUCTS] = {"Snickers", "Twix", "Leon", "Leo", "Bueno", "Milka"};
float Products_Prices[NUMBER_OF_PRODUCTS] = {1.5, 1.5, 1.5, 1, 2, 3};

#endif
