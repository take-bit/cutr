include(cmake/jsoncpp.cmake)
include(cmake/redis-plus-plus.cmake)

set(CMAKE_PREFIX_PATH "/opt/drogon/lib/cmake")

find_package(Drogon REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)