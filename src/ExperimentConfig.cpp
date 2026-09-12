#include "ExperimentConfig.hpp"
#include <fstream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cctype>

namespace {

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string stripQuotes(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

// Splits `text` on `delim` but ignores delimiters that occur inside double
// quotes. Sufficient for the flat, unnested JSON objects we accept.
std::vector<std::string> splitTopLevel(const std::string& text, char delim) {
    std::vector<std::string> parts;
    std::string current;
    bool inQuotes = false;
    for (char c : text) {
        if (c == '"') inQuotes = !inQuotes;
        if (c == delim && !inQuotes) {
            parts.push_back(current);
            current.clear();
        } else {
            current.push_back(c);
        }
    }
    if (!current.empty()) parts.push_back(current);
    return parts;
}

} // namespace

ExperimentConfig::ExperimentConfig(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("ExperimentConfig: could not open " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    parse(buffer.str());
}

void ExperimentConfig::parse(const std::string& text) {
    std::string t = trim(text);
    if (!t.empty() && t.front() == '{') t = t.substr(1);
    if (!t.empty() && t.back() == '}') t.pop_back();

    for (const auto& pairText : splitTopLevel(t, ',')) {
        std::string p = trim(pairText);
        if (p.empty()) continue;

        size_t colon = std::string::npos;
        bool inQuotes = false;
        for (size_t i = 0; i < p.size(); ++i) {
            if (p[i] == '"') inQuotes = !inQuotes;
            if (p[i] == ':' && !inQuotes) { colon = i; break; }
        }
        if (colon == std::string::npos) continue;

        std::string key = stripQuotes(trim(p.substr(0, colon)));
        std::string value = stripQuotes(trim(p.substr(colon + 1)));
        raw_[key] = value;
    }
}

bool ExperimentConfig::has(const std::string& key) const {
    return raw_.find(key) != raw_.end();
}

double ExperimentConfig::getNumber(const std::string& key, double defaultValue) const {
    auto it = raw_.find(key);
    if (it == raw_.end()) return defaultValue;
    try {
        return std::stod(it->second);
    } catch (...) {
        return defaultValue;
    }
}

std::string ExperimentConfig::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = raw_.find(key);
    if (it == raw_.end()) return defaultValue;
    return it->second;
}
