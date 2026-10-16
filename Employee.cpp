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
