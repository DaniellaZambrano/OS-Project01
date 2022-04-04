/**
 * @file station_4.cpp
 * @brief This code is the Station 4 of the problem.
 * @version 0.1
 * @date 2022-04-01
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
    std::cout << "[ESTACION 4] Creando estación\n";
    json config{ get_config() };

    // Create queues
    std::cout << "[ESTACION 4] Creando cadena de traslado entre estaciones 3 y 4\n";
    int msgid_3 = create_msg_queue(config["queues"]["cadena_3"]);

    std::cout << "[ESTACION 4] Creando cadena de información del supervisor\n";
    int supervisor_queue_id = create_msg_queue(config["queues"]["supervisor"]);

    std::normal_distribution<double> norm{ get_normal_dist_object(config["station_4"]["mean"], config["station_4"]["deviation"]) };

    std::cout << "[ESTACION 4] Valor de media: " << norm.mean() << std::endl;
    std::cout << "[ESTACION 4] Valor de desviacion estandar: " << norm.stddev() << std::endl;

    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    std::uniform_int_distribution<int> seat_dist{ 0, 1 };

    QueueMessage msg;
    while (true) {
        std::cout << "[ESTACION 4] Aguardando por vehículo en cola." << std::endl;

        ssize_t data = msgrcv(msgid_3, &msg, sizeof(msg.mtext), 1, 0);

        if (data < 0) {
            perror("[ESTACION 4] error receiving message");
            exit(1);
        }

        ProductionCard& pcard{ msg.mtext };

        std::chrono::duration<double> period(norm(generator));

        std::cout << "[ESTACION 4] Automovil " << pcard.car_id << " colocando muebles y demás componentes" << std::endl;
        std::cout << "[ESTACION 4] Tiempo estimado " << period.count() << std::endl;

        std::cout << "[ESTACION 4] Enviando automóvil " << pcard.car_id << " al supervisor..." << std::endl;
        pcard.station = 4;

        if (msgsnd(supervisor_queue_id, &msg, sizeof(msg.mtext), 0) < 0) {
            perror("[ESTACION 4] sending card to supervisor");
            exit(1);
        }

        std::this_thread::sleep_for(period);

        // Add car seat type
        pcard.seat_type = (CarSeatType)(seat_dist(generator));
        std::cout << "[ESTACION 4] El asiento del automovil " << pcard.car_id << " es: " << CAR_SEAT_TYPE_STR[pcard.seat_type] << std::endl;
    }

    return 0;
}