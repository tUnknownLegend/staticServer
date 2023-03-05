cmake_minimum_required (VERSION 3.2)

project ("StaticServer")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-Wall -Wpedantic -Wextra -Werror -lm -O0")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-lm -O0 -fcoroutines")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -levent")


find_path     (LIBEVENT_INCLUDE_DIR NAMES event.h)
find_library  (LIBEVENT_LIBRARY     NAMES event)
find_library  (LIBEVENT_CORE        NAMES event_core)
find_library  (LIBEVENT_EXTRA       NAMES event_extra)
if (NOT EVHTP_DISABLE_EVTHR)
    find_library (LIBEVENT_THREAD   NAMES event_pthreads)
endif()
if (NOT EVHTP_DISABLE_SSL)
    find_library (LIBEVENT_SSL      NAMES event_openssl)
endif()
include (FindPackageHandleStandardArgs)
set (LIBEVENT_INCLUDE_DIRS ${LIBEVENT_INCLUDE_DIR})
set (LIBEVENT_LIBRARIES
        ${LIBEVENT_LIBRARY}
        ${LIBEVENT_SSL}
        ${LIBEVENT_CORE}
        ${LIBEVENT_EXTRA}
        ${LIBEVENT_THREAD}
        ${LIBEVENT_EXTRA})
find_package_handle_standard_args (LIBEVENT DEFAULT_MSG LIBEVENT_LIBRARIES LIBEVENT_INCLUDE_DIR)
mark_as_advanced(LIBEVENT_INCLUDE_DIRS LIBEVENT_LIBRARIES)

include_directories(./src ./include)

file(GLOB SOURCES *.cpp *.h)

set(SOURCES ./src/main.cpp ./src/server.cpp)
set(HEADERS ./include/server.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${LIBEVENT_LIBRARIES})
#target_compile_options(${PROJECT_NAME} PUBLIC "-fcoroutines")

