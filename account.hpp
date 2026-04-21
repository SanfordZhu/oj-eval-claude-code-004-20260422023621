#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include <string>
#include <vector>
#include <fstream>

struct Account {
    std::string userID;
    std::string password;
    std::string username;
    int privilege;

    Account() : privilege(0) {}
    Account(const std::string& id, const std::string& pwd, const std::string& name, int priv)
        : userID(id), password(pwd), username(name), privilege(priv) {}
};

class AccountManager {
private:
    std::vector<Account> accounts;
    std::vector<std::string> loginStack;
    std::string accountFile;

    bool loadAccounts();
    bool saveAccounts();
    int findAccount(const std::string& userID) const;

public:
    AccountManager(const std::string& filename = "accounts.dat");
    ~AccountManager();

    bool initRootAccount();
    bool login(const std::string& userID, const std::string& password = "");
    bool logout();
    bool registerAccount(const std::string& userID, const std::string& password, const std::string& username);
    bool changePassword(const std::string& userID, const std::string& newPassword, const std::string& currentPassword = "");
    bool addUser(const std::string& userID, const std::string& password, int privilege, const std::string& username);
    bool deleteUser(const std::string& userID);

    int getCurrentPrivilege() const;
    std::string getCurrentUser() const;
    bool isLoggedIn() const;
    bool isUserLoggedIn(const std::string& userID) const;
};

#endif