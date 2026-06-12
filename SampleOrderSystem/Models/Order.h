#pragma once
#include <string>
#include <ctime>
#include "OrderStatus.h"

namespace Model {

struct Order {
    std::string orderId;
    std::string sampleId;
    int         quantity;
    OrderStatus status;
    std::time_t createdAt;
};

} // namespace Model
