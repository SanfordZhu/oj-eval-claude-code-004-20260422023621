#include "utils.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iomanip>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

bool isValidUserID(const std::string& userID) {
    if (userID.empty() || userID.length() > 30) return false;
    for (char c : userID) {
        if (!std::isalnum(c) && c != '_') return false;
    }
    return true;
}

bool isValidPassword(const std::string& password) {
    if (password.empty() || password.length() > 30) return false;
    for (char c : password) {
        if (!std::isalnum(c) && c != '_') return false;
    }
    return true;
}

bool isValidUsername(const std::string& username) {
    if (username.empty() || username.length() > 30) return false;
    for (char c : username) {
        if (c < 32 || c > 126) return false;
    }
    return true;
}

bool isValidISBN(const std::string& isbn) {
    if (isbn.empty() || isbn.length() > 20) return false;
    for (char c : isbn) {
        if (c < 32 || c > 126) return false;
    }
    return true;
}

bool isValidBookName(const std::string& name) {
    if (name.empty() || name.length() > 60) return false;
    for (char c : name) {
        if (c < 32 || c > 126 || c == '"') return false;
    }
    return true;
}

bool isValidAuthor(const std::string& author) {
    if (author.empty() || author.length() > 60) return false;
    for (char c : author) {
        if (c < 32 || c > 126 || c == '"') return false;
    }
    return true;
}

bool isValidKeyword(const std::string& keyword) {
    if (keyword.empty() || keyword.length() > 60) return false;
    std::vector<std::string> keywords = split(keyword, '|');
    if (keywords.empty()) return false;
    for (const std::string& kw : keywords) {
        if (kw.empty()) return false;
        for (char c : kw) {
            if (c < 32 || c > 126 || c == '"') return false;
        }
    }
    return true;
}

bool isValidPrice(const std::string& price) {
    if (price.empty() || price.length() > 13) return false;
    bool hasDot = false;
    for (char c : price) {
        if (c == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool isValidQuantity(const std::string& quantity) {
    if (quantity.empty() || quantity.length() > 10) return false;
    for (char c : quantity) {
        if (!std::isdigit(c)) return false;
    }
    long long val = std::stoll(quantity);
    return val <= 2147483647;
}

bool isValidPrivilege(const std::string& privilege) {
    if (privilege.length() != 1) return false;
    return privilege == "1" || privilege == "3" || privilege == "7";
}

std::string formatPrice(double price) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << price;
    return oss.str();
}