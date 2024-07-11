#include "Exceptions.h"

MyExceptions::MyExceptions() {
    this->message_ = "Unpredictable error!";
}

MyExceptions::MyExceptions(const char *new_message) {
    this->message_ = new_message;
}

void MyExceptions::InvalidConfig() {
    MyExceptions exception("Invalid config data!\n");

    std::cerr << exception.what();
    exit(EXIT_FAILURE);
}

void MyExceptions::GetCoordsRequestError(const std::string& e) {
    std::string s = "An error occurred while "
                    "getting the coordinates of the city " + e + "!\n";
    MyExceptions exception(s.c_str());

    std::cerr << exception.what();
    exit(EXIT_FAILURE);
}

void MyExceptions::GetWeatherRequestError(const std::string &e) {
    std::string s = "An error occurred while "
                    "getting the weather in the city " + e + "!\n";
    MyExceptions exception(s.c_str());

    std::cerr << exception.what();
    exit(EXIT_FAILURE);
}
