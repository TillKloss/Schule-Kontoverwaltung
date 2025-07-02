cmake_minimum_required(VERSION 3.30)
project(Kontoverwaltung C)

set(CMAKE_C_STANDARD 11)

set(MYSQL_CONNECTOR_DIR "${CMAKE_SOURCE_DIR}/mysql-connector")

include_directories(${MYSQL_CONNECTOR_DIR}/include)
link_directories(${MYSQL_CONNECTOR_DIR}/lib)

add_executable(Kontoverwaltung
        src/main.c
        src/database.c
        src/terminal.c
        src/include/database.h
        src/include/terminal.h
        src/utils.c
        src/include/utils.h
)

target_link_libraries(Kontoverwaltung mysql)

add_custom_command(TARGET Kontoverwaltung POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${MYSQL_CONNECTOR_DIR}/lib/libmysql.dll"
        $<TARGET_FILE_DIR:Kontoverwaltung>
)
