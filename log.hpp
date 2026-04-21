#ifndef LOG_HPP
#define LOG_HPP

#include <string>
#include <vector>
#include <fstream>
#include <ctime>

struct FinanceRecord {
    double income;
    double expenditure;
    std::string operation;
    std::string user;
    std::time_t timestamp;

    FinanceRecord() : income(0), expenditure(0), timestamp(0) {}
    FinanceRecord(double inc, double exp, const std::string& op, const std::string& u, std::time_t t)
        : income(inc), expenditure(exp), operation(op), user(u), timestamp(t) {}
};

struct OperationLog {
    std::string user;
    std::string operation;
    std::string details;
    std::time_t timestamp;

    OperationLog() : timestamp(0) {}
    OperationLog(const std::string& u, const std::string& op, const std::string& det, std::time_t t)
        : user(u), operation(op), details(det), timestamp(t) {}
};

class LogManager {
private:
    std::vector<FinanceRecord> financeRecords;
    std::vector<OperationLog> operationLogs;
    std::string financeFile;
    std::string logFile;

    bool loadFinanceRecords();
    bool saveFinanceRecords();
    bool loadOperationLogs();
    bool saveOperationLogs();

public:
    LogManager(const std::string& financeFile = "finance.dat", const std::string& logFile = "logs.dat");
    ~LogManager();

    void addFinanceRecord(double income, double expenditure, const std::string& operation, const std::string& user);
    void addOperationLog(const std::string& user, const std::string& operation, const std::string& details);

    std::pair<double, double> showFinance(int count = -1);
    void generateFinanceReport();
    void generateEmployeeReport();
    void showLogs();
};

#endif