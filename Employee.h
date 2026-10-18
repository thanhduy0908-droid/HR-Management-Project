#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include <string>
#include <vector>
#include <utility>

template <class DataType>
class RecordList {
private:
    std::vector<DataType> _items; // Danh sách các phần tử dữ liệu
public:
    /*************************************************************
     * @Description Thêm phần tử vào danh sách
     * @attention   Sử dụng push_back để thêm vào cuối vector
     *************************************************************/
    void add(const DataType& item);

    /*************************************************************
     * @Description Lấy tham chiếu đến danh sách các phần tử
     * @return      Vector chứa các phần tử dữ liệu
     *************************************************************/
    const std::vector<DataType>& getItems() const;

    /*************************************************************
     * @Description Lấy số lượng phần tử hiện có trong danh sách
     * @return      Kích thước của danh sách (size_t)
     *************************************************************/
    size_t size() const;
};

/*************************************************************
 * @Description Thêm một phần tử vào danh sách
 *************************************************************/
template <class DataType>
void RecordList<DataType>::add(const DataType& item) {
    _items.push_back(item);
}

/*************************************************************
 * @Description Lấy tham chiếu đến danh sách các phần tử
 *************************************************************/
template <class DataType>
const std::vector<DataType>& RecordList<DataType>::getItems() const {
    return _items;
}

/*************************************************************
 * @Description Lấy số lượng phần tử hiện có trong danh sách
 *************************************************************/
template <class DataType>
size_t RecordList<DataType>::size() const {
    return _items.size();
}

class Employee {
public:
    explicit Employee(const std::string& strFileName = "Employees.txt");
    bool authenticateEmployee(const std::string& strUsername, const std::string& strPassword) const;
    bool changePassword(const std::string& strUsername, const std::string& strCurrentPassword, const std::string& strNewPassword);
    bool needsFirstLoginPasswordChange(const std::string& strUsername) const;
private:
    std::string _strFileName;
    bool getPassword(const std::string& strUsername, std::string& strPassword) const;
};

#endif
