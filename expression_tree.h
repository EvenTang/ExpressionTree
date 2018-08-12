#ifndef _EXPRESSION_TREE_
#define _EXPRESSION_TREE_


#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <stdint.h>



	


#ifdef __cplusplus	
extern "C" {
#endif 
    // to be deleted
	uint16_t get_can_id_by_signal_name(char const * _signal_name);
	void get_can_data_by_id(uint16_t _can_id, uint8_t * _p_data, uint8_t * _data_len);
	uint32_t get_signal_value_by_name(char const * _signal_name);
	uint64_t get_timestamp_by_can_id(uint16_t _can_id);
	// ------------------------------

	typedef enum _ExpType
	{
		CAN_SIG,
		OPERATION_LOGIC,
		OPERATION_ARITH,
		OPERAND,
	}ExpType;

	typedef struct tree
	{
		void  * data;
		ExpType type;
		struct tree *left, *right;
	}ExpTree;

	typedef uint8_t BOOL;
#define TRUE  (1)
#define FALSE (0)

	ExpTree *build_exp_tree(char const * _infix_exp);
	BOOL calc_exp_tree(ExpTree *);



#ifdef __cplusplus	
}
#endif 

#endif // _EXPRESSION_TREE_