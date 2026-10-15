#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#pragma once
#include <string>
#include <vector>
#include <utility>

template <class DataType>
class RecordList {
private:
    std::vector<DataType> items;
public:
    void add(const DataType& item) {
        items.push_back(item);
    }
    const std::vector<DataType>& getItems() const {
        return items;
    }
    size_t size() const {
        return items.size();
    }
};

class Employee {
public:
    explicit Employee(const std::string& strFileName = "Employees.txt");
    bool authenticateEmployee(const std::string& strUsername, const std::string& strPassword) const;
    bool changePassword(const std::string& strUsername, const std::string& strCurrentPassword, const std::string& strNewPassword);
    bool displayProfile(const std::string& strUsername) const;
    bool needsFirstLoginPasswordChange(const std::string& strUsername) const;
private:
    std::string strFileName;
    bool getPassword(const std::string& strUsername, std::string& strPassword) const;
    friend class EmployeeManager;
};

#endif
