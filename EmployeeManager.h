#ifndef EMPLOYEE_MANAGER_H
#define EMPLOYEE_MANAGER_H

#include <string>

class EmployeeManager {
public:
    explicit EmployeeManager(const std::string& strFileName = "Administrators.txt");
    bool authenticateManager(const std::string& strUsername, const std::string& strPassword) const;

    bool addEmployee (const std::string& strUsername, const std::string& strFullName, const std::string& strAddress, const std::string& strPhone, const std::string& strEmail) const;
    bool removeEmployee(const std::string& strUsername) const;
private:
    std::string _strFileName;
    bool getPassword(const std::string& strUsername, std::string& strPassword) const;
};

#endif
