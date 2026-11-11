#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>

class Employee {
public:
    explicit Employee(const std::string& strFileName = "Employees.txt");
    bool authenticateEmployee(const std::string& strUsername, const std::string& strPassword) const;
    bool changePassword(const std::string& strUsername, const std::string& strCurrentPassword, const std::string& strNewPassword);
    bool needsFirstLoginPasswordChange(const std::string& strUsername) const;
private:
    std::string _strFileName;
    bool getPassword(const std::string& strUsername, std::string& strPassword) const;
};

#endif
