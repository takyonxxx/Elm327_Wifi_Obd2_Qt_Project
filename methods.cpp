#include "methods.h"

vector<uint32_t> all_pids_int{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
								0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
								0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
								0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
								0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31,
								0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
								0x3c, 0x3d, 0x3e, 0x3f, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45,
								0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
								0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
								0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63,
								0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,
								0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
								0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x80, 0x81,
								0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0xA0, 0xC0 };

vector<uint32_t> available_pids(uint32_t number, uint32_t start) {
	vector<uint32_t> pids;
	for (uint32_t p = 1; p <= 32; p++) {
		if ((number >> (32 - p) & 1) == 1) {
			pids.push_back(p + start);
		}
	}
	return pids;
}

vector<uint32_t> get_available(vector<uint32_t> binarized) {
	vector<vector<uint32_t> > arr;
	vector<uint32_t> appended_arr;

	for (uint32_t i = 0; i < binarized.size(); i++) {
		arr.push_back(available_pids(binarized[i], 0x20 * i));
	}

	if (arr.size() > 0) {
		for (uint32_t i = 0; i < arr.size(); i++) {
			for (uint32_t j = 0; j < arr[i].size(); j++) {
				appended_arr.push_back(arr[i][j]);
			}
		}
		return appended_arr;
	}
	else {
		return vector<uint32_t>();
	}
}

string dec2hex(uint32_t dec) {
	vector<char> hexaDeciNum; 
	// counter for hexadecimal number array 
	int i = 0;
	while (dec != 0) {
		// temporary variable to store remainder 
		int temp = 0;
		// storing remainder in temp variable. 
		temp = dec % 16;
		// check if temp < 10 
		if (temp < 10) {
			hexaDeciNum.push_back(temp + 48);
			i++;
		}
		else {
			hexaDeciNum.push_back(temp + 55);
			i++;
		}
		dec = dec / 16;
	}
	string hex("0x");

	if (hexaDeciNum.size() == 1) {
		hex.push_back('0');
	}

	for (int j = hexaDeciNum.size() - 1; j >= 0; j--) {
		hex.push_back(hexaDeciNum[j]);
	}

	return hex;
}

vector<uint32_t> fill_pids1(vector<uint32_t> arr){
	vector<string> supported_pids;
	vector<string> all_pids_str;
	vector<uint32_t> pids;

	for (uint32_t i = 0; i <= 135; i++) { // 0x0 <= i <= 0x87
		if (i < arr.size()) {
			supported_pids.push_back(dec2hex(arr[i]));
		}
		all_pids_str.push_back(dec2hex(i));
	} 
	all_pids_str.push_back("0xA0"); // 160
	all_pids_str.push_back("0xC0"); // 192


	for (uint32_t i = 0; i < supported_pids.size(); i++) {
		for (uint32_t j = 0; j < all_pids_str.size(); j++) {
			if (supported_pids[i] == all_pids_str[j]) {
				pids.push_back(all_pids_int[j]);
			}
		}
	}

	return pids;
}

vector<uint32_t> decode_car_cmds(vector<uint32_t> cmds){
	return fill_pids1(get_available(cmds));
}



