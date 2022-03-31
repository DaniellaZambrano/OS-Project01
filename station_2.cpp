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
    std::cout << "[ESTACION 2] Creando estación 1\n";
    std::ifstream i("params.json");
    json config;
    i >> config;

    // Create queue for CADENA_1
    std::cout << "[ESTACION 2] Creando cadena de traslado entre estaciones 1 y 2\n";
    std::string queue_name_1 = config["queues"]["cadena_1"];
    int msgid_1 = create_msg_queue(queue_name_1);
    std::cout<<"Key: "<<msgid_1;





    std::normal_distribution<double> norm = get_normal_dist_object(
        config["station_1"]["mean_1"], config["station_1"]["deviation_1"]
    );


    ProductionCard pcard; 


    while (true)
    {
        size_t data = msgrcv(msgid_1, &pcard, sizeof(pcard), 0, 0);
        if (data == 0) {
            std::cout << "[ESTACION 2] No hay vehículos en cola. " << std::endl;
            std::this_thread::sleep_for(500ms);
            continue;
        }
        else {
            int seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::default_random_engine generator (seed);
            std::cout << "[ESTACION 2] Automovil entrando a pintura" << std::endl;
            double number = norm(generator);
            std::chrono::duration<double> period ( number );
            // std::cout << "[ESTACION 1] Procesando chasis y asignando identificador al automóvil. Tiempo estimado " << period.count() << std::endl;

            std::this_thread::sleep_for( period );
            
            std::cout << "[ESTACION 2] Enviando automóvil " << pcard.car_id << " a la siguiente estación..." << std::endl;
            // msgsnd(msgid_1,&pcard,sizeof(pcard),0);            
        }
    }

    return 0;
}