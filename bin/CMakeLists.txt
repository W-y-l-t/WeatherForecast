add_executable(${PROJECT_NAME} main.cpp
        ../lib/Utils.h
        ../lib/Utils.h)

target_link_libraries(${PROJECT_NAME} PRIVATE MyWeather)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

install(DIRECTORY tools/RapidXML DESTINATION include)
