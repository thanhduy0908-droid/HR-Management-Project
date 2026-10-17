#include "Profile.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

const string Profile::EMPLOYEES_FILE_NAME = "Employees.txt";
const string Profile::PROFILE_FILE_EXTENSION = ".txt";
const string Profile::SEPARATOR_LINE = "----------------------------------------";

Profile::Profile(): _strUsername(""), _strFullName(""), _strAddress(""), _strPhoneNumber(""), _strEmail("") {}

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
	cout << "Username      : " << this->_strUsername << endl;
	cout << "Ho ten        : " << this->_strFullName << endl;
	cout << "Dia chi       : " << this->_strAddress << endl;
	cout << "So dien thoai : " << this->_strPhoneNumber << endl;
	cout << "Email         : " << this->_strEmail << endl;
	cout << Profile::SEPARATOR_LINE << endl;
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
	}else {
		cout << "Canh bao: Khong doc duoc file thong tin cua '" << strUsername << "'!" << endl;
	}
}
