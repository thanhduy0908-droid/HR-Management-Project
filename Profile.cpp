#include "Profile.h"
#include <cctype>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

const string Profile::EMPLOYEES_FILE_NAME = "Employees.txt";
const string Profile::PROFILE_FILE_EXTENSION = ".txt";
const string Profile::SEPARATOR_LINE = "----------------------------------------";

namespace {
string trimSpaces(const string& strText) {
    const size_t iBegin = strText.find_first_not_of(" \t\r\n");
    if (iBegin == string::npos) {
        return "";
    }
    const size_t iEnd = strText.find_last_not_of(" \t\r\n");
    return strText.substr(iBegin, iEnd - iBegin + 1);
}
}

Profile::Profile() : _strUsername(""), _strFullName(""), _strAddress(""), _strPhoneNumber(""), _strEmail("") {}

bool Profile::loadFromFile(const string& strUsername) {
    const string strFilePath = strUsername + Profile::PROFILE_FILE_EXTENSION;
    ifstream fileProfile;
    fileProfile.open(strFilePath.c_str());
    if (!fileProfile.is_open()) {
        return false;
    }
    // Mỗi thông tin được lưu trên một dòng theo thứ tự: họ tên, địa chỉ, số điện thoại, email
    string strFullName = "";
    string strAddress = "";
    string strPhoneNumber = "";
    string strEmail = "";
    bool bResult = getline(fileProfile, strFullName) && getline(fileProfile, strAddress) && getline(fileProfile, strPhoneNumber) && getline(fileProfile, strEmail);
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
    cout << Profile::SEPARATOR_LINE << endl;
    cout << "Username : " << this->_strUsername << endl;
    cout << "Ho ten : " << this->_strFullName << endl;
    cout << "Dia chi : " << this->_strAddress << endl;
    cout << "So dien thoai : " << this->_strPhoneNumber << endl;
    cout << "Email : " << this->_strEmail << endl;
    cout << Profile::SEPARATOR_LINE << endl;
}

bool Profile::setFullName(const string& strFullName) {
    const string strValue = trimSpaces(strFullName);
    if (strValue.empty()) {
        return false;
    }
    this->_strFullName = strValue;
    return true;
}

bool Profile::setAddress(const string& strAddress) {
    const string strValue = trimSpaces(strAddress);
    if (strValue.empty()) {
        return false;
    }
    this->_strAddress = strValue;
    return true;
}

bool Profile::setPhoneNumber(const string& strPhoneNumber) {
    string strValue = "";
    if (!Profile::getPhoneNumber(strPhoneNumber, strValue)) {
        return false;
    }
    this->_strPhoneNumber = strValue;
    return true;
}

bool Profile::setEmail(const string& strEmail) {
    string strValue = "";
    if (!Profile::getEmail(strEmail, strValue)) {
        return false;
    }
    this->_strEmail = strValue;
    return true;
}

bool Profile::getPhoneNumber(const string& strInput, string& strPhoneNumber) {
    const string strValue = trimSpaces(strInput);
    if (!Profile::isValidPhoneNumber(strValue)) {
        return false;
    }
    strPhoneNumber = strValue;
    return true;
}

bool Profile::getEmail(const string& strInput, string& strEmail) {
    const string strValue = trimSpaces(strInput);
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
    const string strFilePath = this->_strUsername + Profile::PROFILE_FILE_EXTENSION;
    const string strTempPath = strFilePath + ".tmp";
    const string strBackupPath = strFilePath + ".bak";
    ifstream checkBackup(strBackupPath.c_str());
    if (checkBackup.good()) {
        return false;
    }
    checkBackup.close();
    ofstream outTemp(strTempPath.c_str(), ios::trunc);
    if (!outTemp) {
        return false;
    }
    outTemp << this->_strFullName << '\n'
            << this->_strAddress << '\n'
            << this->_strPhoneNumber << '\n'
            << this->_strEmail << '\n';
    outTemp.close();
    if (!outTemp) {
        remove(strTempPath.c_str());
        return false;
    }
    // Windows không cho rename đè lên file đã tồn tại nên phải chuyển file cũ sang .bak trước
    if (rename(strFilePath.c_str(), strBackupPath.c_str()) != 0) {
        remove(strTempPath.c_str());
        return false;
    }
    if (rename(strTempPath.c_str(), strFilePath.c_str()) != 0) {
        rename(strBackupPath.c_str(), strFilePath.c_str());
        remove(strTempPath.c_str());
        return false;
    }
    remove(strBackupPath.c_str());
    return true;
}

void Profile::searchByUsername(const string& strUsername) {
    vector<string> vecUsernames;
    if (!Profile::loadUsernameList(vecUsernames)) {
        cout << "Loi: Khong mo duoc file " << Profile::EMPLOYEES_FILE_NAME << "!" << endl;
        return;
    }
    if (!Profile::containsUsername(vecUsernames, strUsername)) {
        cout << "Khong tim thay employee co username '" << strUsername << "'!" << endl;
        return;
    }
    Profile profile;
    if (!profile.loadFromFile(strUsername)) {
        cout << "Loi: File thong tin cua '" << strUsername << "' khong ton tai hoac khong hop le!" << endl;
        return;
    }
    profile.display();
}

void Profile::displayAll() {
    vector<string> vecUsernames;
    if (!Profile::loadUsernameList(vecUsernames)) {
        cout << "Loi: Khong mo duoc file " << Profile::EMPLOYEES_FILE_NAME << "!" << endl;
        return;
    }
    if (vecUsernames.empty()) {
        cout << "Danh sach employee dang trong!" << endl;
        return;
    }
    Profile::displayList(vecUsernames);
}

void Profile::viewOwnProfile(const string& strUsername) {
    Profile profile;
    if (!profile.loadFromFile(strUsername)) {
        cout << "Loi: Khong doc duoc file thong tin tai khoan cua ban!" << endl;
        return;
    }
    profile.display();
}

bool Profile::updateByUsername(const string& strUsername) {
    vector<string> vecUsernames;
    if (!Profile::loadUsernameList(vecUsernames)) {
        cout << "Loi: Khong mo duoc file " << Profile::EMPLOYEES_FILE_NAME << "!" << endl;
        return false;
    }
    if (!Profile::containsUsername(vecUsernames, strUsername)) {
        cout << "Khong tim thay employee co username '" << strUsername << "'!" << endl;
        return false;
    }
    Profile profile;
    if (!profile.loadFromFile(strUsername)) {
        cout << "Loi: File thong tin cua '" << strUsername << "' khong ton tai hoac khong hop le!" << endl;
        return false;
    }
    cout << "Thong tin hien tai:" << endl;
    profile.display();
    bool bChanged = false;
    while (true) {
        cout << "1. Ho ten" << endl
             << "2. Dia chi" << endl
             << "3. So dien thoai" << endl
             << "4. Email" << endl
             << "5. Luu va thoat" << endl
             << "0. Huy (khong luu)" << endl
             << "Chon thuoc tinh can cap nhat: ";
        int iChoice = -1;
        if (!(cin >> iChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Vui long nhap so tu 0 den 5." << endl;
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string strInput = "";
        switch (iChoice) {
            case 1:
                cout << "Ho ten moi: ";
                getline(cin, strInput);
                if (profile.setFullName(strInput)) {
                    bChanged = true;
                    cout << "Da doi ho ten." << endl;
                } else {
                    cout << "Loi: Ho ten khong duoc de trong!" << endl;
                }
                break;
            case 2:
                cout << "Dia chi moi: ";
                getline(cin, strInput);
                if (profile.setAddress(strInput)) {
                    bChanged = true;
                    cout << "Da doi dia chi." << endl;
                } else {
                    cout << "Loi: Dia chi khong duoc de trong!" << endl;
                }
                break;
            case 3:
                cout << "So dien thoai moi: ";
                getline(cin, strInput);
                if (profile.setPhoneNumber(strInput)) {
                    bChanged = true;
                    cout << "Da doi so dien thoai." << endl;
                } else {
                    cout << "Loi: So dien thoai chi gom chu so va co tu 10 den 11 chu so!" << endl;
                }
                break;
            case 4:
                cout << "Email moi: ";
                getline(cin, strInput);
                if (profile.setEmail(strInput)) {
                    bChanged = true;
                    cout << "Da doi email." << endl;
                } else {
                    cout << "Loi: Email khong hop le (vi du: ten@congty.com)!" << endl;
                }
                break;
            case 5:
                if (!bChanged) {
                    cout << "Khong co thay doi nao de luu." << endl;
                    return false;
                }
                if (!profile.saveToFile()) {
                    cout << "Loi: Khong luu duoc file thong tin cua '" << strUsername << "'!" << endl;
                    return false;
                }
                cout << "Cap nhat thong tin thanh cong!" << endl;
                profile.display();
                return true;
            case 0:
                cout << "Da huy cap nhat." << endl;
                return false;
            default:
                cout << "Vui long nhap so tu 0 den 5." << endl;
        }
    }
}

bool Profile::loadUsernameList(vector<string>& vecUsernames) {
    ifstream fileEmployees;
    fileEmployees.open(Profile::EMPLOYEES_FILE_NAME.c_str());
    if (!fileEmployees.is_open()) {
        return false;
    }
    string strLine = "";
    while (getline(fileEmployees, strLine)) {
        istringstream streamLine(strLine);
        string strUsername = "";
        if (streamLine >> strUsername) {
            vecUsernames.push_back(strUsername);
        }
    }
    fileEmployees.close();
    return true;
}

void Profile::displayByUsername(const string& strUsername) {
    Profile profile;
    if (profile.loadFromFile(strUsername)) {
        profile.display();
    } else {
        cout << "Canh bao: Khong doc duoc file thong tin cua '" << strUsername << "'!" << endl;
    }
}

bool Profile::isValidPhoneNumber(const string& strPhoneNumber) {
    if (strPhoneNumber.size() < 10 || strPhoneNumber.size() > 11) {
        return false;
    }
    for (size_t i = 0; i < strPhoneNumber.size(); ++i) {
        if (!isdigit(static_cast<unsigned char>(strPhoneNumber[i]))) {
            return false;
        }
    }
    return true;
}

bool Profile::isValidEmail(const string& strEmail) {
    const size_t iAt = strEmail.find('@');
    if (iAt == string::npos || iAt == 0) {
        return false;
    }
    if (strEmail.find('@', iAt + 1) != string::npos || strEmail.find(' ') != string::npos) {
        return false;
    }
    const size_t iDot = strEmail.find_last_of('.');
    return iDot != string::npos && iDot > iAt + 1 && iDot + 1 < strEmail.size();
}
