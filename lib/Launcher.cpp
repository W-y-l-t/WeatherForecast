#include "Launcher.h"

void Launcher::Initialization() {
    Parser parser;
    parser.Parse();

    for (auto& city : parser.cities_) {
        this->cities_.emplace_back(city.first, city.second);
    }
    this->number_of_days_ = parser.number_of_days_;
    this->update_frequency = parser.update_frequency_;
}

void Launcher::UpdateTimer() {
    std::thread([this]() {
      while (true) {
          if (this->escape_pressed_) {
              break;
          }

          UpdateApp();

          std::this_thread::sleep_for(
              std::chrono::minutes(this->update_frequency)
          );
      }
    }).detach();
}

void Launcher::UpdateApp() {
    auto ClearTerminal = [](){
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif

        #ifdef _LINUX__
            std::cout<< u8"\033[2J\033[1;1H";
        #endif
    };
    ClearTerminal();

    GetWeatherInCity();

    PrintWeatherInCity();
}

void Launcher::Run() {
    Initialization();

    UpdateTimer();

    ftxui::ScreenInteractive terminal =
        ftxui::ScreenInteractive::TerminalOutput();

    std::shared_ptr<ftxui::ComponentBase> render = ftxui::Renderer([]
        { return ftxui::text("");
    });

    std::shared_ptr<ftxui::ComponentBase> button_catcher =
        ftxui::CatchEvent(render, [&](const ftxui::Event& event) {

            if (event == ftxui::Event::Character('q')) {
                this->escape_pressed_ = true;
                terminal.Exit();

            } else if (event == ftxui::Event::Character('p')) {
                if (this->current_city_index_ == 0) {
                    this->current_city_index_ = this->cities_.size() - 1;
                } else {
                    this->current_city_index_ -= 1;
                }

                UpdateApp();

            } else if (event == ftxui::Event::Character('n')) {
                if (this->current_city_index_ == this->cities_.size() - 1) {
                    this->current_city_index_ = 0;
                } else {
                    this->current_city_index_ += 1;
                }
                UpdateApp();

            } else if (event == ftxui::Event::Character('+')) {
                this->number_of_days_ += 1;
                this->number_of_days_ = std::min(this->number_of_days_, 16ULL);
                UpdateApp();

            } else if (event == ftxui::Event::Character('-')) {
                this->number_of_days_ -= 1;
                this->number_of_days_ = std::max(this->number_of_days_, 0ULL);
                UpdateApp();
            }

            return false;
        });

    terminal.Loop(button_catcher);
}

void Launcher::GetWeatherInCity() {
    City city = this->cities_[this->current_city_index_];

    if (!this->city_to_coords_.contains(city)) {
        GetCityCoords();
    }

    cpr::Response weather_in_city_data = cpr::Get(
        cpr::Url{kWeatherRequestURL},
        cpr::Parameters{
            {kLatitude, std::to_string(
                city_to_coords_[city].latitude
                )},
            {kLongitude, std::to_string(
                city_to_coords_[city].longitude
                )},
            {kForecastDays, kMaxDays},
            {kTimezone, kGMT},
            {kHourly, kHourlyParameters},
        }
    );

    if (weather_in_city_data.status_code != kRequestSuccessCode) {
        MyExceptions::GetWeatherRequestError(city.city_name);
    }

    this->city_to_weather_[city] =
        nlohmann::json ::parse(weather_in_city_data.text);
}

void Launcher::GetCityCoords() {
    City city = this->cities_[this->current_city_index_];

    cpr::Response city_data = cpr::Get(
        cpr::Url{kCityRequestURL},
        cpr::Header{{kXApiKey, kToken}},
        cpr::Parameters{{kName, city.city_name},
                        {kCountry, city.country}}
    );

    if (city_data.status_code != kRequestSuccessCode) {
        MyExceptions::GetCoordsRequestError(city.city_name);
    }

    Coordinates coords =
        JsonToCoordinates(nlohmann::json::parse(city_data.text));
    if (coords.longitude == kInvalidCoordinate ||
        coords.latitude == kInvalidCoordinate) {

        MyExceptions::GetCoordsRequestError(city.city_name);
    }

    this->city_to_coords_[city] = coords;
}

Coordinates Launcher::JsonToCoordinates(const nlohmann::json& data) {
    if (data.empty()) {
        return {kInvalidCoordinate, kInvalidCoordinate};
    }

    nlohmann::json data_map = data.get<std::vector<nlohmann::json>>()[0];

    auto latitude = data_map[kLatitude].get<double>();
    auto longitude = data_map[kLongitude].get<double>();

    return {latitude, longitude};
}

void Launcher::PrintWeatherInCity() {
    using namespace ftxui;

    Element out =
        vbox({
            hbox({
                center(text("MyWeather v.0.1.0"))
                | flex
                | color(Color::Orange1)
            }),
            separator(),

            center(hbox({
                (text(
                    this->cities_[this->current_city_index_].city_name +
                    ", " +
                    this->cities_[this->current_city_index_].country
                    )) | flex,
                })) | borderEmpty,
        }) | border;

    auto screen = Screen::Create(
        Dimension::Full(),
        Dimension::Fit(out)
    );
    Render(screen, out);
    screen.Print();

    for (size_t i = 0; i < this->number_of_days_; ++i) {
        PrintWeatherInCurrentDay(i);
    }
}

void Launcher::PrintWeatherInCurrentDay(uint64_t current_day) {
    using namespace ftxui;

    uint64_t offset = 24 * current_day;

    std::vector<uint64_t> day_parts = {offset + Utils::PartOfTheDay::Morning,
                                       offset + Utils::PartOfTheDay::Day,
                                       offset + Utils::PartOfTheDay::Evening,
                                       offset + Utils::PartOfTheDay::Night};

    std::string date = GetDate(current_day);

    std::vector<std::vector<std::string>> weather;
    std::vector<Utils::WeatherType> weather_type;
    for (auto now : day_parts) {
        std::stringstream buff;
        buff.precision(1);
        buff << std::fixed;
        buff << this->city_to_weather_[this->cities_[this->current_city_index_]]
            ["hourly"]["temperature_2m"].get<std::vector<double>>()[now];
        std::string temperature = buff.str() + "°C";

        std::string visibility = std::to_string(this->city_to_weather_
            [this->cities_[this->current_city_index_]]
            ["hourly"]["visibility"].get<std::vector<int32_t>>()[now]);
        visibility += "m";

        std::string cloud_cover = "☁ ";
        cloud_cover += std::to_string(this->city_to_weather_
            [this->cities_[this->current_city_index_]]
            ["hourly"]["cloudcover"].get<std::vector<int32_t>>()[now]);
        cloud_cover += "%";

        std::string wind_speed = std::to_string(this->city_to_weather_
            [this->cities_[this->current_city_index_]]
            ["hourly"]["windspeed_10m"].get<std::vector<int32_t>>()[now]);
        wind_speed += "km/h";

        std::string precipitation = std::to_string(this->city_to_weather_
            [this->cities_[this->current_city_index_]]
            ["hourly"]["visibility"].get<std::vector<int32_t>>()[now]);
        precipitation += "mm";

        weather.push_back({temperature, visibility, cloud_cover,
                           wind_speed, precipitation});
        weather_type.push_back(GetWeatherType(now, (now == day_parts[3])));
    }

    Element out =
        vbox({
            hbox({
                center(text(date)) | flex
            }) | borderEmpty,
            hbox({
                center(vbox({
                    center(text("Morning")),
                    separator(),
                    text(Utils::weather_images.at(weather_type[0])[0] + "Temperature:   " + weather[0][0]),
                    text(Utils::weather_images.at(weather_type[0])[1] + "Visibility:    " + weather[0][1]),
                    text(Utils::weather_images.at(weather_type[0])[2] + "Cloud cover:   " + weather[0][2]),
                    text(Utils::weather_images.at(weather_type[0])[3] + "Wind speed:    " + weather[0][3]),
                    text(Utils::weather_images.at(weather_type[0])[4] + "Precipitation: " + weather[0][4])
                })) | flex,
                separator(),

                center(vbox({
                    center(text("Day")),
                    separator(),
                    text(Utils::weather_images.at(weather_type[1])[0] + "Temperature:   " + weather[1][0]),
                    text(Utils::weather_images.at(weather_type[1])[1] + "Visibility:    " + weather[1][1]),
                    text(Utils::weather_images.at(weather_type[1])[2] + "Cloud cover:   " + weather[1][2]),
                    text(Utils::weather_images.at(weather_type[1])[3] + "Wind speed:    " + weather[1][3]),
                    text(Utils::weather_images.at(weather_type[1])[4] + "Precipitation: " + weather[1][4])
                })) | flex,
                separator(),

                center(vbox({
                    center(text("Evening")),
                    separator(),
                    text(Utils::weather_images.at(weather_type[2])[0] + "Temperature:   " + weather[2][0]),
                    text(Utils::weather_images.at(weather_type[2])[1] + "Visibility:    " + weather[2][1]),
                    text(Utils::weather_images.at(weather_type[2])[2] + "Cloud cover:   " + weather[2][2]),
                    text(Utils::weather_images.at(weather_type[2])[3] + "Wind speed:    " + weather[2][3]),
                    text(Utils::weather_images.at(weather_type[2])[4] + "Precipitation: " + weather[2][4])
                })) | flex,
                separator(),

                center(vbox({
                    center(text("Night")),
                    separator(),
                    text(Utils::weather_images.at(weather_type[3])[0] + "Temperature:   " + weather[3][0]),
                    text(Utils::weather_images.at(weather_type[3])[1] + "Visibility:    " + weather[3][1]),
                    text(Utils::weather_images.at(weather_type[3])[2] + "Cloud cover:   " + weather[3][2]),
                    text(Utils::weather_images.at(weather_type[3])[3] + "Wind speed:    " + weather[3][3]),
                    text(Utils::weather_images.at(weather_type[3])[4] + "Precipitation: " + weather[3][4])
                })) | flex,
            }) | border,
        });

    auto screen = Screen::Create(
        Dimension::Full(),
        Dimension::Fit(out)
    );
    Render(screen, out);
    screen.Print();
}

std::string Launcher::GetDate(uint64_t current_day) {
    uint64_t offset = current_day * 24;

    Utils::Date current_date{};

    std::string date_in_string =
        this->city_to_weather_[this->cities_[this->current_city_index_]]
        ["hourly"]["time"].get<std::vector<std::string>>()[offset];

    current_date.day = (uint64_t)(date_in_string[8] - '0') * 10 +
                       (uint64_t)(date_in_string[9] - '0');
    current_date.month = (uint64_t)(date_in_string[5] - '0') * 10 +
                         (uint64_t)(date_in_string[6] - '0');
    current_date.year = (uint64_t)(date_in_string[0] - '0') * 1000 +
                        (uint64_t)(date_in_string[1] - '0') * 100 +
                        (uint64_t)(date_in_string[2] - '0') * 10 +
                        (uint64_t)(date_in_string[3] - '0');

    uint64_t code_of_the_month = Utils::months_[current_date.month - 1];
    uint64_t code_of_the_year =
        (6 + current_date.year % 100 + (current_date.year % 100) / 4) % 7;

    current_date.day_of_week =
        (current_date.day + code_of_the_month + code_of_the_year) % 7;

    std::string date_for_output;

    switch (current_date.day_of_week) {
        case 0:
            date_for_output = "Friday";
            break;
        case 1:
            date_for_output = "Saturday";
            break;
        case 2:
            date_for_output = "Sunday";
            break;
        case 3:
            date_for_output = "Monday";
            break;
        case 4:
            date_for_output = "Tuesday";
            break;
        case 5:
            date_for_output = "Wednesday";
            break;
        case 6:
            date_for_output = "Thursday";
            break;
        default:
            break;
    }
    date_for_output += " ";
    date_for_output += (current_date.month <= 9 ?
                        "0" + std::to_string(current_date.month) :
                        std::to_string(current_date.month));
    date_for_output += '.';
    date_for_output += (current_date.day <= 9 ?
                        "0" + std::to_string(current_date.day) :
                        std::to_string(current_date.day));
    date_for_output += '.';
    date_for_output += std::to_string(current_date.year);

    return date_for_output;
}

Utils::WeatherType Launcher::GetWeatherType(uint64_t time, bool is_night) {
    uint16_t weather_code =
        this->city_to_weather_[this->cities_[this->current_city_index_]]
        ["hourly"]["weathercode"].get<std::vector<uint64_t>>()[time];

    if (weather_code == 0) {
        if (!is_night) {
            return Utils::WeatherType::Sunny;
        }
        return Utils::WeatherType::Lunary;

    } else if (weather_code == 1 || weather_code == 2 || weather_code == 3) {
        return Utils::WeatherType::Cloudy;

    } else if (weather_code == 45 || weather_code == 48) {
        return Utils::WeatherType::Foggy;

    } else if (weather_code == 51 || weather_code == 53 || weather_code == 55 ||
               weather_code == 56 || weather_code == 57) {

        return Utils::WeatherType::Drizzly;

    } else if (weather_code == 61 || weather_code == 63 || weather_code == 65 ||
               weather_code == 66 || weather_code == 67 || weather_code == 80 ||
               weather_code == 81 || weather_code == 82) {

        return Utils::WeatherType::Rainy;

    } else if (weather_code == 71 || weather_code == 73 || weather_code == 75 ||
               weather_code == 77 || weather_code == 85 || weather_code == 86) {

        return Utils::WeatherType::Snowy;

    }

    return Utils::WeatherType::UndefinedWeatherType;
}
