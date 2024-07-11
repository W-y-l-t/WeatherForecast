#include "rapidxml/rapidxml.hpp"
#include "nlohmann/json.hpp"

#include "Exceptions.h"

#include <fstream>
#include <string>
#include <vector>

const std::string kConfigRootName = "MyWeatherConfiguration";
const std::string kConfigListOfCitiesName = "ListOfCities";
const std::string kCity = "City";
const std::string kConfigNumberOfDaysName = "NumberOfDays";
const std::string kConfigUpdateFrequencyName = "UpdateFrequency";

class Parser {
 public:
    Parser();
    explicit Parser(std::string config_path);

    void Parse();


    std::string config_path_;
    std::vector<std::pair<std::string, std::string>> cities_;
    uint64_t number_of_days_;
    uint64_t update_frequency_;
};
