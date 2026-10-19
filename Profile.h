#ifndef PROFILE_H_INCLUDED_
#define PROFILE_H_INCLUDED_

#include <string>
#include <vector>

class Profile {
public:
    Profile();

    /**********************************************************
     * @Description Đọc thông tin employee từ file [username].txt
     *
     * @return true nếu đọc thành công, false nếu có lỗi
     * @attention Trả về false nếu file không tồn tại hoặc thiếu dữ liệu
     * @attention Dữ liệu của đối tượng chỉ thay đổi khi đọc thành công
     **********************************************************/
    bool loadFromFile(const std::string& strUsername);

    /**********************************************************
     * @Description Hiển thị đầy đủ thông tin của employee
     *
     * @return Không có
     **********************************************************/
    void display() const;

    /**********************************************************
     * @Description Đặt họ tên mới cho employee
     *
     * @return true nếu hợp lệ và đã cập nhật, false nếu không hợp lệ
     * @attention Họ tên không được để trống (sau khi cắt khoảng trắng hai đầu)
     * @attention Dữ liệu của đối tượng chỉ thay đổi khi hợp lệ
     **********************************************************/
    bool setFullName(const std::string& strFullName);

    /**********************************************************
     * @Description Đặt địa chỉ mới cho employee
     *
     * @return true nếu hợp lệ và đã cập nhật, false nếu không hợp lệ
     * @attention Địa chỉ không được để trống (sau khi cắt khoảng trắng hai đầu)
     **********************************************************/
    bool setAddress(const std::string& strAddress);

    /**********************************************************
     * @Description Đặt số điện thoại mới cho employee
     *
     * @return true nếu hợp lệ và đã cập nhật, false nếu không hợp lệ
     * @attention Số điện thoại chỉ gồm chữ số và có từ 9 đến 11 chữ số
     **********************************************************/
    bool setPhoneNumber(const std::string& strPhoneNumber);

    /**********************************************************
     * @Description Đặt email mới cho employee
     *
     * @return true nếu hợp lệ và đã cập nhật, false nếu không hợp lệ
     * @attention Email không có khoảng trắng, có đúng một dấu @
     * và có dấu chấm nằm sau @ (không ở ngay sau @ hoặc cuối chuỗi)
     **********************************************************/
    bool setEmail(const std::string& strEmail);

    /**********************************************************
     * @Description Kiểm tra và chuẩn hóa số điện thoại do người dùng nhập
     *
     * @return true nếu hợp lệ, false nếu không hợp lệ
     * @attention Cắt khoảng trắng hai đầu trước khi kiểm tra
     * @attention Số điện thoại chỉ gồm chữ số và có từ 9 đến 11 chữ số
     * @attention Trả về số điện thoại đã chuẩn hóa thông qua tham chiếu strPhoneNumber
     * và không thay đổi strPhoneNumber nếu không hợp lệ
     **********************************************************/
    static bool getPhoneNumber(const std::string& strInput, std::string& strPhoneNumber);

    /**********************************************************
     * @Description Kiểm tra và chuẩn hóa email do người dùng nhập
     *
     * @return true nếu hợp lệ, false nếu không hợp lệ
     * @attention Cắt khoảng trắng hai đầu trước khi kiểm tra
     * @attention Email không có khoảng trắng, có đúng một dấu @
     * và có dấu chấm nằm sau @ (không ở ngay sau @ hoặc cuối chuỗi)
     * @attention Trả về email đã chuẩn hóa thông qua tham chiếu strEmail
     * và không thay đổi strEmail nếu không hợp lệ
     **********************************************************/
    static bool getEmail(const std::string& strInput, std::string& strEmail);

    /**********************************************************
     * @Description Ghi thông tin hiện tại của employee vào file [username].txt
     *
     * @return true nếu ghi thành công, false nếu có lỗi
     * @attention Ghi ra file tạm rồi mới thay file cũ để lỗi giữa chừng không làm hỏng dữ liệu
     * @attention Trả về false nếu đang tồn tại file sao lưu .bak chưa được xử lý
     **********************************************************/
    bool saveToFile() const;

    /**********************************************************
     * @Description Kiểm tra username có nằm trong container hay không
     *
     * @return true nếu tìm thấy, false nếu không tìm thấy
     * @attention Container cần có const_iterator, begin() và end()
     * @attention Định nghĩa template nằm ở cuối file Profile.h
     **********************************************************/
    template <typename Container>
    static bool containsUsername(const Container& containerUsernames, const std::string& strUsername);

    /**********************************************************
     * @Description Hiển thị thông tin của các employee trong container
     *
     * @return Không có
     * @attention Container cần có const_iterator, begin() và end()
     * @attention Cảnh báo và bỏ qua employee nào thiếu file thông tin
     * @attention Định nghĩa template nằm ở cuối file Profile.h
     **********************************************************/
    template <typename Container>
    static void displayList(const Container& containerUsernames);

    /**********************************************************
     * @Description Tìm employee theo username và hiển thị thông tin
     *
     * @return Không có
     * @attention In thông báo lỗi nếu không mở được Employees.txt
     * @attention In thông báo lỗi nếu không tìm thấy username
     * @attention In thông báo lỗi nếu file [username].txt không tồn tại
     **********************************************************/
    static void searchByUsername(const std::string& strUsername);

    /**********************************************************
     * @Description Hiển thị thông tin toàn bộ employee
     *
     * @return Không có
     * @attention In thông báo lỗi nếu không mở được Employees.txt
     **********************************************************/
    static void displayAll();

    /**********************************************************
     * @Description Employee xem thông tin tài khoản của chính mình
     *
     * @return Không có
     * @attention strUsername là username của employee đang đăng nhập
     * @attention In thông báo lỗi nếu file [username].txt không tồn tại
     **********************************************************/
    static void viewOwnProfile(const std::string& strUsername);

    /**********************************************************
     * @Description Cập nhật thông tin của employee theo username
     *
     * @return true nếu đã lưu thay đổi thành công, false nếu hủy hoặc có lỗi
     * @attention Cho chọn một hoặc nhiều thuộc tính: họ tên, địa chỉ, số điện thoại, email
     * @attention Chỉ ghi vào file [username].txt khi người dùng chọn lưu
     * @attention In thông báo lỗi nếu không tìm thấy username hoặc file thông tin không hợp lệ
     **********************************************************/
    static bool updateByUsername(const std::string& strUsername);

private:
    /**********************************************************
     * @Description Đọc danh sách username từ file Employees.txt
     *
     * @return true nếu đọc thành công, false nếu không mở được file
     * @attention Mỗi dòng của file bắt đầu bằng username
     **********************************************************/
    static bool loadUsernameList(std::vector<std::string>& vecUsernames);

    /**********************************************************
     * @Description Đọc và hiển thị thông tin của một employee
     *
     * @return Không có
     * @attention In cảnh báo nếu không đọc được file thông tin
     **********************************************************/
    static void displayByUsername(const std::string& strUsername);

    static bool isValidPhoneNumber(const std::string& strPhoneNumber);

    static bool isValidEmail(const std::string& strEmail);

    static const std::string EMPLOYEES_FILE_NAME;

    static const std::string PROFILE_FILE_EXTENSION;

    static const std::string SEPARATOR_LINE;

    std::string _strUsername;

    std::string _strFullName;

    std::string _strAddress;

    std::string _strPhoneNumber;

    std::string _strEmail;
};

template <typename Container>
bool Profile::containsUsername(const Container& containerUsernames, const std::string& strUsername) {
    typename Container::const_iterator iter = containerUsernames.begin();
    for (; iter != containerUsernames.end(); ++iter) {
        if (*iter == strUsername) {
            return true;
        }
    }
    return false;
}

template <typename Container>
void Profile::displayList(const Container& containerUsernames) {
    typename Container::const_iterator iter = containerUsernames.begin();
    for (; iter != containerUsernames.end(); ++iter) {
        Profile::displayByUsername(*iter);
    }
}

#endif
