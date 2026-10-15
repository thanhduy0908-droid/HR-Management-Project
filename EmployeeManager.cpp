#include "EmployeeManager.h"
#include "Employee.h"

#include <fstream>
#include <iostream>

EmployeeManager::EmployeeManager(const std::string& strFileName) : strFileName(strFileName) {
    if (this->strFileName.empty()) {
        this->strFileName = "Administrators.txt";
    }
}

bool EmployeeManager::getPassword(const std::string& strUsername, std::string& strPassword) const {
    std::ifstream inFile(strFileName);
    if (!inFile) {
        return false;
    }

    std::string strUser, strPass;
    while (inFile >> strUser >> strPass) {
        if (strUser == strUsername) {
            strPassword = strPass;
            inFile.close();
            return true;
        }
    }
    inFile.close();
    return false;
}

bool EmployeeManager::authenticateManager(const std::string& strUsername, const std::string& strPassword) const {
    std::string strStoredPassword;
    return getPassword(strUsername, strStoredPassword) && strStoredPassword == strPassword;
}
