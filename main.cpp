#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include "account.hpp"
#include "book.hpp"
#include "log.hpp"
#include "utils.hpp"

class BookstoreSystem {
private:
    AccountManager accountManager;
    BookManager bookManager;
    LogManager logManager;

    bool processCommand(const std::vector<std::string>& tokens);
    bool parseModifyParams(const std::vector<std::string>& tokens, std::string& isbn, std::string& name, std::string& author, std::string& keyword, double& price);

public:
    BookstoreSystem();
    void run();
};

BookstoreSystem::BookstoreSystem() {
    accountManager.initRootAccount();
}

bool BookstoreSystem::parseModifyParams(const std::vector<std::string>& tokens, std::string& isbn, std::string& name, std::string& author, std::string& keyword, double& price) {
    isbn = "";
    name = "";
    author = "";
    keyword = "";
    price = 0;

    for (size_t i = 1; i < tokens.size(); ++i) {
        if (tokens[i].substr(0, 6) == "-ISBN=") {
            if (!isbn.empty()) return false;
            isbn = tokens[i].substr(6);
            if (!isValidISBN(isbn)) return false;
        } else if (tokens[i].substr(0, 7) == "-name=\"") {
            if (!name.empty()) return false;
            size_t end = tokens[i].find('"', 7);
            if (end == std::string::npos) return false;
            name = tokens[i].substr(7, end - 7);
            if (!isValidBookName(name)) return false;
        } else if (tokens[i].substr(0, 9) == "-author=\"") {
            if (!author.empty()) return false;
            size_t end = tokens[i].find('"', 9);
            if (end == std::string::npos) return false;
            author = tokens[i].substr(9, end - 9);
            if (!isValidAuthor(author)) return false;
        } else if (tokens[i].substr(0, 10) == "-keyword=\"") {
            if (!keyword.empty()) return false;
            size_t end = tokens[i].find('"', 10);
            if (end == std::string::npos) return false;
            keyword = tokens[i].substr(10, end - 10);
            if (!isValidKeyword(keyword)) return false;
        } else if (tokens[i].substr(0, 8) == "-price=") {
            if (price > 0) return false;
            std::string priceStr = tokens[i].substr(8);
            if (!isValidPrice(priceStr)) return false;
            price = std::stod(priceStr);
        } else {
            return false;
        }
    }

    return true;
}

bool BookstoreSystem::processCommand(const std::vector<std::string>& tokens) {
    if (tokens.empty()) return true;

    const std::string& cmd = tokens[0];

    if (cmd == "quit" || cmd == "exit") {
        return false;
    } else if (cmd == "su") {
        if (tokens.size() < 2 || tokens.size() > 3) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        std::string password = tokens.size() == 3 ? tokens[2] : "";
        if (!accountManager.login(tokens[1], password)) {
            std::cout << "Invalid" << std::endl;
        }
    } else if (cmd == "logout") {
        if (accountManager.getCurrentPrivilege() < 1) {
            std::cout << "Invalid" << std::endl;
        } else if (!accountManager.logout()) {
            std::cout << "Invalid" << std::endl;
        }
    } else if (cmd == "register") {
        if (accountManager.getCurrentPrivilege() < 0) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (tokens.size() != 4) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (!accountManager.registerAccount(tokens[1], tokens[2], tokens[3])) {
            std::cout << "Invalid" << std::endl;
        }
    } else if (cmd == "passwd") {
        if (accountManager.getCurrentPrivilege() < 1) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (tokens.size() < 3 || tokens.size() > 4) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        std::string currentPwd = tokens.size() == 4 ? tokens[2] : "";
        std::string newPwd = tokens.size() == 4 ? tokens[3] : tokens[2];
        if (!accountManager.changePassword(tokens[1], newPwd, currentPwd)) {
            std::cout << "Invalid" << std::endl;
        }
    } else if (cmd == "useradd") {
        if (accountManager.getCurrentPrivilege() < 3) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (tokens.size() != 5) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (!isValidPrivilege(tokens[3])) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        int privilege = std::stoi(tokens[3]);
        if (!accountManager.addUser(tokens[1], tokens[2], privilege, tokens[4])) {
            std::cout << "Invalid" << std::endl;
        }
    } else if (cmd == "delete") {
        if (accountManager.getCurrentPrivilege() < 7) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (tokens.size() != 2) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (!accountManager.deleteUser(tokens[1])) {
            std::cout << "Invalid" << std::endl;
        }
    } else if (cmd == "show") {
        if (tokens.size() >= 2 && tokens[1] == "finance") {
            if (accountManager.getCurrentPrivilege() < 7) {
                std::cout << "Invalid" << std::endl;
                return true;
            }
            int count = -1;
            if (tokens.size() == 3) {
                if (!isValidQuantity(tokens[2])) {
                    std::cout << "Invalid" << std::endl;
                    return true;
                }
                count = std::stoi(tokens[2]);
            } else if (tokens.size() != 2) {
                std::cout << "Invalid" << std::endl;
                return true;
            }
            auto [income, expenditure] = logManager.showFinance(count);
            if (income == -1 && expenditure == -1) {
                std::cout << "Invalid" << std::endl;
            } else {
                std::cout << "+ " << std::fixed << std::setprecision(2) << income
                          << " - " << std::fixed << std::setprecision(2) << expenditure << std::endl;
            }
        } else {
            if (accountManager.getCurrentPrivilege() < 1) {
                std::cout << "Invalid" << std::endl;
                return true;
            }
            if (tokens.size() > 2) {
                std::cout << "Invalid" << std::endl;
                return true;
            }
            std::string type = "";
            std::string value = "";
            if (tokens.size() == 2) {
                std::string param = tokens[1];
                if (param.substr(0, 6) == "-ISBN=") {
                    type = "-ISBN";
                    value = param.substr(6);
                } else if (param.substr(0, 7) == "-name=\"") {
                    type = "-name";
                    size_t end = param.find('"', 7);
                    if (end == std::string::npos) {
                        std::cout << "Invalid" << std::endl;
                        return true;
                    }
                    value = param.substr(7, end - 7);
                } else if (param.substr(0, 9) == "-author=\"") {
                    type = "-author";
                    size_t end = param.find('"', 9);
                    if (end == std::string::npos) {
                        std::cout << "Invalid" << std::endl;
                        return true;
                    }
                    value = param.substr(9, end - 9);
                } else if (param.substr(0, 10) == "-keyword=\"") {
                    type = "-keyword";
                    size_t end = param.find('"', 10);
                    if (end == std::string::npos) {
                        std::cout << "Invalid" << std::endl;
                        return true;
                    }
                    value = param.substr(10, end - 10);
                } else {
                    std::cout << "Invalid" << std::endl;
                    return true;
                }
            }
            std::vector<Book> books = bookManager.showBooks(type, value);
            for (const auto& book : books) {
                std::cout << book.isbn << "\t" << book.name << "\t" << book.author << "\t"
                          << book.keyword << "\t" << formatPrice(book.price) << "\t" << book.quantity << std::endl;
            }
        }
    } else if (cmd == "buy") {
        if (accountManager.getCurrentPrivilege() < 1) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (tokens.size() != 3) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (!isValidQuantity(tokens[2])) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        int quantity = std::stoi(tokens[2]);
        double totalCost;
        if (!bookManager.buyBook(tokens[1], quantity, totalCost)) {
            std::cout << "Invalid" << std::endl;
        } else {
            std::cout << std::fixed << std::setprecision(2) << totalCost << std::endl;
            logManager.addFinanceRecord(totalCost, 0, "buy", accountManager.getCurrentUser());
        }
    } else if (cmd == "select") {
        if (accountManager.getCurrentPrivilege() < 3) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (tokens.size() != 2) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (!bookManager.selectBook(tokens[1])) {
            std::cout << "Invalid" << std::endl;
        }
    } else if (cmd == "modify") {
        if (accountManager.getCurrentPrivilege() < 3) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (tokens.size() < 2) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        std::string isbn, name, author, keyword;
        double price;
        if (!parseModifyParams(tokens, isbn, name, author, keyword, price)) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (!bookManager.modifyBook(isbn, name, author, keyword, price)) {
            std::cout << "Invalid" << std::endl;
        }
    } else if (cmd == "import") {
        if (accountManager.getCurrentPrivilege() < 3) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (tokens.size() != 3) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (!isValidQuantity(tokens[1]) || !isValidPrice(tokens[2])) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        int quantity = std::stoi(tokens[1]);
        double totalCost = std::stod(tokens[2]);
        if (!bookManager.importBook(quantity, totalCost)) {
            std::cout << "Invalid" << std::endl;
        } else {
            logManager.addFinanceRecord(0, totalCost, "import", accountManager.getCurrentUser());
        }
    } else if (cmd == "log") {
        if (accountManager.getCurrentPrivilege() < 7) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        logManager.showLogs();
    } else if (cmd == "report" && tokens.size() == 2) {
        if (accountManager.getCurrentPrivilege() < 7) {
            std::cout << "Invalid" << std::endl;
            return true;
        }
        if (tokens[1] == "finance") {
            logManager.generateFinanceReport();
        } else if (tokens[1] == "employee") {
            logManager.generateEmployeeReport();
        } else {
            std::cout << "Invalid" << std::endl;
        }
    } else {
        std::cout << "Invalid" << std::endl;
    }

    return true;
}

void BookstoreSystem::run() {
    std::string line;
    while (std::getline(std::cin, line)) {
        line = trim(line);
        if (line.empty()) continue;

        std::vector<std::string> tokens = split(line, ' ');
        if (!processCommand(tokens)) {
            break;
        }
    }
}

int main() {
    BookstoreSystem system;
    system.run();
    return 0;
}