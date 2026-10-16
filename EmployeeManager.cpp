#include "EmployeeManager.h"
#include "Employee.h"

#include <fstream>
#include <iostream>

namespace {
    const std::string EMPLOYEE_FILE = "Employees.txt";
    const std::string DEFAULT_PASSWORD = "111111";

    bool isValidUsername(const std::string& strUsername) {
        if (strUsername.empty()) {
            return false;
        }
        for (char cCharacter : strUsername) {
            if (!((cCharacter >= 'a' && cCharacter <= 'z') || (cCharacter >= 'A' && cCharacter <= 'Z') || (cCharacter >= '0' && cCharacter <= '9') || cCharacter == '_' || cCharacter == '-')) {
                return false;
            }
        }
        return true;
    }
}

/*************************************************************
 * @Description Khởi tạo đối tượng EmployeeManager với tên file dữ liệu Admin
 *
 * @return      Không có (Constructor)
 * @attention   Mặc định tên file là "Administrators.txt" nếu chuỗi truyền vào rỗng
 *************************************************************/
EmployeeManager::EmployeeManager(const std::string& strFileName) : _strFileName(strFileName) {
    // Kiểm tra nếu tên file trống thì gán giá trị mặc định để tránh lỗi
    if (this->_strFileName.empty()) {
        this->_strFileName = "Administrators.txt";
    }
}

/*************************************************************
 * @Description Lấy mật khẩu đã lưu của Admin từ file dữ liệu
 *
 * @return      true nếu tìm thấy username, ngược lại trả về false
 * @attention   Đọc tuần tự từng dòng trong file để dò khớp username
 * @attention   Trả về mật khẩu thông qua tham chiếu strPassword
 *************************************************************/
bool EmployeeManager::getPassword(const std::string& strUsername, std::string& strPassword) const {
    std::ifstream inFile(_strFileName);
    if (!inFile) {
        return false;
    }

    std::string strUser, strPass;
    while (inFile >> strUser >> strPass) {
        // Khớp username hiện tại trong file với username cần tìm
        if (strUser == strUsername) {
            strPassword = strPass;
            inFile.close();
            return true;
        }
    }
    inFile.close();
    return false;
}

/*************************************************************
 * @Description Xác thực tài khoản Admin dựa trên username và password
 *
 * @return      true nếu thông tin đăng nhập chính xác, ngược lại trả về false
 * @attention   Sử dụng hàm getPassword để lấy mật khẩu đã lưu và đối chiếu
 *************************************************************/
bool EmployeeManager::authenticateManager(const std::string& strUsername, const std::string& strPassword) const {
    std::string strStoredPassword;
    // Lấy password của Admin và kiểm tra với password được nhập vào. Nếu trùng thì trả về true, ngược lại trả về false
    return getPassword(strUsername, strStoredPassword) && strStoredPassword == strPassword;
}
//add employee
bool EmployeeManager::addEmployee (const std::string& strUsername, const std::string& strFullName, const std::string& strAddress, const std::string& strPhone, const std::string& strEmail) const {
    if (!isValidUsername(strUsername)) {
        std::cout << "Username khong hop le (chi dung chu, so, _ va -).\n";
        return false;
    }
    if (strFullName.empty() || strAddress.empty() || strPhone.empty() || strEmail.empty()) {
        std::cout << "Khong duoc de trong thong tin nhan vien.\n";
        return false;
    }
    std::ifstream inFile(EMPLOYEE_FILE);
    if (!inFile) {
        std::cout << "Khong mo duoc Employees.txt. Hay tao file truoc.\n";
        return false;
    }
    std::string strUser, strPass;
    while (inFile >> strUser >> strPass) {
        if (strUser == strUsername) {
            std::cout << "Username da ton tai.\n";
            return false;
        }
    }
    if (inFile.bad()) {
        std::cout << "Loi doc Employees.txt.\n";
        return false;
    }
    inFile.clear();
    inFile.seekg(0, std::ios::end);
    bool bNeedsNewline = false;
    if (inFile.tellg() > 0) {
        inFile.seekg(-1, std::ios::end);
        char cLastCharacter = '\0';
        inFile.get(cLastCharacter);
        bNeedsNewline = (cLastCharacter != '\n');
    }
    inFile.close();
    
    const std::string strProfileFile = strUsername + ".txt";
    std::ifstream checkProfile(strProfileFile);
    if (checkProfile.good()) {
        std::cout << "File thong tin cua username nay da ton tai.\n";
        return false;
    }
    checkProfile.close();
    
    std::ofstream outProfile(strProfileFile);
    if (!outProfile) {
        std::cout << "Khong tao duoc file thong tin.\n";
        return false;
    }
    outProfile << strFullName << '\n'
               << strAddress << '\n'
               << strPhone << '\n'
               << strEmail << '\n';
    outProfile.close();
    if (!outProfile) {
        std::remove(strProfileFile.c_str());
        std::cout << "Loi ghi file thong tin.\n";
        return false;
    }

    std::ofstream outEmployees(EMPLOYEE_FILE, std::ios::app);
    if (!outEmployees) {
        std::remove(strProfileFile.c_str());
        std::cout << "Khong mo duoc Employees.txt de ghi.\n";
        return false;
    }
    if (bNeedsNewline) {
        outEmployees << '\n';
    }
    outEmployees << strUsername << ' ' << DEFAULT_PASSWORD << '\n';
    outEmployees.close();
    if (!outEmployees) {
        // Trong truong hop loi ghi giua chung, can kiem tra Employees.txt thu cong.
        std::cout << "Loi ghi Employees.txt, can kiem tra lai file.\n";
        return false;
    }
    return true;
}

//remove employee
bool EmployeeManager::removeEmployee(const std::string& strUsername) const {
    if (!isValidUsername(strUsername)) {
        std::cout << "Username khong hop le.\n";
        return false;
    }

    std::ifstream inFile(EMPLOYEE_FILE);
    if (!inFile) {
        std::cout << "Khong mo duoc Employees.txt.\n";
        return false;
    }
    RecordList<std::pair<std::string, std::string>> remaining;
    std::string strUser, strPass;
    bool bFound = false;
    while (inFile >> strUser >> strPass) {
        if (strUser == strUsername) {
            bFound = true;
        } else {
            remaining.add({strUser, strPass});
        }
    }
    if (inFile.bad()) {
        std::cout << "Loi doc Employees.txt.\n";
        return false;
    }
    inFile.close();
    if (!bFound) {
        std::cout << "Khong tim thay Employee.\n";
        return false;
    }

    const std::string strProfileFile = strUsername + ".txt";
    std::ifstream checkProfile(strProfileFile);
    if (!checkProfile) {
        std::cout << "Thieu file thong tin, dung xoa de tranh mat dong bo.\n";
        return false;
    }
    checkProfile.close();

    const std::string strTempFile = EMPLOYEE_FILE + ".tmp";
    const std::string strBackupFile = EMPLOYEE_FILE + ".bak";
    std::ifstream checkBackup(strBackupFile);
    if (checkBackup.good()) {
        std::cout << "Dang co file sao luu .bak, can kiem tra truoc.\n";
        return false;
    }
    checkBackup.close();

    std::ofstream outFile(strTempFile, std::ios::trunc);
    if (!outFile) {
        std::cout << "Khong tao duoc file tam.\n";
        return false;
    }
    for (const auto& employee : remaining.getItems()) {
        outFile << employee.first << ' ' << employee.second << '\n';
    }
    outFile.close();
    if (!outFile) {
        std::remove(strTempFile.c_str());
        std::cout << "Loi ghi file tam.\n";
        return false;
    }

    if (std::rename(EMPLOYEE_FILE.c_str(), strBackupFile.c_str()) != 0) {
        std::remove(strTempFile.c_str());
        std::cout << "Khong tao duoc ban sao luu Employees.txt.\n";
        return false;
    }
    if (std::rename(strTempFile.c_str(), EMPLOYEE_FILE.c_str()) != 0) {
        std::rename(strBackupFile.c_str(), EMPLOYEE_FILE.c_str());
        std::remove(strTempFile.c_str());
        std::cout << "Khong cap nhat duoc Employees.txt.\n";
        return false;
    }
    if (std::remove(strProfileFile.c_str()) != 0) {
        std::remove(EMPLOYEE_FILE.c_str());
        std::rename(strBackupFile.c_str(), EMPLOYEE_FILE.c_str());
        std::cout << "Khong xoa duoc file thong tin; da thu khoi phuc danh sach.\n";
        return false;
    }
    std::remove(strBackupFile.c_str());
    return true;
}
