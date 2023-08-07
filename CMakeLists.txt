cmake_minimum_required(VERSION 3.2)

project(HW-sqlcpp-4)

set(PostgreSQL_ADDITIONAL_VERSIONS "15")

#add_subdirectory("D:\\source\\repos\\libpqxx" libpqxx-build)
add_subdirectory("H:\\ProjectsCpp\\libpqxx" libpqxx-build)

add_executable(HW-sqlcpp-4 main.h main.cpp DatabaseClients.h DatabaseClients.cpp)

target_compile_features(HW-sqlcpp-4 PRIVATE cxx_std_17)

target_link_libraries(HW-sqlcpp-4 pqxx)
