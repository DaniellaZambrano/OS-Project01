
#include <iostream>
#include <chrono>
#include <thread>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "production_card.hpp"

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
        std::cout << "[ESTACION 1] Asignando nuevo vehículo para producción.\n";
        ProductionCard car;
        msgsnd(queue_id, &car, sizeof(car), 0);
        double number = exp(generator);
        std::chrono::duration<double> period ( number );
        std::cout << "[ESTACION 1] Tiempo estimado para nueva llegada: " << period.count() << std::endl;
        std::this_thread::sleep_for( period );
    } 
}
