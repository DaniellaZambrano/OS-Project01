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

using json = nlohmann::json;
using namespace std::chrono_literals;

int main()
{
    // Read parameters file
    std::cout << "[ESTACION 3] Creando estación 3\n";
    std::ifstream i("params.json");
    json config;
    i >> config;

    // Create queue for CADENA_2
    std::cout << "[ESTACION 3] Creando cadena de traslado entre estaciones 2 y 3\n";
    std::string queue_name_2 = config["queues"]["cadena_2"];
    int msgid_2 = create_msg_queue(queue_name_2[0]);

    // Create queue for CADENA_3
    std::cout << "[ESTACION 3] Creando cadena de traslado entre estaciones 3 y 4\n";
    std::string queue_name_3 = config["queues"]["cadena_3"];
    int msgid_3 = create_msg_queue(queue_name_3[0]);

    std::cout << "[ESTACION 3] Creando cadena de información del supervisor\n";
    std::string supervisor_queue = config["queues"]["supervisor"];
    int supervisor_queue_id = create_msg_queue(supervisor_queue[0]);

    std::normal_distribution<double> norm = get_normal_dist_object(config["station_3"]["mean_3"], config["station_3"]["deviation_3"]);

    // POP cars from the queue
    std::cout << "[ESTACION 3] Valor de media M3: " << config["station_3"]["mean_3"] << std::endl;
    std::cout << "[ESTACION 3] Valor de desviacion estandard D3: " << config["station_3"]["deviation_3"] << std::endl;

    QueueMessage msg;
    while (true)
    {
        ssize_t data = msgrcv(msgid_2, &msg, sizeof(msg.mtext), 1, 0);
        if (data == 0)
        {
            std::cout << "[ESTACION 3] No hay vehículos en cola. " << std::endl;
            std::this_thread::sleep_for(200ms);
            continue;
        }

        if (data < 0)
        {
            perror("[ESTACION 3] error receiving message");
            exit(1);
        }

        ProductionCard &pcard{msg.mtext};

        int seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        double number = norm(generator);
        std::chrono::duration<double> period(number);

        std::cout << "[ESTACION 3] Automovil " << pcard.car_id << " colocando motor y cableado ..." << std::endl;
        std::cout << "[ESTACION 3] Tiempo estimado " << period.count() << std::endl;

        pcard.station = 3;
        if (msgsnd(supervisor_queue_id, &msg, sizeof(msg.mtext), 0) < 0)
        {
            perror("[ESTACION 3] sending card to supervisor");
            exit(1);
        }

        std::this_thread::sleep_for(period);

        // Add car bodywork
        pcard.motor_type = (MotorType)(seed % 3);
        std::cout << "[ESTACION 3] El motor del automovil " << pcard.car_bodywork << " es: " << MOTOR_TYPE_STR[pcard.motor_type] << std::endl;

        std::cout << "[ESTACION 3] Enviando automóvil " << pcard.car_id << " a la siguiente estación..." << std::endl;

        if (msgsnd(msgid_3, &msg, sizeof(msg.mtext), 0) < 0)
        {
            perror("[ESTACION 3] sending msg");
            exit(1);
        }
    }

    return 0;
}