#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/component/mouse.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include "cpr/cpr.h"

#include "Parser.h"

#include <functional>
#include <thread>
#include <chrono>
#include <conio.h>
#include <windows.h>

const std::string kXApiKey = "X-Api-Key";
const std::string kName = "name";
const std::string kCountry = "country";
const std::string kLatitude = "latitude";
const std::string kLongitude = "longitude";
const std::string kForecastDays = "forecast_days";
const std::string kMaxDays = "15";
const std::string kTimezone = "timezone";
const std::string kHourly = "hourly";
const std::string kGMT = "GMT";
const std::string kHourlyParameters = "temperature_2m,"
                                      "visibility,"
                                      "cloudcover,"
                                      "windspeed_10m,"
                                      "weathercode,"
                                      "precipitation,"
                                      "is_day";

double kInvalidCoordinate = -1;

const std::string kToken = "J2gkci6Lro7lRWgL0leRGw==iQ6S02c82SDwkkH0";
const std::string kCityRequestURL = "https://api.api-ninjas.com/v1/city";
const std::string kWeatherRequestURL = "https://api.open-meteo.com/v1/forecast";

const uint16_t kRequestSuccessCode = 200;

struct Coordinates {
    double latitude;
    double longitude;
};

struct City {
    std::string city_name;
    std::string country;

    bool operator<(const City& city) const {return city_name < city.city_name;}
};

class Launcher {
 public:
    void Initialization();
    void Run();

    void GetWeatherInCity();
    void GetCityCoords();
    std::string GetDate(uint64_t current_day);
    Utils::WeatherType GetWeatherType(uint64_t time, bool is_night);

    void UpdateTimer();
    void UpdateApp();

    void PrintWeatherInCity();
    void PrintWeatherInCurrentDay(uint64_t current_day);

    static Coordinates JsonToCoordinates(const nlohmann::json& data);

 private:
    std::vector<City> cities_ = {};
    uint64_t number_of_days_;
    uint64_t update_frequency;

    uint64_t current_city_index_;

    uint64_t time_;

    bool escape_pressed_ = false;

    std::map<City, nlohmann::json> city_to_weather_;
    std::map<City, Coordinates> city_to_coords_;
};
