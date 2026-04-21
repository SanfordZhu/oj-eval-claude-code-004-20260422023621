#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

std::vector<std::string> split(const std::string& str, char delimiter);
std::string trim(const std::string& str);
bool isValidUserID(const std::string& userID);
bool isValidPassword(const std::string& password);
bool isValidUsername(const std::string& username);
bool isValidISBN(const std::string& isbn);
bool isValidBookName(const std::string& name);
bool isValidAuthor(const std::string& author);
bool isValidKeyword(const std::string& keyword);
bool isValidPrice(const std::string& price);
bool isValidQuantity(const std::string& quantity);
bool isValidPrivilege(const std::string& privilege);
std::string formatPrice(double price);

#endif