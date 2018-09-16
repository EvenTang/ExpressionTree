#include "expression_tree.h"

#include <ctype.h>


void * malloc_clean_mem(size_t _size) {
	void * p = malloc(_size);
	if (p == NULL) {
		return NULL;
	}
	else {
		memset(p, 0x00, _size);
	}
	return p;
}

void free_my_mem(void *p) {
	if (p != NULL) {
		free(p);
	}
}

static uint8_t make_operation_tree(ExpTree * _exp_stack[], uint8_t _stack_top, char const * _op, ExpType _type) {
	ExpTree * op1 = _exp_stack[_stack_top - 1];
	ExpTree * op2 = _exp_stack[_stack_top - 2];
	_stack_top -= 2;
	_exp_stack[_stack_top] = (ExpTree *)malloc_clean_mem(sizeof(ExpTree));
	_exp_stack[_stack_top]->data = malloc_clean_mem(strlen(_op) + 1);
	strcpy((char *)_exp_stack[_stack_top]->data, _op);
	_exp_stack[_stack_top]->type = _type;
	_exp_stack[_stack_top]->left = op2;
	_exp_stack[_stack_top]->right = op1;
	_stack_top++;

	return _stack_top;
}

static BOOL is_in_str_array(char const * _p_str, char const * _p_str_ary[]) {
	char const* * p = NULL;
	for (p = _p_str_ary; *p != NULL; p++) {
		if (strcmp(*p, _p_str) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL is_boolean_operation(char const * _p_op) {
	char const * opreations[] = { "<", "<=", "==","!=",">",">=", "&&", "||", NULL };

	return is_in_str_array(_p_op, opreations);
}

static BOOL is_arithmetic_operation(char const * _p_op) {
	char const * opreations[] = { "+", "-", "*", "/", "%", "&", "|", NULL};

	return is_in_str_array(_p_op, opreations);
}

static BOOL is_any_of(char c, char const * _p_str) {
	char const * p = _p_str;
	for (; *p != '\0'; ++p) {
		if (*p == c) {
			return TRUE;
		}
	}
	return FALSE;
}

static BOOL is_sub_set_character_of(char const * p_target, char const * _p_set) {
	char const * p = p_target;
	for (; *p != '\0'; ++p) {
		if (is_any_of(*p, _p_set) == FALSE) {
			return FALSE;
		}
	}
	return TRUE;
}

static BOOL is_hex_str(char const * _p_num) {
	char const * hex_digital = "0123456789abcdefABCDEF";

	if ((*_p_num == '0') && ((*(_p_num + 1) == 'X') || (*(_p_num + 1) == 'x'))) {
		return is_sub_set_character_of(_p_num + 2, hex_digital);
	}
	
	return FALSE;
}

static BOOL is_dec_str(char const * _p_num) {
	char const * dec_digital_char = "0123456789";

	return is_sub_set_character_of(_p_num, dec_digital_char);
}

static BOOL is_operand(char const * _data) {
	return is_hex_str(_data) || is_dec_str(_data);
}

static BOOL is_can_signal_name(char const * _data) {
	char const * c_symbol_char = "0123456789_"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	return is_sub_set_character_of(_data, c_symbol_char);
}


static ExpTree* post_expression_to_tree(char * _post_exp_str_tokens[]) {
	ExpTree * exp_stack[100];
	uint8_t stack_top = 0;
	char ** item = NULL;
	for (item = _post_exp_str_tokens; *item != NULL; ++item) {
		if (is_boolean_operation(*item)) {
			stack_top = make_operation_tree(exp_stack, stack_top, *item, OPERATION_LOGIC);
		}
		else if (is_arithmetic_operation(*item)) {
			stack_top = make_operation_tree(exp_stack, stack_top, *item, OPERATION_ARITH);
		}
		else if (is_operand(*item)){ // Operand
			exp_stack[stack_top] = (ExpTree *)malloc_clean_mem(sizeof(ExpTree));
			exp_stack[stack_top]->type = OPERAND;
			exp_stack[stack_top]->data = malloc_clean_mem(sizeof(uint32_t));
			if (is_hex_str(*item)) {
				*(uint32_t*)exp_stack[stack_top]->data = strtol(*item, NULL, 16);
			}
			else {
				*(uint32_t*)exp_stack[stack_top]->data = atoi(*item);
			}
			stack_top++;
		}
		else { // if (is_can_signal(item)) {
			exp_stack[stack_top] = (ExpTree *)malloc_clean_mem(sizeof(ExpTree));
			exp_stack[stack_top]->type = CAN_SIG;
			exp_stack[stack_top]->data = malloc_clean_mem(strlen(*item));
			strcpy((char *)exp_stack[stack_top]->data, *item);
			stack_top++;
		}
	}

	return exp_stack[0];
}


static uint32_t calc_value(ExpTree * _exp_tree)
{
	if (_exp_tree->type == OPERAND) {
		return *(uint32_t*)_exp_tree->data;
	}
	else if (_exp_tree->type == CAN_SIG) {
		return get_signal_value_by_name((char const *)_exp_tree->data);
	}
	else if (strcmp((char const*)_exp_tree->data, "+") == 0) {
		return calc_value(_exp_tree->left) + calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "-") == 0) {
		return calc_value(_exp_tree->left) - calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "*") == 0) {
		return calc_value(_exp_tree->left) * calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "/") == 0) {
		return calc_value(_exp_tree->left) / calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "%") == 0) {
		return calc_value(_exp_tree->left) % calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "&") == 0) {
		return calc_value(_exp_tree->left) & calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "|") == 0) {
		return calc_value(_exp_tree->left) | calc_value(_exp_tree->right);
	}
	return 0;
}

static BOOL calc_boolean(ExpTree * _exp_tree)
{
	if (strcmp((char const*)_exp_tree->data, "==") == 0) {
		return calc_value(_exp_tree->left) == calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, ">") == 0) {
		return calc_value(_exp_tree->left) > calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, ">=") == 0) {
		return calc_value(_exp_tree->left) >= calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "<") == 0) {
		return calc_value(_exp_tree->left) < calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "<=") == 0) {
		return calc_value(_exp_tree->left) <= calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "!=") == 0) {
		return calc_value(_exp_tree->left) != calc_value(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "&&") == 0) {
		return calc_boolean(_exp_tree->left) && calc_boolean(_exp_tree->right);
	}
	else if (strcmp((char const*)_exp_tree->data, "||") == 0) {
		return calc_boolean(_exp_tree->left) || calc_boolean(_exp_tree->right);
	}
	return FALSE;
}

static char const * find_first_not_of(char const * _p_str, char const * _p_set) {
	for (; *_p_str != '\0'; ++_p_str) {
		if (!is_any_of(*_p_str, _p_set)) {
			return _p_str;
		}
	}

	return _p_str;
}

static char const * find_first_of(char const * _p_str, char const * _p_set) {
	for (; *_p_str != '\0'; ++_p_str) {
		if (is_any_of(*_p_str, _p_set)) {
			return _p_str;
		}
	}

	return _p_str;
}

static void expression_to_tokens(char const * _exp_str, char * _tokens[]) {
	char const * operations[] = {
		"||", "&&", ">=", ">", "<=", "<", "==",
		"!=", "(",  ")", "+",  "-", "*",  "/",
		"%",  "&",  "|", NULL
	};

	char const * separator = " \t\r\n";

	uint16_t tokens_count = 0;
	char const * p_exp_str = _exp_str;
	for (; *p_exp_str != '\0'; ) {
		if (is_any_of(*p_exp_str, separator)) {
			++p_exp_str;
			continue;
		}
		else {
			char const * * token = operations;
			for (; *token != NULL; token++) {
				if (strstr(p_exp_str, *token) == p_exp_str) {
					size_t token_size = strlen(*token);
					_tokens[tokens_count] = (char *)malloc_clean_mem(token_size + 1);
					strcpy(_tokens[tokens_count], *token);
					tokens_count++;
					p_exp_str += token_size;
					break;
				}
			}
			if (*token == NULL) {
				char const * next_token_char = " \t=()<>!+-*&|/!%";
				char const * p_c_v_end = find_first_of(p_exp_str, next_token_char);
				uint8_t token_size = p_c_v_end - p_exp_str;
				_tokens[tokens_count] = (char *)malloc_clean_mem(token_size + 1);
				memset((void*)_tokens[tokens_count], 0, token_size + 1);
				memcpy((void*)_tokens[tokens_count], p_exp_str, token_size);
				tokens_count++;
				p_exp_str = p_c_v_end;
			}
		}
	}
}

static BOOL is_parenth_operation(char const * _p_op) {
	char const * opreations[] = { "(", ")", NULL };

	return is_in_str_array(_p_op, opreations);
}

static BOOL is_operation(char const * _p_token) {
	return is_arithmetic_operation(_p_token) || is_boolean_operation(_p_token) || is_parenth_operation(_p_token);
}

static uint8_t priority(char const * _str)
{
	struct op_pri_pair {
		char const * p_op;
		uint8_t pri;
	} *p_table, token_patters[] = {
		{ ")",	0 },
		{ "+",	1 },
		{ "-",	1 },
		{ "*",	2 },
		{ "/",	2 },
		{ "%",	2 },
		{ "&",  3 },
		{ "|",	3 },
		{ ">",	4 },
		{ ">=",	4 },
		{ "<",	4 },
		{ "<=",	4 },
		{ "==",	5 },
		{ "!=",	5 },
		{ "||",	6 },
		{ "&&",	6 },
		{ "(",	7 },
	    { NULL, 0 }
	};
	p_table = token_patters;


	for (; p_table->p_op != NULL; ++p_table) {
		if (strcmp(p_table->p_op, _str) == 0) {
			return p_table->pri;
		}
	}
	return 0;
}

void release_all_str_memory(char * p_str_pool[], uint8_t _pool_size) {
	uint8_t i = 0;
	for (; i < _pool_size; ++i) {
		if (p_str_pool[i] != NULL) {
			free_my_mem(p_str_pool[i]);
		}
	}
}

static void infix_to_postfix_token_list(char const * _exp_str, char * _postfix_tokens[])
{
#define TOKEN_LIST_SIZE (100)
	char * token_list[TOKEN_LIST_SIZE] = { NULL };
	
	expression_to_tokens(_exp_str, token_list);

	char * operatorstack[100] = { NULL };
	uint8_t stack_top = 0;
	
	uint8_t postfix_tokens_count = 0;

	char ** p_token = token_list;
	for (; *p_token != NULL; ++p_token) {
		if (is_operation(*p_token)) {
			if (stack_top == 0) { //operatorstack.empty()
				operatorstack[stack_top++] = *p_token; // operatorstack.push(token);
			}
			else {
				if (strcmp(*p_token, ")") == 0) {
					while (strcmp(operatorstack[stack_top-1], "(") != 0) {
						_postfix_tokens[postfix_tokens_count] = (char *)malloc_clean_mem(strlen(operatorstack[stack_top-1]) + 1);
						strcpy(_postfix_tokens[postfix_tokens_count], operatorstack[stack_top-1]);
						postfix_tokens_count++;
						stack_top--;
					}
					stack_top--;
				}
				else if (strcmp(*p_token, "(") == 0) {
					operatorstack[stack_top++] = *p_token;
				}
				else
				{
					auto curpri = priority(*p_token);
					while (stack_top != 0) {
						char const * top = operatorstack[stack_top-1];
						int toppor = priority(top);

						// small value means high priority
						if ((curpri >= toppor) && (strcmp(top, "(") != 0)) {
							_postfix_tokens[postfix_tokens_count] = (char *)malloc_clean_mem(strlen(top) + 1);
							strcpy(_postfix_tokens[postfix_tokens_count], top);
							postfix_tokens_count++;
							stack_top--;
						}
						else {
							break;
						}
					}
					operatorstack[stack_top++] = *p_token;
				}
			}
		}
		else {
			_postfix_tokens[postfix_tokens_count] = (char *)malloc_clean_mem(strlen(*p_token) + 1);
			strcpy(_postfix_tokens[postfix_tokens_count], *p_token);
			postfix_tokens_count++;
		}
	}

	while (stack_top != 0) {
		_postfix_tokens[postfix_tokens_count] = (char *)malloc_clean_mem(strlen(operatorstack[stack_top-1]) + 1);
		strcpy(_postfix_tokens[postfix_tokens_count], operatorstack[stack_top-1]);
		postfix_tokens_count++;
		stack_top--;
	}

	release_all_str_memory(token_list, TOKEN_LIST_SIZE);
}

BOOL calc_exp_tree(ExpTree * _exp_tree)
{
	return calc_boolean(_exp_tree);
}

ExpTree *build_exp_tree(char const * _infix_exp) {
#define POSTFIX_TOKEN_LIST_SIZE (100)
	char * post_fix_token_list[POSTFIX_TOKEN_LIST_SIZE] = {NULL};
	ExpTree * exp_tree = NULL;

	infix_to_postfix_token_list(_infix_exp, post_fix_token_list);
	exp_tree = post_expression_to_tree(post_fix_token_list);
	release_all_str_memory(post_fix_token_list, POSTFIX_TOKEN_LIST_SIZE);

	return exp_tree;
}
