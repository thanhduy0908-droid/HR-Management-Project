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
