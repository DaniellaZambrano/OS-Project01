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


int main()
{
    // Read parameters file
    std::cout << "[ESTACION 1] Creando estación 1\n";
    std::ifstream i("params.json");
    json config;
    i >> config;

    // Create queue for CADENA_0
    std::cout << "[ESTACION 1] Creando cola de arrivo de nuevos vehículos 1\n";
    std::string queue_name = config["queues"]["cadena_0"];
    int msgid_0 = create_msg_queue(queue_name[0]);

    // Create thread to PUT new cars in the queue
    std::cout << "[ESTACION 1] Valor de lambda: " << config["station_1"]["lambda_1"] << std::endl;
    std::exponential_distribution<double> exp = get_exponential_object(
        config["station_1"]["lambda_1"]
        );
    std::thread t1(new_cars_simulator, exp, msgid_0);
    t1.detach();


    // Create queue for CADENA_1
    std::cout << "[ESTACION 1] Creando cadena de traslado entre estaciones 1 y 2\n";
    std::string queue_name_1 = config["queues"]["cadena_1"];
    int msgid_1 = create_msg_queue(queue_name_1[0]);



    // POP cars from the queue
    std::cout << "[ESTACION 1] Valor de media M1: " << config["station_1"]["mean_1"] << std::endl;
    std::cout << "[ESTACION 1] Valor de desviacion estandard D1: " << config["station_1"]["deviation_1"] << std::endl;
    std::normal_distribution<double> norm = get_normal_dist_object(
        config["station_1"]["mean_1"], config["station_1"]["deviation_1"]
    );

    int car_id_counter{0};

    ProductionCard pcard; 
    while (true)
    {
        size_t data = msgrcv(msgid_0, &pcard, sizeof(pcard), 0, 0);
        if (data == 0) {
            std::cout << "[ESTACION 1] No hay vehículos en cola. " << std::endl;
            std::this_thread::sleep_for(500ms);
            continue;
        }
        if(data == -1) {
            perror("error receiving message");
            exit(1);
        }

        else {
            int seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::default_random_engine generator (seed);
            std::cout << "[ESTACION 1] Nuevo automovil entrando a producción " << std::endl;
            double number = norm(generator);
            std::chrono::duration<double> period ( number );
            std::cout << "[ESTACION 1] Procesando chasis y asignando identificador al automóvil. Tiempo estimado " << period.count() << std::endl;

            std::this_thread::sleep_for( period );
            pcard.car_id = ++car_id_counter;
            
            std::cout << "[ESTACION 1] Enviando automóvil " << pcard.car_id << " a la siguiente estación..." << std::endl;
            if(msgsnd(msgid_1,&pcard,sizeof(pcard),0) == -1) {
                perror("sending msg");
                exit(1);
            }
        }
    }
    

    return 0;
}
