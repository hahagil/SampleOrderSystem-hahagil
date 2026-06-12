#include <gtest/gtest.h>
#include "../../SampleOrderSystem/Controllers/ProductionController.h"

using Controller::ProductionController;

// ceil(qty / (yield * 0.9))

TEST(FormulaTest, TypicalCase) {
    // ceil(10 / (0.9 * 0.9)) = ceil(10 / 0.81) = ceil(12.35) = 13
    EXPECT_EQ(13, ProductionController::calcProductionQty(10, 0.9));
}

TEST(FormulaTest, PerfectYield) {
    // ceil(1 / (1.0 * 0.9)) = ceil(1.11) = 2
    EXPECT_EQ(2, ProductionController::calcProductionQty(1, 1.0));
}

TEST(FormulaTest, LowYield) {
    // ceil(20 / (0.5 * 0.9)) = ceil(20 / 0.45) = ceil(44.44) = 45
    EXPECT_EQ(45, ProductionController::calcProductionQty(20, 0.5));
}

TEST(FormulaTest, ExactDivision) {
    // ceil(9 / (1.0 * 0.9)) = ceil(10.0) = 10
    EXPECT_EQ(10, ProductionController::calcProductionQty(9, 1.0));
}
