#include "account.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>

AccountManager::AccountManager(const std::string& filename) : accountFile(filename) {
    loadAccounts();
}

AccountManager::~AccountManager() {
    saveAccounts();
}

bool AccountManager::loadAccounts() {
    std::ifstream file(accountFile, std::ios::binary);
    if (!file.is_open()) return false;

    accounts.clear();
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        Account acc;
        size_t len;

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        acc.userID.resize(len);
        file.read(&acc.userID[0], len);

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        acc.password.resize(len);
        file.read(&acc.password[0], len);

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        acc.username.resize(len);
        file.read(&acc.username[0], len);

        file.read(reinterpret_cast<char*>(&acc.privilege), sizeof(acc.privilege));

        accounts.push_back(acc);
    }

    file.close();
    return true;
}

bool AccountManager::saveAccounts() {
    std::ofstream file(accountFile, std::ios::binary);
    if (!file.is_open()) return false;

    size_t count = accounts.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& acc : accounts) {
        size_t len = acc.userID.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(acc.userID.c_str(), len);

        len = acc.password.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(acc.password.c_str(), len);

        len = acc.username.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(acc.username.c_str(), len);

        file.write(reinterpret_cast<const char*>(&acc.privilege), sizeof(acc.privilege));
    }

    file.close();
    return true;
}

int AccountManager::findAccount(const std::string& userID) const {
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i].userID == userID) {
            return i;
        }
    }
    return -1;
}

bool AccountManager::initRootAccount() {
    if (findAccount("root") != -1) return true;

    Account root("root", "sjtu", "root", 7);
    accounts.push_back(root);
    return saveAccounts();
}

bool AccountManager::login(const std::string& userID, const std::string& password) {
    if (!isValidUserID(userID)) return false;

    int index = findAccount(userID);
    if (index == -1) return false;

    int currentPrivilege = getCurrentPrivilege();

    if (currentPrivilege <= accounts[index].privilege) {
        if (password.empty() || password != accounts[index].password) {
            return false;
        }
    }

    loginStack.push_back(userID);
    return true;
}

bool AccountManager::logout() {
    if (loginStack.empty()) return false;
    loginStack.pop_back();
    return true;
}

bool AccountManager::registerAccount(const std::string& userID, const std::string& password, const std::string& username) {
    if (!isValidUserID(userID) || !isValidPassword(password) || !isValidUsername(username)) {
        return false;
    }

    if (findAccount(userID) != -1) return false;

    Account newAcc(userID, password, username, 1);
    accounts.push_back(newAcc);
    return saveAccounts();
}

bool AccountManager::changePassword(const std::string& userID, const std::string& newPassword, const std::string& currentPassword) {
    if (!isValidUserID(userID) || !isValidPassword(newPassword)) return false;

    int index = findAccount(userID);
    if (index == -1) return false;

    int currentPrivilege = getCurrentPrivilege();

    if (currentPrivilege < 7) {
        if (currentPassword.empty() || currentPassword != accounts[index].password) {
            return false;
        }
    }

    accounts[index].password = newPassword;
    return saveAccounts();
}

bool AccountManager::addUser(const std::string& userID, const std::string& password, int privilege, const std::string& username) {
    if (!isValidUserID(userID) || !isValidPassword(password) || !isValidUsername(username)) {
        return false;
    }

    if (privilege != 1 && privilege != 3 && privilege != 7) return false;

    int currentPrivilege = getCurrentPrivilege();
    if (currentPrivilege <= privilege) return false;

    if (findAccount(userID) != -1) return false;

    Account newAcc(userID, password, username, privilege);
    accounts.push_back(newAcc);
    return saveAccounts();
}

bool AccountManager::deleteUser(const std::string& userID) {
    if (!isValidUserID(userID)) return false;

    int index = findAccount(userID);
    if (index == -1) return false;

    if (isUserLoggedIn(userID)) return false;

    accounts.erase(accounts.begin() + index);
    return saveAccounts();
}

int AccountManager::getCurrentPrivilege() const {
    if (loginStack.empty()) return 0;
    int index = findAccount(loginStack.back());
    if (index == -1) return 0;
    return accounts[index].privilege;
}

std::string AccountManager::getCurrentUser() const {
    if (loginStack.empty()) return "";
    return loginStack.back();
}

bool AccountManager::isLoggedIn() const {
    return !loginStack.empty();
}

bool AccountManager::isUserLoggedIn(const std::string& userID) const {
    return std::find(loginStack.begin(), loginStack.end(), userID) != loginStack.end();
}