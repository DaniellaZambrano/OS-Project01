#include <iostream>
#include "../production_card.cpp"

int main() {
    std::cout << "Testing Production Card" << std::endl;
    
    ProductionCard card;
    card.car_id = 1;
    card.car_bodywork = sedan;
    card.color = red;
    card.motor_type = gasoline;
    card.seat_type = leather;

    std::cout << "Card ID: " << card.car_id << std::endl;
    std::cout << "Card color: " << CAR_COLORS_STR[card.color] << std::endl;
    std::cout << "Card Bodywork: " << CAR_BODYWORK_STR[card.car_bodywork] << std::endl;
    std::cout << "Motor Type: " << MOTOR_TYPE_STR[card.motor_type] << std::endl;
    std::cout << "Seat type: " << CAR_SEAT_TYPE_STR[card.seat_type] << std::endl;

    return 0;
}
