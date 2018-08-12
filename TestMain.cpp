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
	test_data_probe();
	//TestForExpression();


	return 0;
}

