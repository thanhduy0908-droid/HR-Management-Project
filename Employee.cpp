#include "Employee.h"

#include <fstream>
#include <iostream>
#include <cstdio>
#include <vector>

/*************************************************************
 * @Description Khởi tạo đối tượng Employee với tên file dữ liệu nhân viên
 *
 * @return      Không có (Constructor)
 * @attention   Mặc định tên file là "Employees.txt" nếu chuỗi truyền vào rỗng
 *************************************************************/
Employee::Employee(const std::string& strFileName) : _strFileName(strFileName) {
    // Kiểm tra nếu tên file trống thì gán giá trị mặc định để tránh lỗi
    if (this->_strFileName.empty()) {
        this->_strFileName = "Employees.txt";
    }
}

/*************************************************************
 * @Description Lấy mật khẩu đã lưu của nhân viên từ file dữ liệu
 *
 * @return      true nếu tìm thấy username, ngược lại trả về false
 * @attention   Đọc tuần tự từng dòng trong file để dò khớp username
 * @attention   Trả về mật khẩu thông qua tham chiếu strPassword
 *************************************************************/
bool Employee::getPassword(const std::string& strUsername, std::string& strPassword) const {
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
 * @Description Xác thực tài khoản nhân viên dựa trên username và password
 *
 * @return      true nếu thông tin đăng nhập chính xác, ngược lại trả về false
 * @attention   Sử dụng hàm getPassword để lấy mật khẩu đã lưu và đối chiếu
 *************************************************************/
bool Employee::authenticateEmployee(const std::string& strUsername, const std::string& strPassword) const {
    std::string strStoredPassword;
    // Lấy password của nhân viên và kiểm tra với password được nhập vào. Nếu trùng thì trả về true, ngược lại trả về false
    return getPassword(strUsername, strStoredPassword) && strStoredPassword == strPassword;
}

/*************************************************************
 * @Description Kiểm tra xem nhân viên có đang dùng mật khẩu mặc định lần đầu không
 *
 * @return      true nếu mật khẩu hiện tại là "111111", ngược lại trả về false
 * @attention   Dùng để bắt buộc người dùng đổi mật khẩu ngay trong lần đăng nhập đầu tiên
 *************************************************************/
bool Employee::needsFirstLoginPasswordChange(const std::string& strUsername) const {
    std::string strStoredPassword;
    return getPassword(strUsername, strStoredPassword) && strStoredPassword == "111111";
}

/*************************************************************
 * @Description Thay đổi mật khẩu cho nhân viên và lưu vào file
 *
 * @return      true nếu đổi mật khẩu thành công, false nếu sai mật khẩu hiện tại hoặc lỗi file
 * @attention   Sử dụng RecordList template để lưu trữ bộ nhớ tạm thời
 * @attention   Sử dụng file tạm (.tmp) để ghi dữ liệu an toàn trước khi thay thế file chính
 *************************************************************/

bool Employee::changePassword(const std::string& strUsername,
                              const std::string& strCurrentPassword,
                              const std::string& strNewPassword) {
    // Kiểm tra mật khẩu mới không rỗng và xác thực mật khẩu hiện tại có đúng không
    if (strNewPassword.empty() || !authenticateEmployee(strUsername, strCurrentPassword)) {
        return false;
    }

    std::string strTargetFile = _strFileName.empty() ? "Employees.txt" : _strFileName;

    std::ifstream inFile(strTargetFile);
    if (!inFile) {
        return false;
    }

    RecordList<std::pair<std::string, std::string>> employeeList;
    std::string strUser, strPass;
    bool bFound = false;

    // Đọc dữ liệu cũ vào RecordList và cập nhật mật khẩu mới cho đúng user
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

    // Lặp qua danh sách để ghi ra file tạm
    for (const auto& employee : employeeList.getItems()) {
        outFile << employee.first << ' ' << employee.second << '\n';
    }
    outFile.close();

    std::remove(strTargetFile.c_str());

    // Đổi tên file tạm thành file chính để hoàn tất quá trình thay đổi mật khẩu
    if (std::rename(strTempFile.c_str(), strTargetFile.c_str()) != 0) {
        std::remove(strTempFile.c_str());
        return false;
    }
    return true;
}

/*************************************************************
 * @Description Hiển thị thông tin profile cá nhân của nhân viên từ file riêng
 *
 * @return      true nếu đọc file thành công, false nếu không tìm thấy file
 * @attention   Đọc lần lượt 4 thông tin: Ho ten, Dia chi, So dien thoai, Email
 *************************************************************/
