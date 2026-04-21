cmake_minimum_required(VERSION 3.10)
project(BookstoreManagement)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    account.cpp
    book.cpp
    log.cpp
    utils.cpp
)

set(HEADERS
    account.hpp
    book.hpp
    log.hpp
    utils.hpp
)

add_executable(code ${SOURCES} ${HEADERS})