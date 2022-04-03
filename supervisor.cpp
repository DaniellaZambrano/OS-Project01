
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
    std::cout << "[SUPERVISOR] Creando supervisor\n";
    std::ifstream i("params.json");
    json config;
    i >> config;

    // Create queue for CADENA_0
    std::cout << "[SUPERVISOR] Creando cola de arrivo de vehículos de las estaciones\n";
    std::string queue_name = config["queues"]["supervisor"];
    int msgid = create_msg_queue(queue_name[0]);

    ProductionCard pcard;
    while (true)
    {
        std::cout << "[SUPERVISOR] Aguardando por estación" << std::endl;

        ssize_t data = msgrcv(msgid, &pcard, sizeof(pcard), 1, 0);

        if (data < 0)
        {
            perror("[SUPERVISOR] error receiving message");
            exit(1);
        }

        std::cout << "[SUPERVISOR] Estación: " << pcard.station << ": procesando vehículo: " << pcard.car_id << std::endl;
    }

    return 0;
}