
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

static void stop(int unused){
        // Signal managment to stop the process 
    kill(getpid(), SIGINT);

	std::cout << RED <<"[SUPERVISOR] Fabrica detenida.............\n";

    raise(SIGKILL);
}


int main() {
    // Signal managment to stop the process 
    signal(SIGINT,stop);

    // Read parameters file
    std::cout << "[SUPERVISOR] Creando supervisor" << std::endl;
    std::ifstream i("params.json");
    json config;
    i >> config;

    // Create queue for SUPERVISOR
    std::cout << "[SUPERVISOR] Creando cola de arrivo de vehículos de las estaciones" << std::endl;
    int queue_id = create_msg_queue(config["queues"]["supervisor"]);

    QueueMessage msg;
    while (true) {
        ssize_t data = msgrcv(queue_id, &msg, sizeof(msg.mtext), 1, 0);

        if (data < 0) {
            perror("[SUPERVISOR] error receiving message");
            kill(getpid(), SIGINT);
            exit(1);
        }

        ProductionCard& pcard{ msg.mtext };

        if (!pcard.finalizado) {
            std::cout << "[SUPERVISOR][ESTACION " << pcard.station << "] procesando automóvil: " << pcard.car_id << std::endl;
        }
        else {
            std::cout << "[SUPERVISOR][ESTACION " << pcard.station << "] finalizó automóvil: " << pcard.car_id << std::endl;
            std::cout << "\tid: " << pcard.car_id << std::endl;
            std::cout << "\tcolor: " << CAR_COLORS_STR[pcard.color] << std::endl;
            std::cout << "\tbody work: " << CAR_BODYWORK_STR[pcard.car_bodywork] << std::endl;
            std::cout << "\tmotor: " << MOTOR_TYPE_STR[pcard.motor_type] << std::endl;
            std::cout << "\tasientos: " << CAR_SEAT_TYPE_STR[pcard.seat_type] << std::endl;
        }
    }

    return 0;
}