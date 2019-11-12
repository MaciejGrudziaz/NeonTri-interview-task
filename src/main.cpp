#include <iostream>
#include "gtest/gtest.h"

using namespace std;

int main(int argc, char* argv[]) {
    std::cout<<"double max: "<<std::numeric_limits<double>::max()<<std::endl;

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}