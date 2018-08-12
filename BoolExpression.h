#pragma once

#include <string>
#include <memory>

using namespace std;

class Expression {
public:
	enum class ExpType
	{
		Operand_int,
		Operand_float,
		Operation_logic,
		Operation_compare,
		Operation_arithmetic,
	};

	ExpType get_expression_type() {
		return m_expression_type;
	}
public:
	virtual uint32_t evaluate_value() const { return 0; }
	virtual bool evaluate_boolean() const { return false; }

protected:
	ExpType m_expression_type;
};


class BoolExpression
{
public:
	BoolExpression(std::string const & _cond);
	~BoolExpression();

public:
	bool evaluate() const {
		return m_exp->evaluate_boolean();
	}

private:
	std::shared_ptr<Expression> m_exp;
};

