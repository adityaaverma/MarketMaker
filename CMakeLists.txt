cmake_minimum_required(VERSION 3.16)
project(MarketMaker CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_executable(market_maker
    source/main.cpp
    source/PriceSimulator.cpp
    source/NaiveQuoter.cpp
    source/OrderArrivalModel.cpp
)

target_include_directories(market_maker PRIVATE include)
target_compile_options(market_maker PRIVATE -Wall -Wextra)
