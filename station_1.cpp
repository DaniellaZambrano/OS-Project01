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
 * @brief This function will be executed concurrently to simulate
 * the arrival of new cars to be produced in the factory.
 * 
 * @param exp 
 */
void new_cars_simulator(std::exponential_distribution<double> exp, int queue_id) {
    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);
    while (true){
        std::cout << "[ESTACION 1] Asignando nueva vehículo para producción\n";
        ProductionCard car;
        msgsnd(queue_id, &car, sizeof(car), 0);
        double number = exp(generator);
        std::chrono::duration<double> period ( number );
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

    // Create messages queue 
    key_t key;
    int msgid;
  
    // ftok to generate unique key
    std::cout << "[ESTACION 1] Creando cola de arrivo de nuevos vehículos 1\n";
    std::string queue_name = config["queues"]["cadena_0"];
    int n = queue_name.length();
    char char_array[n + 1];
    strcpy(char_array, queue_name.c_str());
    key = ftok(char_array, 65);
    msgid = msgget(key, 0666 | IPC_CREAT);

    std::cout << "[ESTACION 1] Valor de lambda: " << config["station_1"]["lambda_1"] << std::endl;
    std::exponential_distribution<double> exp = get_exponential_object(config["station_1"]["lambda_1"]);
    std::thread t1(new_cars_simulator, exp, msgid);
    t1.join();
    
    return 0;
}
