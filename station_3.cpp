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
#include <signal.h>
#include <unistd.h>

#include "json.hpp"
#include "production_card.hpp"
#include "utilities.hpp"

int msgid_2, msgid_3;

static void stop(int unused){
	std::cout << RED <<"[ESTACION 3] Deteniendo proceso de la estacion 3.............\n";

    delete_queue(msgid_2);
    delete_queue(msgid_3);

    raise(SIGKILL);
}


int main() {
        // Signal managment to stop the process 
    signal(SIGINT,(__sighandler_t)stop);

    // Read parameters file
    std::cout << CYAN << "[ESTACION 3] Creando estación" << std::endl;
    std::ifstream i("params.json");
    json config;
    i >> config;

    // Create queue for CADENA_2
    std::cout << "[ESTACION 3] Creando cadena de traslado entre estaciones 2 y 3" << std::endl;
    msgid_2 = create_msg_queue(config["queues"]["cadena_2"]);

    // Create queue for CADENA_3
    std::cout << "[ESTACION 3] Creando cadena de traslado entre estaciones 3 y 4" << std::endl;
    msgid_3 = create_msg_queue(config["queues"]["cadena_3"]);

    // Create queue for SUPERVISOR
    std::cout << "[ESTACION 3] Creando cadena de información del supervisor" << std::endl;
    int supervisor_queue_id = create_msg_queue(config["queues"]["supervisor"]);

    // POP cars from the queue
    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    std::normal_distribution<double> norm = get_normal_dist_object(config["station_3"]["mean"], config["station_3"]["deviation"]);
    std::cout << "[ESTACION 3] Valor de media: " << norm.mean() << std::endl;
    std::cout << "[ESTACION 3] Valor de desviación estandar: " << norm.stddev() << std::endl;

    std::uniform_int_distribution<int> motor_dist{ 0, 2 };

    QueueMessage msg;
    while (true) {
        ssize_t data = msgrcv(msgid_2, &msg, sizeof(msg.mtext), 1, 0);

        if (data < 0) {
            perror("[ESTACION 3] error receiving message");
            sigqueue(getpid(), SIGINT,(union sigval){.sival_ptr = NULL });
            exit(1);
        }
        ProductionCard& pcard{ msg.mtext };

        std::cout << "[ESTACION 3] ----- Nuevo automóvil: " << pcard.car_id << " -----" << std::endl;

        std::chrono::duration<double> period(norm(generator));

        std::cout << "[ESTACION 3] Notificando al supervisor." << std::endl;
        pcard.station = 3;
        if (msgsnd(supervisor_queue_id, &msg, sizeof(msg.mtext), 0) < 0) {
            perror("[ESTACION 3] sending card to supervisor");
            sigqueue(getpid(), SIGINT,(union sigval){.sival_ptr = NULL });
            exit(1);
        }

        std::cout << "[ESTACION 3] automóvil entrando a colocado de motor y cableado" << std::endl;
        std::cout << "[ESTACION 3] Tiempo estimado: " << period.count() << std::endl;

        std::this_thread::sleep_for(period);

        // Add car bodywork
        pcard.motor_type = (MotorType)(motor_dist(generator));
        std::cout << "[ESTACION 3] motor asignado al automóvil: " << MOTOR_TYPE_STR[pcard.motor_type] << std::endl;

        std::cout << "[ESTACION 3] Enviando automóvil a la siguiente estación..." << std::endl;
        if (msgsnd(msgid_3, &msg, sizeof(msg.mtext), 0) < 0) {
            perror("[ESTACION 3] sending msg");
            sigqueue(getpid(), SIGINT,(union sigval){.sival_ptr = NULL });
            exit(1);
        }
    }

    return 0;
}