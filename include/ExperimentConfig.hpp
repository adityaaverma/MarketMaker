#pragma once
#include <string>
#include <unordered_map>

// Minimal flat JSON object parser: supports {"key": number|string, ...}.
// No nesting, no arrays -- just enough to drive experiment config files
// without pulling in an external JSON library.
class ExperimentConfig {
public:
    explicit ExperimentConfig(const std::string& path);

    double getNumber(const std::string& key, double defaultValue) const;
    std::string getString(const std::string& key, const std::string& defaultValue) const;
    bool has(const std::string& key) const;

private:
    std::unordered_map<std::string, std::string> raw_;
    void parse(const std::string& text);
};
