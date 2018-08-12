#include "BoolExpression.h"

#include <vector>
#include <tuple>
#include <stack>

class BinaryOpAnd :public Expression
{
public:
	BinaryOpAnd(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: m_left(_left), m_right(_right)
	{}


	bool evaluate_boolean() const
	{
		return m_left->evaluate_boolean() && m_right->evaluate_boolean();
	}
private:
	std::shared_ptr<Expression> m_left;
	std::shared_ptr<Expression> m_right;
};

class BinaryOpOr :public Expression
{
public:
	BinaryOpOr(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: m_left(_left), m_right(_right)
	{}

	bool evaluate_boolean() const
	{
		return m_left->evaluate_boolean() || m_right->evaluate_boolean();
	}
private:
	std::shared_ptr<Expression> m_left;
	std::shared_ptr<Expression> m_right;
};

class UnifyOpNot :public Expression
{
public:
	bool evaluate_boolean() const
	{
		return !m_bool_value->evaluate_boolean();
	}
private:
	std::shared_ptr<Expression> m_bool_value;
};

template <typename T>
class ValueExp : public Expression
{
public:
	ValueExp(T _value) {
		this->m_value = _value;
	}

	T evaluate_value() const
	{
		return m_value;
	}
	
protected:
	T m_value;
};

template <typename T>
class CANSignalExp : ValueExp<T>
{
public:
	CANSignalExp(string const & _name) : ValueExp<T>(T()) {
		m_signal_name = _name;
	}

	T evaluate_value() const {
		return m_value;
	}

private:
	string m_signal_name;
};

class BinaryOpArithmetic :public ValueExp<uint32_t>
{
public:
	BinaryOpArithmetic(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: ValueExp<uint32_t>(0), m_left_value(_left), m_right_value(_right)
	{}

protected:
	template <typename T>
	inline T get_left() const {
		return reinterpret_cast<ValueExp<T>*>(m_left_value.get())->evaluate_value();
	}

	template <typename T>
	inline T get_right() const {
		return reinterpret_cast<ValueExp<T>*>(m_right_value.get())->evaluate_value();
	}

protected:
	std::shared_ptr<Expression> m_left_value;
	std::shared_ptr<Expression> m_right_value;
};

class BinaryOpPlus : public BinaryOpArithmetic
{
public:
	BinaryOpPlus(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: BinaryOpArithmetic(_left, _right)
	{}

	uint32_t evaluate_value() const {
		return get_left<uint32_t>() + get_right<uint32_t>();
	}
};

class BinaryOpMinus : public BinaryOpArithmetic
{
public:
	BinaryOpMinus(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: BinaryOpArithmetic(_left, _right)
	{}

	uint32_t evaluate_value() const {
		return get_left<uint32_t>() - get_right<uint32_t>();
	}
};

class BinaryOpMuliply : public BinaryOpArithmetic
{
public:
	BinaryOpMuliply(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: BinaryOpArithmetic(_left, _right)
	{}

	uint32_t evaluate_value() const {
		return get_left<uint32_t>() * get_right<uint32_t>();
	}
};

class BinaryOpDivid : public BinaryOpArithmetic
{
public:
	BinaryOpDivid(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: BinaryOpArithmetic(_left, _right)
	{}

	uint32_t evaluate_value() const {
		return get_left<uint32_t>() / get_right<uint32_t>();
	}
};

class BinaryOpModulus : public BinaryOpArithmetic
{
public:
	BinaryOpModulus(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: BinaryOpArithmetic(_left, _right)
	{}

	uint32_t evaluate_value() const {
		return get_left<uint32_t>() % get_right<uint32_t>();
	}
};

class BinaryOpCompare :public Expression
{
public:
	BinaryOpCompare(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: m_left_value(_left), m_right_value(_right)
	{}

protected:
	template <typename T>
	inline T get_left() const {
		return reinterpret_cast<ValueExp<T>*>(m_left_value.get())->evaluate_value();
	}

	template <typename T>
	inline T get_right() const {
		return reinterpret_cast<ValueExp<T>*>(m_right_value.get())->evaluate_value();
	}

protected:
	std::shared_ptr<Expression> m_left_value;
	std::shared_ptr<Expression> m_right_value;
};


class BinaryOpGreatThan : public BinaryOpCompare
{
public:
	BinaryOpGreatThan(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: BinaryOpCompare(_left, _right)
	{}

	bool evaluate_boolean() const {
		return get_left<uint32_t>() > get_right<uint32_t>();
	}
};


class BinaryOpLessThan : public BinaryOpCompare
{
public:
	BinaryOpLessThan(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: BinaryOpCompare(_left, _right)
	{}

	bool evaluate_boolean() const {
		return get_left<uint32_t>() < get_right<uint32_t>();
	}
};


class BinaryOpEqual : public BinaryOpCompare
{
public:
	BinaryOpEqual(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: BinaryOpCompare(_left, _right)
	{}

	bool evaluate_boolean() const {
		return get_left<uint32_t>() == get_right<uint32_t>();
	}

};


class BinaryOpNotEqual : public BinaryOpCompare
{
public:
	BinaryOpNotEqual(std::shared_ptr<Expression> _left, std::shared_ptr<Expression> _right)
		: BinaryOpCompare(_left, _right)
	{}

	bool evaluate_boolean() const {
		return get_left<uint32_t>() != get_right<uint32_t>();
	}

};

static uint8_t priority(const string & _str)
{
	vector<tuple<string, uint8_t>> const token_patters{
		{ "||",		     4 },
		{ "&&",		     4 },
		{ ">",		     1 },
		{ "<",		     1 },
		{ "==",		     1 },
		{ "!=",		     1 },
		{ "(",		     5 },
		{ ")",		     0 },
		{"+",            2 },
		{"-",			 2 },
		{"*",			 3 },
		{"/",			 3 },
		{"%",			 3 },
	};

	for (auto item : token_patters) {
		if (std::get<0>(item) == _str) {
			return std::get<1>(item);
		}
	}
	return 0;
}

bool is_operation(string const & _str) {
	vector<string> const token_patters{
		"||",		   
		"&&",		   
		">",		   
		"<",		   
		"==",
		"!=",
		"(",		   
		")",
		"+",
		"-",
		"*",
		"/",
		"%"
	};

	return std::find(token_patters.begin(), token_patters.end(), _str) != token_patters.end();
}


static vector<string> expression_to_tokens(string const _exp_str) {
	vector<string> const token_patters {
		"||",
		"&&",
		">",
		"<",
		"==",
		"!=",
		"(",
		")",
		"+",
		"-",
		"*",
		"/",
		"%",
		"0123456789.",
		"abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	};

	vector<string> tokens;

	for (unsigned int i = 0; i < _exp_str.size(); ++i) {
		for (auto const & token : token_patters) {
			if (_exp_str.find(token, i) == i) {
				tokens.push_back(token);
				i += token.size() - 1; 
				break;
			}
			else if (_exp_str.find_first_of(token, i) == i) {
				auto j = _exp_str.find_first_not_of(token, i);
				tokens.push_back(_exp_str.substr(i, j - i));
				i = j - 1;
			}
		}
	}

	return tokens;
}

vector<string> infix_to_postfix_token_list(const string &str)
{
	auto Inputvec = expression_to_tokens(str); 	

	stack<string> operatorstack;     
	vector<string> PostfiOutvec; 

	for (auto const & token : Inputvec) {
		if (is_operation(token)) {
			if (operatorstack.empty()) {
				operatorstack.push(token);     
			}
			else {
				if (token == ")") {
					while (operatorstack.top() != "(") {
						PostfiOutvec.push_back(operatorstack.top());     
						operatorstack.pop();    
					}
					operatorstack.pop();    
				}
				else
				{
					auto curpri = priority(token);  	
					while (!operatorstack.empty()) {
						string top = operatorstack.top();     
						int toppor = priority(top);    

						if ((curpri <= toppor) && top != "(") {
							PostfiOutvec.push_back(top);
							operatorstack.pop();   
						}
						else {
							break;
						}
					}
					operatorstack.push(token);
				}
			}
		}
		else {
			PostfiOutvec.push_back(token);
		}
	}

	while (!operatorstack.empty()) {
		PostfiOutvec.push_back(operatorstack.top());
		operatorstack.pop();
	}

	return PostfiOutvec;
}

bool is_constant(string const & _str) {
	return true;
}

template <typename T>
void make_operation_tree(stack<std::shared_ptr<Expression>> & _exp_stack) {
	auto op1 = _exp_stack.top();
	_exp_stack.pop();
	auto op2 = _exp_stack.top();
	_exp_stack.pop();
	_exp_stack.push(std::make_shared<T>(op2, op1));
}

std::shared_ptr<Expression> post_expression_to_tree(vector<string> const & _post_exp_str) {
	stack<std::shared_ptr<Expression>> exp_stack;

	for (auto const & item : _post_exp_str) {
		if (item == ">") {
			make_operation_tree<BinaryOpGreatThan>(exp_stack);
		}
		else if (item == "<") {
			make_operation_tree<BinaryOpLessThan>(exp_stack);
		}
		else if (item == "==") {
			make_operation_tree<BinaryOpEqual>(exp_stack);
		}
		else if (item == "&&") {
			make_operation_tree<BinaryOpAnd>(exp_stack);
		}
		else if (item == "||") {
			make_operation_tree<BinaryOpOr>(exp_stack);
		}
		else if (item == "!=") {
			make_operation_tree<BinaryOpNotEqual>(exp_stack);
		}
		else if (item == "+") {
			make_operation_tree<BinaryOpPlus>(exp_stack);
		}
		else if (item == "-") {
			make_operation_tree<BinaryOpMinus>(exp_stack);
		}
		else if (item == "*") {
			make_operation_tree<BinaryOpMuliply>(exp_stack);
		}
		else if (item == "/") {
			make_operation_tree<BinaryOpDivid>(exp_stack);
		}
		else if (item == "%") {
			make_operation_tree<BinaryOpModulus>(exp_stack);
		}
		else { // Operand
			auto constant = std::make_shared<ValueExp<uint32_t>>(atoi(item.c_str()));
			exp_stack.push(constant);
		}
	}

	auto final_tree = exp_stack.top();
	exp_stack.pop();

	return final_tree;
}


BoolExpression::BoolExpression(std::string const & _cond) {
	auto post_expression = infix_to_postfix_token_list(_cond);
	m_exp = post_expression_to_tree(post_expression);
}

BoolExpression::~BoolExpression()
{
}
