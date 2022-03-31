/**
 * @file station_1.cpp
 * @brief This code is the Station 1 of the problem. 
 * @version 0.1
 * @date 2022-03-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <fstream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include "json.hpp"
#include "production_card.hpp"
#include "utilities.hpp"

using json = nlohmann::json;
using namespace std::chrono_literals;

int main(){
    // Read parameters file
    std::cout << "[ESTACION 2] Creando estación 2\n";
    std::ifstream i("params.json");
    json config;
    i >> config;

    // Create queue for CADENA_1
    std::cout << "[ESTACION 2] Creando cadena de traslado entre estaciones 1 y 2\n";
    std::string queue_name_1 = config["queues"]["cadena_1"];
    int msgid_1 = create_msg_queue(queue_name_1);


    // Create queue for CADENA_2
    std::cout << "[ESTACION 2] Creando cadena de traslado entre estaciones 2 y 3\n";
    std::string queue_name_2 = config["queues"]["cadena_2"];
    int msgid_2 = create_msg_queue(queue_name_2);


    std::normal_distribution<double> norm = get_normal_dist_object(config["station_2"]["mean_2"], config["station_2"]["deviation_2"]);


    // POP cars from the queue
    std::cout << "[ESTACION 2] Valor de media M2: " << config["station_2"]["mean_2"] << std::endl;
    std::cout << "[ESTACION 2] Valor de desviacion estandard D2: " << config["station_2"]["deviation_2"] << std::endl;


    CarColor color_counter = CarColor::red;
    ProductionCard pcard; 


    while (true)
    {
        size_t data = msgrcv(msgid_1, &pcard, sizeof(pcard), 0, 0);
        if (data == 0) {
            std::cout << "[ESTACION 2] No hay vehículos en cola. " << std::endl;
            std::this_thread::sleep_for(200ms);
            continue;
        }
        else {
            int seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::default_random_engine generator (seed);
            double number = norm(generator);
            std::chrono::duration<double> period ( number );

            std::cout << "[ESTACION 2] Automovil "<< pcard.car_id <<" entrando a pintura" << std::endl;
            std::cout << "[ESTACION 2] Tiempo estimado " << period.count() << std::endl;

            // Add color to the car
            color_counter = (CarColor)(((int)(color_counter)+1)%3);
            pcard.color = color_counter;
            std::cout << "[ESTACION 2] Color asignado al automovil "<< pcard.car_id <<":" <<CAR_COLORS_STR[pcard.color] << std::endl;


            std::this_thread::sleep_for( period );

            // Add car bodywork
            pcard.car_bodywork = (CarBodywork)(seed%2);
            std::cout << "[ESTACION 2] Carroceria asignada al automovil "<< pcard.car_id <<":" <<CAR_BODYWORK_STR[pcard.car_bodywork] << std::endl;

            
            std::cout << "[ESTACION 2] Enviando automóvil " << pcard.car_id << " a la siguiente estación..." << std::endl;
            msgsnd(msgid_2,&pcard,sizeof(pcard),0);            
        }
    }

    return 0;
}