#include "io/ConfigReader.h"

#include <cctype>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>

namespace cfd {
namespace {

struct JsonValue {
    enum class Type {
        Null,
        Bool,
        Number,
        String,
        Array,
        Object
    };

    Type type = Type::Null;
    bool boolean = false;
    double number = 0.0;
    std::string string;
    std::vector<JsonValue> array;
    std::map<std::string, JsonValue> object;
};

class JsonParser {
public:
    explicit JsonParser(const std::string& input) : input_(input) {}

    JsonValue parse() {
        skipWhitespace();
        JsonValue value = parseValue();
        skipWhitespace();
        if (pos_ != input_.size()) {
            throw std::runtime_error("Unexpected trailing characters in JSON");
        }
        return value;
    }

private:
    JsonValue parseValue() {
        if (match("null")) {
            return JsonValue{JsonValue::Type::Null};
        }
        if (match("true")) {
            JsonValue value{JsonValue::Type::Bool};
            value.boolean = true;
            return value;
        }
        if (match("false")) {
            JsonValue value{JsonValue::Type::Bool};
            value.boolean = false;
            return value;
        }
        if (peek() == '"') {
            JsonValue value{JsonValue::Type::String};
            value.string = parseString();
            return value;
        }
        if (peek() == '{') {
            return parseObject();
        }
        if (peek() == '[') {
            return parseArray();
        }
        if (peek() == '-' || std::isdigit(peek())) {
            return parseNumber();
        }
        throw std::runtime_error("Invalid JSON value");
    }

    JsonValue parseObject() {
        expect('{');
        JsonValue value{JsonValue::Type::Object};
        skipWhitespace();
        if (peek() == '}') {
            ++pos_;
            return value;
        }
        while (true) {
            skipWhitespace();
            std::string key = parseString();
            skipWhitespace();
            expect(':');
            skipWhitespace();
            value.object.emplace(key, parseValue());
            skipWhitespace();
            if (peek() == '}') {
                ++pos_;
                break;
            }
            expect(',');
        }
        return value;
    }

    JsonValue parseArray() {
        expect('[');
        JsonValue value{JsonValue::Type::Array};
        skipWhitespace();
        if (peek() == ']') {
            ++pos_;
            return value;
        }
        while (true) {
            value.array.push_back(parseValue());
            skipWhitespace();
            if (peek() == ']') {
                ++pos_;
                break;
            }
            expect(',');
        }
        return value;
    }

    JsonValue parseNumber() {
        size_t start = pos_;
        if (peek() == '-') {
            ++pos_;
        }
        while (std::isdigit(peek())) {
            ++pos_;
        }
        if (peek() == '.') {
            ++pos_;
            while (std::isdigit(peek())) {
                ++pos_;
            }
        }
        if (peek() == 'e' || peek() == 'E') {
            ++pos_;
            if (peek() == '+' || peek() == '-') {
                ++pos_;
            }
            while (std::isdigit(peek())) {
                ++pos_;
            }
        }
        double number = std::stod(input_.substr(start, pos_ - start));
        JsonValue value{JsonValue::Type::Number};
        value.number = number;
        return value;
    }

    std::string parseString() {
        expect('"');
        std::ostringstream result;
        bool closed = false;
        while (pos_ < input_.size()) {
            char c = input_[pos_++];
            if (c == '"') {
                closed = true;
                break;
            }
            if (c == '\\') {
                if (pos_ >= input_.size()) {
                    throw std::runtime_error("Invalid escape sequence");
                }
                char esc = input_[pos_++];
                switch (esc) {
                    case '"': result << '"'; break;
                    case '\\': result << '\\'; break;
                    case '/': result << '/'; break;
                    case 'b': result << '\b'; break;
                    case 'f': result << '\f'; break;
                    case 'n': result << '\n'; break;
                    case 'r': result << '\r'; break;
                    case 't': result << '\t'; break;
                    default: throw std::runtime_error("Unsupported escape sequence");
                }
            } else {
                result << c;
            }
        }
        if (!closed) {
            throw std::runtime_error("Unterminated string in JSON");
        }
        return result.str();
    }

    void skipWhitespace() {
        while (pos_ < input_.size() && std::isspace(static_cast<unsigned char>(input_[pos_]))) {
            ++pos_;
        }
    }

    bool match(const std::string& token) {
        if (input_.compare(pos_, token.size(), token) == 0) {
            pos_ += token.size();
            return true;
        }
        return false;
    }

    void expect(char c) {
        if (peek() != c) {
            throw std::runtime_error(std::string("Expected '") + c + "' in JSON");
        }
        ++pos_;
    }

    char peek() const {
        if (pos_ >= input_.size()) {
            return '\0';
        }
        return input_[pos_];
    }

    const std::string& input_;
    size_t pos_ = 0;
};

const JsonValue* findObject(const JsonValue& value, const std::string& key) {
    if (value.type != JsonValue::Type::Object) {
        return nullptr;
    }
    auto it = value.object.find(key);
    if (it == value.object.end()) {
        return nullptr;
    }
    return &it->second;
}

const JsonValue* findArray(const JsonValue& value, const std::string& key) {
    const JsonValue* child = findObject(value, key);
    if (!child || child->type != JsonValue::Type::Array) {
        return nullptr;
    }
    return child;
}

bool loadNumber(const JsonValue& value, double& out) {
    if (value.type != JsonValue::Type::Number) {
        return false;
    }
    out = value.number;
    return true;
}

bool loadInt(const JsonValue& value, int& out) {
    if (value.type != JsonValue::Type::Number) {
        return false;
    }
    out = static_cast<int>(value.number);
    return true;
}

bool loadString(const JsonValue& value, std::string& out) {
    if (value.type != JsonValue::Type::String) {
        return false;
    }
    out = value.string;
    return true;
}

std::array<double, 3> defaultVector3() {
    return {0.0, 0.0, 0.0};
}

} // namespace

ConfigReader ConfigReader::fromFile(const std::string& path) {
    ConfigReader reader;
    std::ifstream file(path);
    if (!file) {
        reader.errors_.push_back("Unable to open config file: " + path);
        return reader;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    try {
        JsonParser parser(buffer.str());
        JsonValue root = parser.parse();
        if (root.type != JsonValue::Type::Object) {
            reader.errors_.push_back("Config root must be a JSON object.");
            return reader;
        }

        if (const JsonValue* geometry = findObject(root, "geometry")) {
            if (const JsonValue* fileValue = findObject(*geometry, "file")) {
                if (!loadString(*fileValue, reader.config_.geometry.file)) {
                    reader.errors_.push_back("geometry.file must be a string.");
                }
            } else {
                reader.errors_.push_back("Missing required geometry.file setting.");
            }

            if (const JsonValue* scaleValue = findObject(*geometry, "scale")) {
                if (!loadNumber(*scaleValue, reader.config_.geometry.scale)) {
                    reader.errors_.push_back("geometry.scale must be a number.");
                }
            }
        } else {
            reader.errors_.push_back("Missing geometry section.");
        }

        if (const JsonValue* mesh = findObject(root, "mesh")) {
            if (const JsonValue* baseSize = findObject(*mesh, "baseSize")) {
                if (!loadNumber(*baseSize, reader.config_.mesh.baseSize)) {
                    reader.errors_.push_back("mesh.baseSize must be a number.");
                }
            }
            if (const JsonValue* layers = findObject(*mesh, "boundaryLayers")) {
                if (!loadInt(*layers, reader.config_.mesh.boundaryLayers)) {
                    reader.errors_.push_back("mesh.boundaryLayers must be an integer.");
                }
            }
        } else {
            reader.warnings_.push_back("Mesh section missing. Using defaults.");
        }

        if (const JsonValue* simulation = findObject(root, "simulation")) {
            if (const JsonValue* startTime = findObject(*simulation, "startTime")) {
                if (!loadNumber(*startTime, reader.config_.simulation.startTime)) {
                    reader.errors_.push_back("simulation.startTime must be a number.");
                }
            }
            if (const JsonValue* endTime = findObject(*simulation, "endTime")) {
                if (!loadNumber(*endTime, reader.config_.simulation.endTime)) {
                    reader.errors_.push_back("simulation.endTime must be a number.");
                }
            }
            if (const JsonValue* timeStep = findObject(*simulation, "timeStep")) {
                if (!loadNumber(*timeStep, reader.config_.simulation.timeStep)) {
                    reader.errors_.push_back("simulation.timeStep must be a number.");
                }
            }
            if (const JsonValue* outputInterval = findObject(*simulation, "outputInterval")) {
                if (!loadNumber(*outputInterval, reader.config_.simulation.outputInterval)) {
                    reader.errors_.push_back("simulation.outputInterval must be a number.");
                }
            }
        } else {
            reader.warnings_.push_back("Simulation section missing. Using defaults.");
        }

        if (const JsonValue* combustion = findObject(root, "combustion")) {
            if (const JsonValue* mechanism = findObject(*combustion, "mechanism")) {
                if (!loadString(*mechanism, reader.config_.combustion.mechanism)) {
                    reader.errors_.push_back("combustion.mechanism must be a string.");
                }
            }
            if (const JsonValue* ethanolFraction = findObject(*combustion, "ethanolFraction")) {
                if (!loadNumber(*ethanolFraction, reader.config_.combustion.ethanolFraction)) {
                    reader.errors_.push_back("combustion.ethanolFraction must be a number.");
                }
            }

            if (const JsonValue* spark = findObject(*combustion, "spark")) {
                if (const JsonValue* location = findArray(*spark, "location")) {
                    if (location->array.size() == 3) {
                        std::array<double, 3> loc = defaultVector3();
                        for (size_t i = 0; i < 3; ++i) {
                            if (!loadNumber(location->array[i], loc[i])) {
                                reader.errors_.push_back("combustion.spark.location must be an array of numbers.");
                                break;
                            }
                        }
                        reader.config_.combustion.spark.location = loc;
                    } else {
                        reader.errors_.push_back("combustion.spark.location must have 3 entries.");
                    }
                }
                if (const JsonValue* time = findObject(*spark, "time")) {
                    if (!loadNumber(*time, reader.config_.combustion.spark.time)) {
                        reader.errors_.push_back("combustion.spark.time must be a number.");
                    }
                }
                if (const JsonValue* energy = findObject(*spark, "energy")) {
                    if (!loadNumber(*energy, reader.config_.combustion.spark.energy)) {
                        reader.errors_.push_back("combustion.spark.energy must be a number.");
                    }
                }
            } else {
                reader.warnings_.push_back("Combustion spark settings missing.");
            }
        } else {
            reader.warnings_.push_back("Combustion section missing. Using defaults.");
        }

        if (reader.config_.geometry.scale <= 0.0) {
            reader.errors_.push_back("geometry.scale must be greater than zero.");
        }
        if (reader.config_.mesh.baseSize <= 0.0) {
            reader.errors_.push_back("mesh.baseSize must be greater than zero.");
        }
        if (reader.config_.simulation.endTime < reader.config_.simulation.startTime) {
            reader.errors_.push_back("simulation.endTime must be >= simulation.startTime.");
        }
        if (reader.config_.simulation.timeStep <= 0.0) {
            reader.errors_.push_back("simulation.timeStep must be greater than zero.");
        }
        if (reader.config_.simulation.outputInterval <= 0.0) {
            reader.errors_.push_back("simulation.outputInterval must be greater than zero.");
        }
        if (reader.config_.combustion.ethanolFraction < 0.0 || reader.config_.combustion.ethanolFraction > 1.0) {
            reader.errors_.push_back("combustion.ethanolFraction must be between 0 and 1.");
        }

        if (reader.config_.combustion.mechanism.empty()) {
            reader.warnings_.push_back("Combustion mechanism not specified.");
        }
    } catch (const std::exception& ex) {
        reader.errors_.push_back(std::string("Failed to parse config: ") + ex.what());
    }

    return reader;
}

std::string ConfigReader::summarize() const {
    std::ostringstream summary;
    summary << "Geometry file: " << (config_.geometry.file.empty() ? "<none>" : config_.geometry.file) << "\n";
    summary << "Geometry scale: " << config_.geometry.scale << "\n";
    summary << "Mesh base size: " << config_.mesh.baseSize << "\n";
    summary << "Mesh boundary layers: " << config_.mesh.boundaryLayers << "\n";
    summary << "Simulation window: " << config_.simulation.startTime << " -> " << config_.simulation.endTime << "\n";
    summary << "Time step: " << config_.simulation.timeStep << "\n";
    summary << "Output interval: " << config_.simulation.outputInterval << "\n";
    summary << "Combustion mechanism: " << (config_.combustion.mechanism.empty() ? "<none>" : config_.combustion.mechanism) << "\n";
    summary << "Ethanol fraction: " << config_.combustion.ethanolFraction << "\n";
    summary << "Spark location: [" << config_.combustion.spark.location[0] << ", "
            << config_.combustion.spark.location[1] << ", "
            << config_.combustion.spark.location[2] << "]\n";
    summary << "Spark time: " << config_.combustion.spark.time << "\n";
    summary << "Spark energy: " << config_.combustion.spark.energy << "\n";
    return summary.str();
}

} // namespace cfd
