
#include <iostream>
#include <chrono>
#include <thread>
#include <unordered_map>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "production_card.hpp"

using json = nlohmann::json;
using namespace std::chrono_literals;

/**
 * @brief Returns the exponential distribution object based on the input lambda.
 *
 * @param lambda
 * @return double
 */
std::exponential_distribution<double> get_exponential_object(double lambda) {
    std::exponential_distribution<double> distribution(lambda);
    return distribution;
}

/**
 * @brief Returns the normal distribution object based on the mean m1 and the
 *      deviation d1
 *
 * @param m1
 * @param d1
 * @return std::normal_distribution<double>
 */
std::normal_distribution<double> get_normal_dist_object(double m1, double d1) {
    std::normal_distribution<double> distribution(m1, d1);
    return distribution;
}

/**
 * @brief This function will be executed concurrently to simulate
 * the arrival of new cars to be produced in the factory.
 *
 * @param exp
 */
void new_cars_simulator(std::exponential_distribution<double> exp_dist, int queue_id) {
    int seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    while (true) {
        QueueMessage msg;
        std::cout << "[ESTACION 0] Asignando nuevo vehículo para producción" << std::endl;

        if (msgsnd(queue_id, &msg, sizeof(msg.mtext), 0) < 0) {
            perror("[ESTACION 0] error sending car");
            exit(0);
        }

        std::chrono::duration<double> period(exp_dist(generator));
        std::cout << "[ESTACION 0] Tiempo estimado para nueva llegada: " << period.count() << std::endl;

        std::this_thread::sleep_for(period);
    }
}

/**
 * @brief This function will create a new message queue
 *
 *
 * @param queue_name is the char used to generate the key to communicate the queue
 * @return msg_id : ID of queue
 */
int create_msg_queue(std::string queue_name) {
    key_t key{ ftok(".keyfile", queue_name[0]) };

    int msg_id;
    if ((msg_id = msgget(key, 0666 | IPC_CREAT)) < 0) {
        perror("creating msg queue");
        exit(1);
    }

    return msg_id;
}

void delete_queue(int queue_id){
    msgctl(queue_id,IPC_RMID,NULL);
}


json get_config() {
    std::ifstream i("params.json");
    json config;
    i >> config;

    return std::move(config);
}

#define KNRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GREEN  "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CYAN  "\x1B[36m"
