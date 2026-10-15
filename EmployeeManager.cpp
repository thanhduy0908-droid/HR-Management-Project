#include "EmployeeManager.h"

#include <fstream>
#include <iostream>
#include <cstdio>
#include <vector>

EmployeeManager::EmployeeManager(const std::string& fileName) : fileName(fileName) {
    if (this->fileName.empty()) {
        this->fileName = "Administrators.txt";
    }
}

bool EmployeeManager::getPassword(const std::string& username, std::string& password) const {
    std::ifstream in(fileName);
    if (!in) return false;

    std::string user, pass;
    while (in >> user >> pass) {
        if (user == username) {
            password = pass;
            return true;
        }
    }
    return false;
}

bool EmployeeManager::authenticate(const std::string& username, const std::string& password) const {
    std::string storedPassword;
    return getPassword(username, storedPassword) && storedPassword == password;
}
