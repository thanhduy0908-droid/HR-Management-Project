#include "Employee.h"

#include <fstream>
#include <iostream>
#include <cstdio>
#include <vector>

Employee::Employee(const std::string& fileName) : fileName(fileName) {
    if (this->fileName.empty()) {
        this->fileName = "Employees.txt";
    }
}

bool Employee::getPassword(const std::string& username, std::string& password) const {
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

bool Employee::authenticate(const std::string& username, const std::string& password) const {
    std::string storedPassword;
    return getPassword(username, storedPassword) && storedPassword == password;
}

bool Employee::needsFirstLoginPasswordChange(const std::string& username) const {
    std::string storedPassword;
    return getPassword(username, storedPassword) && storedPassword == "111111";
}

bool Employee::changePassword(const std::string& username,
                                     const std::string& currentPassword,
                                     const std::string& newPassword) {
    if (newPassword.empty() || !authenticate(username, currentPassword)) return false;

    std::string targetFile = fileName.empty() ? "Employees.txt" : fileName;

    std::ifstream in(targetFile);
    if (!in) return false;

    std::vector<std::pair<std::string, std::string>> accounts;
    std::string user, pass;
    bool found = false;
    while (in >> user >> pass) {
        if (user == username) {
            pass = newPassword;
            found = true;
        }
        accounts.push_back({user, pass});
    }
    in.close();
    if (!found) return false;

    const std::string tempFile = targetFile + ".tmp";
    std::ofstream out(tempFile, std::ios::trunc);
    if (!out) return false;

    for (const auto& account : accounts) {
        out << account.first << ' ' << account.second << '\n';
    }
    out.close();

    std::remove(targetFile.c_str());

    if (std::rename(tempFile.c_str(), targetFile.c_str()) != 0) {
        std::remove(tempFile.c_str());
        return false;
    }
    return true;
}

bool Employee::displayProfile(const std::string& username) const {
    std::ifstream in(username + ".txt");
    if (!in) {
        std::cout << "\033[31mKhong tim thay file thong tin: " << username << ".txt\033[0m\n";
        return false;
    }

    const char* labels[] = {"Ho ten", "Dia chi", "So dien thoai", "Email"};
    std::string value;
    for (int i = 0; i < 4; ++i) {
        if (!std::getline(in, value)) value = "(chua cap nhat)";
        std::cout << "\033[36m" << labels[i] << ": \033[0m" << value << '\n';
    }
    return true;
}
