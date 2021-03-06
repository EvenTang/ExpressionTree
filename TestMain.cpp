// CTraining.cpp: 定义控制台应用程序的入口点。
//
#include <cstdint>
#include <sstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <iostream>
#include <stdio.h>
#include <memory>
#include <fstream>
#include <iterator>
#include <vector>
#include <xmemory>
#include <deque>
#include <streambuf>
#include <map>


#include "expression_tree.h"

using namespace std;

using namespace std;

map<string, uint32_t> can_signal;

extern "C" void set_singal_value_by_name(char const * _signal_name, uint32_t _value) {
	can_signal[_signal_name] = _value;
}

extern "C" uint16_t get_can_id_by_signal_name(char const * _signal_name)
{	
	static uint16_t can_id = 0x200;
	return can_id++;
}

extern "C" void get_can_data_by_id(uint16_t _can_id, uint8_t * _p_data, uint8_t * _data_len) {
	uint8_t i = 0;
	for(i = 0; i < 8; i++) {
		_p_data[i] = i;
	}

	*_data_len = 8;
}

extern "C" uint32_t get_signal_value_by_name(char const * _signal_name) {
	
	return can_signal[_signal_name];
}


void TestExpressionTree(char const * _p_exp, BOOL _expected) {

	auto et = build_exp_tree(_p_exp);
	if (calc_exp_tree(et) != _expected) {
		std::cout << "Wrong for :" << _p_exp << " [calculated as : " << (_expected == TRUE ? "FALSE ]" : "TRUE ]") << std::endl;
		std::cout << "      Should be :" << (_expected == TRUE ? "TRUE" : "FALSE") << std::endl;
	}
	else {
		std::cout << "Past test for :" << _p_exp << " And calculated as : " << (_expected == TRUE ? "TRUE" : "FALSE") << std::endl;
	}

}

void TestForExpression() {
	TestExpressionTree("1 == 1 || 2 == 2", TRUE);
	TestExpressionTree("(1 == 1) || (2 == 2)", TRUE);
	TestExpressionTree("1 <= 2", TRUE);
	TestExpressionTree("1 != 2", TRUE);
	TestExpressionTree("1 == 2", FALSE);
	TestExpressionTree("(1 == 2)", FALSE);
	TestExpressionTree("(1 != 2)", TRUE);
	TestExpressionTree("(12 > 3) || (1 != 2)", TRUE);
	TestExpressionTree("(12 < 3) || (1 != 2)", TRUE);
	TestExpressionTree("(12 < 3) && (1 != 2)", FALSE);
	TestExpressionTree("((12 < 3)||(12 > 3)) && (1 != 2)", TRUE);
	TestExpressionTree("1 + 2 > 4", FALSE);
	TestExpressionTree("1 + 5 > 4", TRUE);
	TestExpressionTree("1 + 5 == 6", TRUE);

	set_singal_value_by_name("ACC", 1);
	set_singal_value_by_name("ACC_Old", 2);
	TestExpressionTree("ACC == 6", FALSE);
	TestExpressionTree("ACC == 1", TRUE);
	TestExpressionTree("ACC == ACC_Old", FALSE);
	set_singal_value_by_name("BCC_2", 0x3);
	set_singal_value_by_name("ACC_Old", 4);

	TestExpressionTree("BCC_2 == ACC_Old", FALSE);
	TestExpressionTree("BCC_2 != ACC", TRUE);

	TestExpressionTree("(0x11 & 0x0f) > 0", TRUE);
	TestExpressionTree("(0x11 & 0x0f) == 0x01", TRUE);
	TestExpressionTree("(0x11 & 0x0f) == 0x11", FALSE);
	TestExpressionTree("(0x01 | 0x10) == 0x11", TRUE);
	TestExpressionTree("(BCC_2 | 0x01) == 0x01", FALSE);
}

int main()
{
	TestForExpression();

	return 0;
}

