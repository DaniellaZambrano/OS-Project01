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
    std::cout << "[ESTACION 2] Creando estación" << std::endl;
    json config{ get_config() };

    // Create queue for CADENA_1
    std::cout << "[ESTACION 2] Creando cadena de traslado entre estaciones 1 y 2" << std::endl;
    int msgid_1 = create_msg_queue(config["queues"]["cadena_1"]);

    // Create queue for CADENA_2
    std::cout << "[ESTACION 2] Creando cadena de traslado entre estaciones 2 y 3" << std::endl;
    int msgid_2 = create_msg_queue(config["queues"]["cadena_2"]);

    // Create queue for SUPERVISOR
    std::cout << "[ESTACION 2] Creando cadena de información del supervisor" << std::endl;
    int supervisor_queue_id = create_msg_queue(config["queues"]["supervisor"]);

    // POP cars from the queue
    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    std::normal_distribution<double> norm_dist = get_normal_dist_object(config["station_2"]["mean"], config["station_2"]["deviation"]);
    std::cout << "[ESTACION 2] Valor de media: " << norm_dist.mean() << std::endl;
    std::cout << "[ESTACION 2] Valor de desviación estandar: " << norm_dist.stddev() << std::endl;

    std::uniform_int_distribution<int>  bodywork_dist{ 0, 1 };

    CarColor color_counter = CarColor::red;

    QueueMessage msg;
    while (true) {
        ssize_t data = msgrcv(msgid_1, &msg, sizeof(msg.mtext), 1, 0);

        if (data < 0) {
            perror("[ESTACION 2] error receiving message");
            exit(1);
        }
        ProductionCard& pcard{ msg.mtext };

        std::cout << "[ESTACION 2] ----- Nuevo automóvil: " << pcard.car_id << " -----" << std::endl;

        std::chrono::duration<double> period(norm_dist(generator));

        std::cout << "[ESTACION 2] Notificando al supervisor..." << std::endl;
        pcard.station = 2;
        if (msgsnd(supervisor_queue_id, &msg, sizeof(msg.mtext), 0) < 0) {
            perror("[ESTACION 2] sending card to supervisor");
            exit(1);
        }

        std::cout << "[ESTACION 2] automóvil entrando a pintura y carroceria..." << std::endl;
        std::cout << "[ESTACION 2] Tiempo estimado: " << period.count() << std::endl;

        std::this_thread::sleep_for(period);

        // Add color to the car
        color_counter = (CarColor)(((int)(color_counter)+1) % 3);
        pcard.color = color_counter;
        std::cout << "[ESTACION 2] Color asignado al automóvil: " << CAR_COLORS_STR[pcard.color] << std::endl;

        // Add car bodywork
        pcard.car_bodywork = (CarBodywork)(bodywork_dist(generator));
        std::cout << "[ESTACION 2] Carroceria asignada al automóvil:" << CAR_BODYWORK_STR[pcard.car_bodywork] << std::endl;

        std::cout << "[ESTACION 2] Enviando automóvil a la siguiente estación..." << std::endl;
        if (msgsnd(msgid_2, &msg, sizeof(msg.mtext), 0) < 0) {
            perror("[ESTACION 2] sending msg");
            exit(1);
        }
    }

    return 0;
}