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
#include "production_card.cpp"


using json = nlohmann::json;
using namespace std::chrono_literals;

/**
 * @brief Returns the exponential distribution object based on the input lambda.
 * 
 * @param lambda 
 * @return double 
 */
std::exponential_distribution<double> get_exponential_object(double lambda) {
    std::exponential_distribution<double> distribution(lambda);
    return distribution;
}

/**
 * @brief Returns the normal distribution object based on the mean m1 and the 
 *      deviation d1
 * 
 * @param m1 
 * @param d1 
 * @return std::normal_distribution<double> 
 */
std::normal_distribution<double> get_normal_dist_object(double m1, double d1) {
    std::normal_distribution<double> distribution(m1, d1);
    return distribution;
}


/**
 * @brief This function will be executed concurrently to simulate
 * the arrival of new cars to be produced in the factory.
 * 
 * @param exp 
 */
void new_cars_simulator(std::exponential_distribution<double> exp, int queue_id) {
    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    while (true){
        std::cout << "[ESTACION 1] Asignando nueva vehículo para producción.\n";
        ProductionCard car;
        msgsnd(queue_id, &car, sizeof(car), 0);
        double number = exp(generator);
        std::chrono::duration<double> period ( number );
        std::cout << "[ESTACION 1] Tiempo estimado para nueva llegada: " << period.count() << std::endl;
        std::this_thread::sleep_for( period );
    } 
}

int main()
{
    // Read parameters file
    std::cout << "[ESTACION 1] Creando estación 1\n";
    std::ifstream i("params.json");
    json config;
    i >> config;

    // Create queue for CADENA_0
    key_t key;
    int msgid;
    std::cout << "[ESTACION 1] Creando cola de arrivo de nuevos vehículos 1\n";
    std::string queue_name = config["queues"]["cadena_0"];
    int n = queue_name.length();
    char char_array[n + 1];
    strcpy(char_array, queue_name.c_str());
    key = ftok(char_array, 65);
    msgid = msgget(key, 0666 | IPC_CREAT);

    // Create thread to PUT new cars in the queue
    std::cout << "[ESTACION 1] Valor de lambda: " << config["station_1"]["lambda_1"] << std::endl;
    std::exponential_distribution<double> exp = get_exponential_object(
        config["station_1"]["lambda_1"]
        );
    std::thread t1(new_cars_simulator, exp, msgid);
    t1.detach();

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
        size_t data = msgrcv(msgid, &pcard, sizeof(pcard), 0, 0);
        if (data == 0) {
            std::cout << "[ESTACION 1] No hay vehículos en cola. " << std::endl;
            std::cout << "[ESTACION 1] No hay vehículos en cola. " << std::endl;
            std::this_thread::sleep_for(500ms);
            continue;
        }
        else {
            int seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::default_random_engine generator (seed);
            std::cout << "[ESTACION 1] Nuevo automovil entrando a producción " << std::endl;
            double number = exp(generator);
            std::chrono::duration<double> period ( number );
            std::cout << "[ESTACION 1] Procesando chasis y asignando identificador al automóvil. Tiempo estimado " << period.count() << std::endl;

            std::this_thread::sleep_for( period );
            pcard.car_id = ++car_id_counter;
            
            std::cout << "[ESTACION 1] Enviando automóvil " << pcard.car_id << " a la siguiente estación..." << std::endl;
            
        }
    }
    

    return 0;
}
