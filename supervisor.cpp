
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
    std::cout << "[SUPERVISOR] Creando supervisor\n";
    json config{ get_config() };

    // Create queue for SUPERVISOR
    std::cout << "[SUPERVISOR] Creando cola de arrivo de vehículos de las estaciones\n";
    int queue_id = create_msg_queue(config["queues"]["supervisor"]);

    QueueMessage msg;
    while (true) {
        ssize_t data = msgrcv(queue_id, &msg, sizeof(msg.mtext), 1, 0);

        if (data < 0) {
            perror("[SUPERVISOR] error receiving message");
            exit(1);
        }

        ProductionCard& pcard{ msg.mtext };

        std::cout << "[SUPERVISOR][ESTACION " << pcard.station << "] procesando vehículo: " << pcard.car_id << std::endl;
    }

    return 0;
}