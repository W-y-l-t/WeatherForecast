#include <iostream>
#include <cstdint>
#include <utility>
#include <vector>
#include <map>

namespace Utils {
    enum PartOfTheDay {
        Morning = 6,
        Day = 12,
        Evening = 18,
        Night = 23
    };

    enum DayOfWeek {
        Saturday = 0,
        Sunday = 1,
        Monday = 2,
        Tuesday = 3,
        Wednesday = 4,
        Thursday = 5,
        Friday = 6
    };

    enum Month {
        January = 1,
        February = 4,
        March = 4,
        April = 0,
        May = 2,
        June = 5,
        July = 0,
        August = 3,
        September = 6,
        October = 1,
        November = 4,
        December = 6
    };

    struct Date {
        uint64_t day;
        uint64_t month;
        uint64_t year;
        uint64_t day_of_week;
    };

    const std::vector<Month> months_ = {Month::January, Month::February,
                                        Month::March, Month::April, Month::May,
                                        Month::June, Month::July, Month::August,
                                        Month::September, Month::October,
                                        Month::November, Month::December};

    enum class WeatherType {
        Sunny,
        Lunary,
        Cloudy,
        Foggy,
        Drizzly,
        Rainy,
        Snowy,
        UndefinedWeatherType
    };

    const std::map<WeatherType, std::vector<std::string>> weather_images = {
        {   WeatherType::Sunny,
            {
                "     \\   /     ",
                "    \\ .-. /    ",
                "  ~~ (   ) ~~  ",
                "    / \"-\" \\    ",
                "     /   \\     "
            }
        },
        {   WeatherType::Lunary,
            {
                "      _.._     ",
                "   .' .-'``    ",
                "  /  /         ",
                "  \\  \\         ",
                "   '.___`_-`   "
            }
        },
        {   WeatherType::Cloudy,
            {
                "               ",
                "      .--.     ",
                "   .-(    ).   ",
                "  (___.__)__)  ",
                "               "
            }
        },
        {   WeatherType::Foggy,
            {
                "               ",
                "  _ - _ - _ -  ",
                "  _ - _ - _ -  ",
                "  _ - _ - _ -  ",
                "               "
            }
        },
        {   WeatherType::Drizzly,
            {
                "      .--.     ",
                "   .-(    ).   ",
                "  (___.__)__)  ",
                "    , ʻ ‚ ʻ    ",
                "     ‚   '     "
            }
        },
        {   WeatherType::Rainy,
            {
                "       .--.    ",
                "    .-(    ).  ",
                "   (___.__)__) ",
                "    ‚ʻ‚ʻ‚ʻ‚ʻ   ",
                "   ‚ʻ‚ʻ‚ʻ‚ʻ    "
            }
        },
        {   WeatherType::Snowy,
            {
                "       .--.    ",
                "    .-(    ).  ",
                "   (___.__)__) ",
                "     * * * *   ",
                "    * * * *    "
            }
        },
        {   WeatherType::UndefinedWeatherType,
            {
                "     _---_     ",
                "          )    ",
                "     (````     ",
                "      (_       ",
                "       *       "
            }
        }
    };
}
