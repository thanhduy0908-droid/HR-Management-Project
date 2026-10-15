#include "Employee.h"

#include <fstream>
#include <iostream>
#include <cstdio>
#include <vector>

Employee::Employee(const std::string& strFileName) : strFileName(strFileName) {
    if (this->strFileName.empty()) {
        this->strFileName = "Employees.txt";
    }
}

bool Employee::getPassword(const std::string& strUsername, std::string& strPassword) const {
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

bool Employee::authenticateEmployee(const std::string& strUsername, const std::string& strPassword) const {
    std::string strStoredPassword;
    return getPassword(strUsername, strStoredPassword) && strStoredPassword == strPassword;
}

bool Employee::needsFirstLoginPasswordChange(const std::string& strUsername) const {
    std::string strStoredPassword;
    return getPassword(strUsername, strStoredPassword) && strStoredPassword == "111111";
}

bool Employee::changePassword(const std::string& strUsername,
                              const std::string& strCurrentPassword,
                              const std::string& strNewPassword) {
    if (strNewPassword.empty() || !authenticateEmployee(strUsername, strCurrentPassword)) {
        return false;
    }

    std::string strTargetFile = strFileName.empty() ? "Employees.txt" : strFileName;

    std::ifstream inFile(strTargetFile);
    if (!inFile) {
        return false;
    }

    RecordList<std::pair<std::string, std::string>> employeeList;
    std::string strUser, strPass;
    bool bFound = false;

    while (inFile >> strUser >> strPass) {
        if (strUser == strUsername) {
            strPass = strNewPassword;
            bFound = true;
        }
        employeeList.add({strUser, strPass});
    }
    inFile.close();

    if (!bFound) {
        return false;
    }

    const std::string strTempFile = strTargetFile + ".tmp";
    std::ofstream outFile(strTempFile, std::ios::trunc);
    if (!outFile) {
        return false;
    }

    // Lặp qua danh sách từ Template Collection để ghi ra file
    for (const auto& employee : employeeList.getItems()) {
        outFile << employee.first << ' ' << employee.second << '\n';
    }
    outFile.close();

    std::remove(strTargetFile.c_str());

    if (std::rename(strTempFile.c_str(), strTargetFile.c_str()) != 0) {
        std::remove(strTempFile.c_str());
        return false;
    }
    return true;
}

bool Employee::displayProfile(const std::string& strUsername) const {
    std::ifstream inFile(strUsername + ".txt");
    if (!inFile) {
        std::cout << "\033[31mKhong tim thay file thong tin: " << strUsername << ".txt\033[0m\n";
        return false;
    }

    const char* szLabels[] = {"Ho ten", "Dia chi", "So dien thoai", "Email"};
    std::string strValue;
    for (int i = 0; i < 4; ++i) {
        if (!std::getline(inFile, strValue)) {
            strValue = "(chua cap nhat)";
        }
        std::cout << "\033[36m" << szLabels[i] << ": \033[0m" << strValue << '\n';
    }
    inFile.close();
    return true;
}
