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

using json = nlohmann::json;
using namespace std::chrono_literals;

int main()
{
    std::cout << "[ESTACION 4] Creando estación\n";
    std::ifstream i("params.json");
    json config;
    i >> config;

    // Create queue for CADENA_3
    std::cout << "[ESTACION 4] Creando cadena de traslado entre estaciones 3 y 4\n";
    std::string queue_name_3 = config["queues"]["cadena_3"];
    int msgid_3 = create_msg_queue(queue_name_3[0]);

    std::cout << "[ESTACION 4] Creando cadena de información del supervisor\n";
    std::string supervisor_queue = config["queues"]["supervisor"];
    int supervisor_queue_id = create_msg_queue(supervisor_queue[0]);

    double mean{config["station_4"]["mean"]}, deviation{config["station_4"]["deviation"]};
    std::normal_distribution<double> norm{get_normal_dist_object(mean, deviation)};

    std::cout << "[ESTACION 4] Valor de media: " << mean << std::endl;
    std::cout << "[ESTACION 4] Valor de desviacion estandar: " << deviation << std::endl;

    ProductionCard pcard;

    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    std::uniform_int_distribution<int> seat_dist{0, 1};

    while (true)
    {
        std::cout << "[ESTACION 4] Aguardando por vehículo en cola." << std::endl;

        ssize_t data = msgrcv(msgid_3, &pcard, sizeof(pcard), 1, 0);
        if (data == 0)
        {
            std::cout << "[ESTACION 4] No hay vehículos en cola. " << std::endl;
            std::this_thread::sleep_for(200ms);
            continue;
        }

        if (data < 0)
        {
            perror("[ESTACION 4] error receiving message");
            exit(1);
        }

        std::chrono::duration<double> period(norm(generator));

        std::cout << "[ESTACION 4] Automovil " << pcard.car_id << " colocando muebles y demás componentes" << std::endl;
        std::cout << "[ESTACION 4] Tiempo estimado " << period.count() << std::endl;

        pcard.station = 4;
        if (msgsnd(supervisor_queue_id, &pcard, sizeof(pcard), 0) < 0)
        {
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