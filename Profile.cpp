#include "Profile.h"
#include <cctype>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

const std::string Profile::EMPLOYEES_FILE_NAME = "Employees.txt";
const std::string Profile::PROFILE_FILE_EXTENSION = ".txt";
const std::string Profile::SEPARATOR_LINE = "----------------------------------------";

namespace {
std::string trimSpaces(const std::string& strText) {
    const std::size_t iBegin = strText.find_first_not_of(" \t\r\n");
    if (iBegin == std::string::npos) {
        return "";
    }
    const std::size_t iEnd = strText.find_last_not_of(" \t\r\n");
    return strText.substr(iBegin, iEnd - iBegin + 1);
}
}

Profile::Profile() : _strUsername(""), _strFullName(""), _strAddress(""), _strPhoneNumber(""), _strEmail("") {}

bool Profile::loadFromFile(const std::string& strUsername) {
    const std::string strFilePath = strUsername + Profile::PROFILE_FILE_EXTENSION;
    std::ifstream fileProfile;
    fileProfile.open(strFilePath.c_str());
    if (!fileProfile.is_open()) {
        return false;
    }
    // Mỗi thông tin được lưu trên một dòng theo thứ tự: họ tên, địa chỉ, số điện thoại, email
    std::string strFullName = "";
    std::string strAddress = "";
    std::string strPhoneNumber = "";
    std::string strEmail = "";
    bool bResult = std::getline(fileProfile, strFullName) && std::getline(fileProfile, strAddress) && std::getline(fileProfile, strPhoneNumber) && std::getline(fileProfile, strEmail);
    fileProfile.close();
    if (!bResult) {
        return false;
    }
    this->_strUsername = strUsername;
    this->_strFullName = strFullName;
    this->_strAddress = strAddress;
    this->_strPhoneNumber = strPhoneNumber;
    this->_strEmail = strEmail;
    return true;
}

void Profile::display() const {
    std::cout << Profile::SEPARATOR_LINE << std::endl;
    std::cout << "Username : " << this->_strUsername << std::endl;
    std::cout << "Ho ten : " << this->_strFullName << std::endl;
    std::cout << "Dia chi : " << this->_strAddress << std::endl;
    std::cout << "So dien thoai : " << this->_strPhoneNumber << std::endl;
    std::cout << "Email : " << this->_strEmail << std::endl;
    std::cout << Profile::SEPARATOR_LINE << std::endl;
}

bool Profile::setFullName(const std::string& strFullName) {
    const std::string strValue = trimSpaces(strFullName);
    if (strValue.empty()) {
        return false;
    }
    this->_strFullName = strValue;
    return true;
}

bool Profile::setAddress(const std::string& strAddress) {
    const std::string strValue = trimSpaces(strAddress);
    if (strValue.empty()) {
        return false;
    }
    this->_strAddress = strValue;
    return true;
}

bool Profile::setPhoneNumber(const std::string& strPhoneNumber) {
    std::string strValue = "";
    if (!Profile::getPhoneNumber(strPhoneNumber, strValue)) {
        return false;
    }
    this->_strPhoneNumber = strValue;
    return true;
}

bool Profile::setEmail(const std::string& strEmail) {
    std::string strValue = "";
    if (!Profile::getEmail(strEmail, strValue)) {
        return false;
    }
    this->_strEmail = strValue;
    return true;
}

bool Profile::getPhoneNumber(const std::string& strInput, std::string& strPhoneNumber) {
    const std::string strValue = trimSpaces(strInput);
    if (!Profile::isValidPhoneNumber(strValue)) {
        return false;
    }
    strPhoneNumber = strValue;
    return true;
}

bool Profile::getEmail(const std::string& strInput, std::string& strEmail) {
    const std::string strValue = trimSpaces(strInput);
    if (!Profile::isValidEmail(strValue)) {
        return false;
    }
    strEmail = strValue;
    return true;
}

bool Profile::saveToFile() const {
    if (this->_strUsername.empty()) {
        return false;
    }
    const std::string strFilePath = this->_strUsername + Profile::PROFILE_FILE_EXTENSION;
    const std::string strTempPath = strFilePath + ".tmp";
    const std::string strBackupPath = strFilePath + ".bak";
    std::ifstream checkBackup(strBackupPath.c_str());
    if (checkBackup.good()) {
        return false;
    }
    checkBackup.close();
    std::ofstream outTemp(strTempPath.c_str(), std::ios::trunc);
    if (!outTemp) {
        return false;
    }
    outTemp << this->_strFullName << '\n'
            << this->_strAddress << '\n'
            << this->_strPhoneNumber << '\n'
            << this->_strEmail << '\n';
    outTemp.close();
    if (!outTemp) {
        std::remove(strTempPath.c_str());
        return false;
    }
    // Windows không cho rename đè lên file đã tồn tại nên phải chuyển file cũ sang .bak trước
    if (std::rename(strFilePath.c_str(), strBackupPath.c_str()) != 0) {
        std::remove(strTempPath.c_str());
        return false;
    }
    if (std::rename(strTempPath.c_str(), strFilePath.c_str()) != 0) {
        std::rename(strBackupPath.c_str(), strFilePath.c_str());
        std::remove(strTempPath.c_str());
        return false;
    }
    std::remove(strBackupPath.c_str());
    return true;
}

void Profile::searchByUsername(const std::string& strUsername) {
    LinkedList<std::string> usernames;
    if (!Profile::loadUsernameList(usernames)) {
        std::cout << "Loi: Khong mo duoc file " << Profile::EMPLOYEES_FILE_NAME << "!" << std::endl;
        return;
    }
    if (!Profile::containsUsername(usernames, strUsername)) {
        std::cout << "Khong tim thay employee co username '" << strUsername << "'!" << std::endl;
        return;
    }
    Profile profile;
    if (!profile.loadFromFile(strUsername)) {
        std::cout << "Loi: File thong tin cua '" << strUsername << "' khong ton tai hoac khong hop le!" << std::endl;
        return;
    }
    profile.display();
}

void Profile::displayAll() {
    LinkedList<std::string> usernames;
    if (!Profile::loadUsernameList(usernames)) {
        std::cout << "Loi: Khong mo duoc file " << Profile::EMPLOYEES_FILE_NAME << "!" << std::endl;
        return;
    }
    if (usernames.empty()) {
        std::cout << "Danh sach employee dang trong!" << std::endl;
        return;
    }
    Profile::displayList(usernames);
}

void Profile::viewOwnProfile(const std::string& strUsername) {
    Profile profile;
    if (!profile.loadFromFile(strUsername)) {
        std::cout << "Loi: Khong doc duoc file thong tin tai khoan cua ban!" << std::endl;
        return;
    }
    profile.display();
}

bool Profile::updateByUsername(const std::string& strUsername) {
    LinkedList<std::string> usernames;
    if (!Profile::loadUsernameList(usernames)) {
        std::cout << "Loi: Khong mo duoc file " << Profile::EMPLOYEES_FILE_NAME << "!" << std::endl;
        return false;
    }
    if (!Profile::containsUsername(usernames, strUsername)) {
        std::cout << "Khong tim thay employee co username '" << strUsername << "'!" << std::endl;
        return false;
    }
    Profile profile;
    if (!profile.loadFromFile(strUsername)) {
        std::cout << "Loi: File thong tin cua '" << strUsername << "' khong ton tai hoac khong hop le!" << std::endl;
        return false;
    }
    std::cout << "Thong tin hien tai:" << std::endl;
    profile.display();
    bool bChanged = false;
    while (true) {
        std::cout << "1. Ho ten" << std::endl
             << "2. Dia chi" << std::endl
             << "3. So dien thoai" << std::endl
             << "4. Email" << std::endl
             << "5. Luu va thoat" << std::endl
             << "0. Huy (khong luu)" << std::endl
             << "Chon thuoc tinh can cap nhat: ";
        int iChoice = -1;
        if (!(std::cin >> iChoice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Vui long nhap so tu 0 den 5." << std::endl;
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::string strInput = "";
        switch (iChoice) {
            case 1:
                std::cout << "Ho ten moi: ";
                std::getline(std::cin, strInput);
                if (profile.setFullName(strInput)) {
                    bChanged = true;
                    std::cout << "Da doi ho ten." << std::endl;
                } else {
                    std::cout << "Loi: Ho ten khong duoc de trong!" << std::endl;
                }
                break;
            case 2:
                std::cout << "Dia chi moi: ";
                std::getline(std::cin, strInput);
                if (profile.setAddress(strInput)) {
                    bChanged = true;
                    std::cout << "Da doi dia chi." << std::endl;
                } else {
                    std::cout << "Loi: Dia chi khong duoc de trong!" << std::endl;
                }
                break;
            case 3:
                std::cout << "So dien thoai moi: ";
                std::getline(std::cin, strInput);
                if (profile.setPhoneNumber(strInput)) {
                    bChanged = true;
                    std::cout << "Da doi so dien thoai." << std::endl;
                } else {
                    std::cout << "Loi: So dien thoai chi gom chu so va co tu 10 den 11 chu so!" << std::endl;
                }
                break;
            case 4:
                std::cout << "Email moi: ";
                std::getline(std::cin, strInput);
                if (profile.setEmail(strInput)) {
                    bChanged = true;
                    std::cout << "Da doi email." << std::endl;
                } else {
                    std::cout << "Loi: Email khong hop le (vi du: ten@congty.com)!" << std::endl;
                }
                break;
            case 5:
                if (!bChanged) {
                    std::cout << "Khong co thay doi nao de luu." << std::endl;
                    return false;
                }
                if (!profile.saveToFile()) {
                    std::cout << "Loi: Khong luu duoc file thong tin cua '" << strUsername << "'!" << std::endl;
                    return false;
                }
                std::cout << "Cap nhat thong tin thanh cong!" << std::endl;
                profile.display();
                return true;
            case 0:
                std::cout << "Da huy cap nhat." << std::endl;
                return false;
            default:
                std::cout << "Vui long nhap so tu 0 den 5." << std::endl;
        }
    }
}

bool Profile::loadUsernameList(LinkedList<std::string>& usernames) {
    std::ifstream fileEmployees;
    fileEmployees.open(Profile::EMPLOYEES_FILE_NAME.c_str());
    if (!fileEmployees.is_open()) {
        return false;
    }
    std::string strLine = "";
    while (std::getline(fileEmployees, strLine)) {
        std::istringstream streamLine(strLine);
        std::string strUsername = "";
        if (streamLine >> strUsername) {
            usernames.add(strUsername);
        }
    }
    fileEmployees.close();
    return true;
}

void Profile::displayByUsername(const std::string& strUsername) {
    Profile profile;
    if (profile.loadFromFile(strUsername)) {
        profile.display();
    } else {
        std::cout << "Canh bao: Khong doc duoc file thong tin cua '" << strUsername << "'!" << std::endl;
    }
}

bool Profile::isValidPhoneNumber(const std::string& strPhoneNumber) {
    if (strPhoneNumber.size() < 10 || strPhoneNumber.size() > 11) {
        return false;
    }
    for (std::size_t i = 0; i < strPhoneNumber.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(strPhoneNumber[i]))) {
            return false;
        }
    }
    return true;
}

bool Profile::isValidEmail(const std::string& strEmail) {
    const std::size_t iAt = strEmail.find('@');
    if (iAt == std::string::npos || iAt == 0) {
        return false;
    }
    if (strEmail.find('@', iAt + 1) != std::string::npos || strEmail.find(' ') != std::string::npos) {
        return false;
    }
    const std::size_t iDot = strEmail.find_last_of('.');
    return iDot != std::string::npos && iDot > iAt + 1 && iDot + 1 < strEmail.size();
}
