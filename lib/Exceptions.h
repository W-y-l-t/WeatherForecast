#include "Utils.h"

class MyExceptions : std::exception {
 public:
    MyExceptions();
    explicit MyExceptions(const char* new_message);

    [[nodiscard]] const char* what() const noexcept override {
        return message_;
    }

    static void InvalidConfig();

    static void GetCoordsRequestError(const std::string& e);

    static void GetWeatherRequestError(const std::string& e);

 private:
    const char* message_;
};
