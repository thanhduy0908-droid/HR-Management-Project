#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>

class Employee {
public:
    explicit Employee(const std::string& fileName = "Employees.txt");
    bool authenticate(const std::string& username, const std::string& password) const;
    bool changePassword(const std::string& username,
                        const std::string& currentPassword,
                        const std::string& newPassword);
    bool displayProfile(const std::string& username) const;
    bool needsFirstLoginPasswordChange(const std::string& username) const;

private:
    std::string fileName;
    bool getPassword(const std::string& username, std::string& password) const;
};

#endif
