add_library(
        MyWeather
        Launcher.h
        Launcher.cpp
        Parser.cpp
        Parser.h
        Exceptions.cpp
        Exceptions.h
        Utils.h
)

target_link_libraries(
        MyWeather
        PUBLIC cpr::cpr
        rapidxml
        PUBLIC ftxui::screen
        PUBLIC ftxui::dom
        PUBLIC ftxui::component
        PUBLIC nlohmann_json::nlohmann_json
)

target_include_directories(MyWeather PUBLIC ${PROJECT_SOURCE_DIR})
