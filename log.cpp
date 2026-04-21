#include "log.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

LogManager::LogManager(const std::string& financeFile, const std::string& logFile)
    : financeFile(financeFile), logFile(logFile) {
    loadFinanceRecords();
    loadOperationLogs();
}

LogManager::~LogManager() {
    saveFinanceRecords();
    saveOperationLogs();
}

bool LogManager::loadFinanceRecords() {
    std::ifstream file(financeFile, std::ios::binary);
    if (!file.is_open()) return false;

    financeRecords.clear();
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        FinanceRecord record;

        file.read(reinterpret_cast<char*>(&record.income), sizeof(record.income));
        file.read(reinterpret_cast<char*>(&record.expenditure), sizeof(record.expenditure));

        size_t len;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        record.operation.resize(len);
        file.read(&record.operation[0], len);

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        record.user.resize(len);
        file.read(&record.user[0], len);

        file.read(reinterpret_cast<char*>(&record.timestamp), sizeof(record.timestamp));

        financeRecords.push_back(record);
    }

    file.close();
    return true;
}

bool LogManager::saveFinanceRecords() {
    std::ofstream file(financeFile, std::ios::binary);
    if (!file.is_open()) return false;

    size_t count = financeRecords.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& record : financeRecords) {
        file.write(reinterpret_cast<const char*>(&record.income), sizeof(record.income));
        file.write(reinterpret_cast<const char*>(&record.expenditure), sizeof(record.expenditure));

        size_t len = record.operation.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(record.operation.c_str(), len);

        len = record.user.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(record.user.c_str(), len);

        file.write(reinterpret_cast<const char*>(&record.timestamp), sizeof(record.timestamp));
    }

    file.close();
    return true;
}

bool LogManager::loadOperationLogs() {
    std::ifstream file(logFile, std::ios::binary);
    if (!file.is_open()) return false;

    operationLogs.clear();
    size_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));

    for (size_t i = 0; i < count; ++i) {
        OperationLog log;

        size_t len;
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        log.user.resize(len);
        file.read(&log.user[0], len);

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        log.operation.resize(len);
        file.read(&log.operation[0], len);

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        log.details.resize(len);
        file.read(&log.details[0], len);

        file.read(reinterpret_cast<char*>(&log.timestamp), sizeof(log.timestamp));

        operationLogs.push_back(log);
    }

    file.close();
    return true;
}

bool LogManager::saveOperationLogs() {
    std::ofstream file(logFile, std::ios::binary);
    if (!file.is_open()) return false;

    size_t count = operationLogs.size();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& log : operationLogs) {
        size_t len = log.user.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(log.user.c_str(), len);

        len = log.operation.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(log.operation.c_str(), len);

        len = log.details.length();
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(log.details.c_str(), len);

        file.write(reinterpret_cast<const char*>(&log.timestamp), sizeof(log.timestamp));
    }

    file.close();
    return true;
}

void LogManager::addFinanceRecord(double income, double expenditure, const std::string& operation, const std::string& user) {
    FinanceRecord record(income, expenditure, operation, user, std::time(nullptr));
    financeRecords.push_back(record);
}

void LogManager::addOperationLog(const std::string& user, const std::string& operation, const std::string& details) {
    OperationLog log(user, operation, details, std::time(nullptr));
    operationLogs.push_back(log);
}

std::pair<double, double> LogManager::showFinance(int count) {
    double totalIncome = 0;
    double totalExpenditure = 0;

    if (count == -1) {
        for (const auto& record : financeRecords) {
            totalIncome += record.income;
            totalExpenditure += record.expenditure;
        }
    } else if (count == 0) {
        return {0, 0};
    } else {
        if (count > static_cast<int>(financeRecords.size())) {
            return {-1, -1};
        }
        int start = financeRecords.size() - count;
        for (int i = start; i < static_cast<int>(financeRecords.size()); ++i) {
            totalIncome += financeRecords[i].income;
            totalExpenditure += financeRecords[i].expenditure;
        }
    }

    return {totalIncome, totalExpenditure};
}

void LogManager::generateFinanceReport() {
    auto [income, expenditure] = showFinance();
    std::cout << "+ " << std::fixed << std::setprecision(2) << income
              << " - " << std::fixed << std::setprecision(2) << expenditure << std::endl;
}

void LogManager::generateEmployeeReport() {
    std::cout << "Employee Work Report" << std::endl;
    std::cout << "====================" << std::endl;

    for (const auto& log : operationLogs) {
        std::cout << "User: " << log.user << std::endl;
        std::cout << "Operation: " << log.operation << std::endl;
        std::cout << "Details: " << log.details << std::endl;
        std::cout << "Time: " << std::ctime(&log.timestamp);
        std::cout << "--------------------" << std::endl;
    }
}

void LogManager::showLogs() {
    for (const auto& log : operationLogs) {
        std::cout << "[" << std::ctime(&log.timestamp) << "] "
                  << log.user << " " << log.operation << " " << log.details << std::endl;
    }
}