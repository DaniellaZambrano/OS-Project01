/**
 * @file production_card.cpp
 * @brief The definition of the ProductionCard struct which is passed across the differents stations.
 * @version 0.1
 * @date 2022-03-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <iostream>
using namespace std;

enum CarColor { red, green, blue };

string CAR_COLORS_STR[3] = {"Red", "Green", "Blue"};

enum CarBodywork { sedan, coupe };

string CAR_BODYWORK_STR[2] = {"Sedan", "Coupe"};

enum MotorType { gasoline, diesel, gas };

string MOTOR_TYPE_STR[3] = {"Gasoline", "Diesel", "Gas"};

enum CarSeatType { leather, fabric };

string CAR_SEAT_TYPE_STR[2] = { "Leather", "Fabric" };
  
struct ProductionCard {
    int car_id;
    CarColor color;
    CarBodywork car_bodywork;
    MotorType motor_type;
    CarSeatType seat_type;
};
