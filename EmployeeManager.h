#ifndef EMPLOYEE_MANAGER_H
#define EMPLOYEE_MANAGER_H

#include <string>

class EmployeeManager {
public:
    explicit EmployeeManager(const std::string& fileName = "Administrators..txt");
    bool authenticate(const std::string& username, const std::string& password) const;
    void showAdminMenu();
private:
    std::string fileName;
    bool getPassword(const std::string& username, std::string& password) const;
};

#endif
