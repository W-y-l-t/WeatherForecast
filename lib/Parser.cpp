#include "Parser.h"

Parser::Parser() {
    this->config_path_ = R"(C:\ITMO\C++\labwork7-W-y-l-t\config.xml)";
    this->cities_ = {};
    this->number_of_days_ = 0;
    this->update_frequency_ = 0;
}

Parser::Parser(std::string config_path) {
    this->config_path_ = std::move(config_path);
    this->cities_ = {};
    this->number_of_days_ = 0;
    this->update_frequency_ = 0;
}

void Parser::Parse() {
    rapidxml::xml_document<char> doc;

    try {
        std::ifstream file(this->config_path_);
        std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
        buffer.push_back('\0');

        doc.parse<0>( &buffer[0]);
    } catch (rapidxml::parse_error &e) {
        std::cerr << "[Error] Parse error " << e.what() << '\n';
        exit(EXIT_FAILURE);
    }

    rapidxml::xml_node<>* current_node = doc.first_node();
    if (current_node->name() != kConfigRootName) {
        MyExceptions::InvalidConfig();
    }
    current_node = current_node->first_node();

    while (current_node) {
        if (current_node->name() == kConfigListOfCitiesName) {
            rapidxml::xml_node<>* city = current_node->first_node();

            if (!city) {
                MyExceptions::InvalidConfig();
            }

            while (city) {
                std::string city_string = city->value();

                size_t pos = city_string.find(',');
                if (pos == std::string::npos) {
                    MyExceptions::InvalidConfig();
                }

                std::string city_name = city_string.substr(0, pos);
                std::string country;
                country += city_string[city_string.size() - 2];
                country += city_string[city_string.size() - 1];

                this->cities_.emplace_back(city_name, country);

                city = city->next_sibling();
            }
        } else if (current_node->name() == kConfigNumberOfDaysName) {
            this->number_of_days_ = std::stoull(current_node->value());

        } else if (current_node->name() == kConfigUpdateFrequencyName) {
            this->update_frequency_ = std::stoull(current_node->value());

        }
        else {
            MyExceptions::InvalidConfig();
        }

        current_node = current_node->next_sibling();
    }
}
