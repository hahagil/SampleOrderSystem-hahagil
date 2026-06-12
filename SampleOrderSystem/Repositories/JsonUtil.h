#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// Minimal JSON read/write utilities for fixed-schema objects.
// Format: { "root": [ { ... }, ... ] }
// Limitations: values must not contain nested objects or escaped quotes.
namespace JsonUtil {

inline std::string escapeStr(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else out += c;
    }
    return out;
}

inline std::string unescapeStr(const std::string& s) {
    std::string out;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char n = s[++i];
            if (n == '"')  out += '"';
            else if (n == '\\') out += '\\';
            else if (n == 'n')  out += '\n';
            else if (n == 'r')  out += '\r';
            else { out += '\\'; out += n; }
        } else {
            out += s[i];
        }
    }
    return out;
}

// Extract raw value string for key (without surrounding quotes for strings).
// Returns "" if not found.
inline std::string extractRaw(const std::string& obj, const std::string& key) {
    std::string search = "\"" + key + "\"";
    size_t kpos = obj.find(search);
    if (kpos == std::string::npos) return "";
    size_t colon = obj.find(':', kpos + search.size());
    if (colon == std::string::npos) return "";
    size_t vstart = obj.find_first_not_of(" \t\r\n", colon + 1);
    if (vstart == std::string::npos) return "";
    if (obj[vstart] == '"') {
        // String value
        size_t end = vstart + 1;
        while (end < obj.size()) {
            if (obj[end] == '\\') { end += 2; continue; }
            if (obj[end] == '"')  break;
            ++end;
        }
        return obj.substr(vstart + 1, end - vstart - 1);
    } else {
        // Numeric / boolean value
        size_t end = obj.find_first_of(",}\r\n", vstart);
        std::string raw = obj.substr(vstart, end == std::string::npos ? std::string::npos : end - vstart);
        while (!raw.empty() && (raw.back() == ' ' || raw.back() == '\t')) raw.pop_back();
        return raw;
    }
}

inline std::string getString(const std::string& obj, const std::string& key) {
    return unescapeStr(extractRaw(obj, key));
}
inline int getInt(const std::string& obj, const std::string& key) {
    std::string v = extractRaw(obj, key);
    return v.empty() ? 0 : std::stoi(v);
}
inline double getDouble(const std::string& obj, const std::string& key) {
    std::string v = extractRaw(obj, key);
    return v.empty() ? 0.0 : std::stod(v);
}
inline long long getInt64(const std::string& obj, const std::string& key) {
    std::string v = extractRaw(obj, key);
    return v.empty() ? 0LL : std::stoll(v);
}

// Split a JSON array (the part inside [...]) into individual object strings.
inline std::vector<std::string> splitObjects(const std::string& arr) {
    std::vector<std::string> result;
    int depth = 0;
    size_t start = std::string::npos;
    bool inStr = false;
    for (size_t i = 0; i < arr.size(); ++i) {
        char c = arr[i];
        if (c == '\\' && inStr) { ++i; continue; }
        if (c == '"') { inStr = !inStr; continue; }
        if (inStr) continue;
        if (c == '{') {
            if (depth == 0) start = i;
            ++depth;
        } else if (c == '}') {
            --depth;
            if (depth == 0 && start != std::string::npos) {
                result.push_back(arr.substr(start, i - start + 1));
                start = std::string::npos;
            }
        }
    }
    return result;
}

// Read entire file content.
inline std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// Extract the JSON array for rootKey from file content.
inline std::string extractArray(const std::string& content, const std::string& rootKey) {
    std::string search = "\"" + rootKey + "\"";
    size_t kpos = content.find(search);
    if (kpos == std::string::npos) return "[]";
    size_t bpos = content.find('[', kpos + search.size());
    if (bpos == std::string::npos) return "[]";
    int depth = 0;
    bool inStr = false;
    for (size_t i = bpos; i < content.size(); ++i) {
        char c = content[i];
        if (c == '\\' && inStr) { ++i; continue; }
        if (c == '"') { inStr = !inStr; continue; }
        if (inStr) continue;
        if (c == '[') ++depth;
        else if (c == ']') {
            --depth;
            if (depth == 0) return content.substr(bpos, i - bpos + 1);
        }
    }
    return "[]";
}

// Write JSON file: { "rootKey": [ items... ] }
inline void writeArrayFile(const std::string& path, const std::string& rootKey,
                           const std::vector<std::string>& items) {
    std::ofstream f(path);
    f << "{\n  \"" << rootKey << "\": [\n";
    for (size_t i = 0; i < items.size(); ++i) {
        f << "    " << items[i];
        if (i + 1 < items.size()) f << ",";
        f << "\n";
    }
    f << "  ]\n}\n";
}

} // namespace JsonUtil
