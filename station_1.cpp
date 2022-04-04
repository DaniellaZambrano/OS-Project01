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

int main() {
    // Read parameters file
    std::cout << "[ESTACION 1] Creando estación 1\n";
    json config{ get_config() };

    // Create queue for CADENA_0
    std::cout << "[ESTACION 1] Creando cola de arrivo de nuevos vehículos 1\n";
    int msgid_0 = create_msg_queue(config["queues"]["cadena_0"]);

    // Create queue for CADENA_1
    std::cout << "[ESTACION 1] Creando cadena de traslado entre estaciones 1 y 2\n";
    int msgid_1 = create_msg_queue(config["queues"]["cadena_1"]);

    // Create queue for SUPERVISOR
    std::cout << "[ESTACION 1] Creando cadena de información del supervisor\n";
    int supervisor_queue_id = create_msg_queue(config["queues"]["supervisor"]);

    // Create thread to PUT new cars in the queue
    std::cout << "[ESTACION 1] Valor de lambda: " << config["station_1"]["lambda"] << std::endl;
    std::exponential_distribution<double> exp = get_exponential_object(
        config["station_1"]["lambda"]);
    std::thread t1(new_cars_simulator, exp, msgid_0);
    t1.detach();

    // POP cars from the queue

    std::normal_distribution<double> norm = get_normal_dist_object(
        config["station_1"]["mean"], config["station_1"]["deviation"]);

    std::cout << "[ESTACION 1] Valor de media: " << norm.mean() << std::endl;
    std::cout << "[ESTACION 1] Valor de desviacion estandard: " << norm.stddev() << std::endl;

    int car_id_counter{ 0 };

    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    QueueMessage msg;
    while (true) {
        ssize_t data = msgrcv(msgid_0, &msg, sizeof(msg.mtext), 1, 0);

        if (data < 0) {
            perror("[ESTACION 1] error receiving message");
            exit(1);
        }

        ProductionCard& pcard{ msg.mtext };

        std::cout << "[ESTACION 1] Nuevo automovil entrando a producción " << std::endl;

        std::chrono::duration<double> period(norm(generator));
        std::cout << "[ESTACION 1] Procesando chasis y asignando identificador al automóvil. Tiempo estimado " << period.count() << std::endl;

        pcard.car_id = ++car_id_counter;

        pcard.station = 1;
        if (msgsnd(supervisor_queue_id, &msg, sizeof(msg.mtext), 0) < 0) {
            perror("[ESTACION 1] sending card to supervisor");
            exit(1);
        }

        std::this_thread::sleep_for(period);

        std::cout << "[ESTACION 1] Enviando automóvil " << pcard.car_id << " a la siguiente estación..." << std::endl;
        if (msgsnd(msgid_1, &msg, sizeof(msg.mtext), 0) < 0) {
            perror("[ESTACION 1] sending card");
            exit(1);
        }
    }

    return 0;
}
