/**
 * @file station_3.cpp
 * @brief This code is the Station 3 of the problem.
 * @version 0.1
 * @date 2022-03-31
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
    std::cout << "[ESTACION 3] Creando estación 3\n";
    json config{ get_config() };

    // Create queue for CADENA_2
    std::cout << "[ESTACION 3] Creando cadena de traslado entre estaciones 2 y 3\n";
    int msgid_2 = create_msg_queue(config["queues"]["cadena_2"]);

    // Create queue for CADENA_3
    std::cout << "[ESTACION 3] Creando cadena de traslado entre estaciones 3 y 4\n";
    int msgid_3 = create_msg_queue(config["queues"]["cadena_3"]);

    std::cout << "[ESTACION 3] Creando cadena de información del supervisor\n";
    int supervisor_queue_id = create_msg_queue(config["queues"]["supervisor"]);

    std::normal_distribution<double> norm = get_normal_dist_object(config["station_3"]["mean"], config["station_3"]["deviation"]);

    // POP cars from the queue
    std::cout << "[ESTACION 3] Valor de media: " << norm.mean() << std::endl;
    std::cout << "[ESTACION 3] Valor de desviación estandar: " << norm.stddev() << std::endl;

    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    std::uniform_int_distribution<int> motor_dist{ 0, 2 };

    QueueMessage msg;
    while (true) {
        ssize_t data = msgrcv(msgid_2, &msg, sizeof(msg.mtext), 1, 0);

        if (data < 0) {
            perror("[ESTACION 3] error receiving message");
            exit(1);
        }

        ProductionCard& pcard{ msg.mtext };

        std::chrono::duration<double> period(norm(generator));

        std::cout << "[ESTACION 3] Automovil " << pcard.car_id << " colocando motor y cableado ..." << std::endl;
        std::cout << "[ESTACION 3] Tiempo estimado " << period.count() << std::endl;

        std::cout << "[ESTACION 3] Enviando automóvil " << pcard.car_id << " al supervisor..." << std::endl;
        pcard.station = 3;
        if (msgsnd(supervisor_queue_id, &msg, sizeof(msg.mtext), 0) < 0) {
            perror("[ESTACION 3] sending card to supervisor");
            exit(1);
        }

        std::this_thread::sleep_for(period);

        // Add car bodywork
        pcard.motor_type = (MotorType)(motor_dist(generator));
        std::cout << "[ESTACION 3] El motor del automovil " << pcard.car_bodywork << " es: " << MOTOR_TYPE_STR[pcard.motor_type] << std::endl;

        std::cout << "[ESTACION 3] Enviando automóvil " << pcard.car_id << " a la siguiente estación..." << std::endl;
        if (msgsnd(msgid_3, &msg, sizeof(msg.mtext), 0) < 0) {
            perror("[ESTACION 3] sending msg");
            exit(1);
        }
    }

    return 0;
}