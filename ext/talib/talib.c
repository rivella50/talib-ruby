#include "ruby.h"
#include <ta-lib/ta_abstract.h>

static VALUE rb_mTaLib;
static VALUE rb_cTAFunction;
static VALUE rb_sInParamInfo;
static VALUE rb_sOptInParamInfo;
static VALUE rb_sOutParamInfo;

#ifndef RUBY_19
#ifndef RARRAY_LEN
#define RARRAY_LEN(v) (RARRAY(v)->len)
#endif
#ifndef RARRAY_PTR
#define RARRAY_PTR(v) (RARRAY(v)->ptr)
#endif
#ifndef RSTRING_PTR
#define RSTRING_PTR(v) (RSTRING(v)->ptr)
#endif
#endif


#define TA_INPUT_PARAM	 				1
#define TA_OPTION_INPUT_PARAM		2
#define TA_OUTPUT_PARAM 				3
#define IN_CNT  7  // allow up to 7 arrays of input
#define OUT_CNT 3  // allow up to 3 arrays of outputs
// combine all heap storage to this struct and free only this on ta_free
typedef struct _ph {
	TA_ParamHolder *p;	
  double* in[IN_CNT];  // johnribera@Hotmail: the usual case (double)
  double* out[OUT_CNT];
} ParamHolder;

/* :nodoc: */
// get TA_FuncInfo by name
static const TA_FuncInfo* abstract_layer_get_func_info(VALUE name)
{
	TA_RetCode ret_code;
  const TA_FuncHandle *handle;
	const TA_FuncInfo *the_info;

  ret_code = TA_GetFuncHandle( RSTRING_PTR(name), &handle );
	if ( ret_code == TA_SUCCESS )
	{
		ret_code = TA_GetFuncInfo( handle, &the_info );
		if ( ret_code == TA_SUCCESS )	
			return the_info;
		rb_raise(rb_eRuntimeError, "unsuccess return code TA_GetFuncInfo");
	}
	rb_raise(rb_eRuntimeError, "unsuccess return code TA_GetFuncHandle");
}

// get function parameter info
static VALUE ta_func_param_info(int param_type, VALUE self, VALUE name, VALUE index)
{
	TA_RetCode ret_code;
  const TA_FuncHandle *handle;

  ret_code = TA_GetFuncHandle( StringValuePtr(name), &handle );
	if ( ret_code == TA_SUCCESS )
	{
		switch (param_type)
		{
			case TA_INPUT_PARAM:
				{
  				const TA_InputParameterInfo *param_info;
					
					ret_code = TA_GetInputParameterInfo( handle, FIX2INT(index), &param_info );
					if (ret_code != TA_SUCCESS)
						rb_raise(rb_eRuntimeError, "unsuccess return code TA_GetInputParameterInfo");
					return rb_struct_new(rb_sInParamInfo, INT2FIX(param_info->type), rb_str_new2(param_info->paramName), INT2FIX(param_info->flags),NULL);
				}
				break;
			case TA_OPTION_INPUT_PARAM:
				{	
  				const TA_OptInputParameterInfo *param_info;

					ret_code = TA_GetOptInputParameterInfo( handle, FIX2INT(index), &param_info );
					if (ret_code != TA_SUCCESS)
						rb_raise(rb_eRuntimeError, "unsuccess return code TA_GetOptInputParameterInfo");
					// FIXME: helpFile = Qnil
					// FIXME: dataSet = Qnil
					return rb_struct_new(rb_sOptInParamInfo, INT2FIX(param_info->type), rb_str_new2(param_info->paramName), INT2FIX(param_info->flags), rb_str_new2(param_info->displayName), Qnil, rb_float_new(param_info->defaultValue), rb_str_new2(param_info->hint), Qnil, NULL);
				}
				break;
			case TA_OUTPUT_PARAM:
				{
  				const TA_OutputParameterInfo *param_info;

					ret_code = TA_GetOutputParameterInfo( handle, FIX2INT(index), &param_info );
					if (ret_code != TA_SUCCESS)
						rb_raise(rb_eRuntimeError, "unsuccess return code TA_GetOutputParameterInfo");
					return rb_struct_new(rb_sOutParamInfo, INT2FIX(param_info->type), rb_str_new2(param_info->paramName), INT2FIX(param_info->flags),NULL);
				}
				break;
		} // switch
	}
	rb_raise(rb_eRuntimeError, "unsuccess return code TA_GetFuncHandle");
}

static double* FLT2DBL(double **dest, VALUE in_array)
{
	VALUE *inp;
	int i;

  if(NIL_P(in_array)) return 0;

	inp = RARRAY_PTR(in_array);
    if (*dest) free(*dest);  // only on 1st use do we skip this step
	*dest = calloc(1, sizeof(double) * RARRAY_LEN(in_array));
	for (i = 0; i < RARRAY_LEN(in_array); i++)
		(*dest)[i] = NUM2DBL(inp[i]);
	return *dest;
}

void init_tables()
{
    // temporary variable return code
    TA_RetCode ret_code;
    // functions group table
    TA_StringTable *group_table;

    // define class variable @@groups - array of function groups
    // define class variable @@functions - hash of array of functions
    ret_code = TA_GroupTableAlloc( &group_table );
    if( ret_code == TA_SUCCESS )
    {
        // functions table
        TA_StringTable *function_table;

        // ruby array for functions group table
        VALUE rb_group_table = rb_ary_new();

        // ruby hash for functions
        VALUE rb_function_table = rb_hash_new();
        unsigned i, j;

        for( i=0; i < group_table->size; i++ )
        {
            // temporary ruby array for group's function table
            VALUE rb_temp_func_table = rb_ary_new();

            rb_ary_push(rb_group_table, rb_str_new2(group_table->string[i]));

            // function names table in hash.
            ret_code = TA_FuncTableAlloc( group_table->string[i],
                                          &function_table );
            if( ret_code == TA_SUCCESS )
            {
                for ( j=0; j < function_table->size; j++)
                rb_ary_push( rb_temp_func_table,
                             rb_str_new2(function_table->string[j]) );
                rb_hash_aset( rb_function_table,
                              rb_str_new2(group_table->string[i]),
                              rb_temp_func_table );
                TA_FuncTableFree ( function_table );
            }
            else {
                rb_raise( rb_eRuntimeError,
                          "unsuccess return code TA_FuncTableAlloc" );
            }
        }
        rb_define_class_variable( rb_cTAFunction,
                                  "@@groups", rb_group_table );
        rb_define_class_variable( rb_cTAFunction,
                                  "@@functions", rb_function_table );
        TA_GroupTableFree( group_table );
    }
    else {
        rb_raise( rb_eRuntimeError,
                  "unsuccess return code TA_GetGroupTable" );
    }
}

// free function
static void ta_func_free(void *phv)
{
    int i = 0; // this now properly cleans up memory from very last call
    ParamHolder* ph = (ParamHolder*)phv;
    for (i = 0; i < IN_CNT; i++) if (ph->in[i]) free(ph->in[i]);
    for (i = 0; i < OUT_CNT; i++) if (ph->out[i]) free(ph->out[i]);
	TA_ParamHolderFree( ph->p );
	free(ph);
}

// allocation function
static VALUE ta_func_alloc(VALUE klass) 
{
	ParamHolder *param_holder;
    // call calloc() to ensure we set all values (ptrs to NULL/0)
    // so if (free(in/out)) returns false only the first time!
	param_holder = (ParamHolder*)calloc(1, sizeof(struct _ph));
	return Data_Wrap_Struct(klass, 0, ta_func_free, param_holder);
}

/*
 * call-seq: initialize(name)
 *
 * 	Create new instance of technical analysis function with given _name_.
 *
 */
static VALUE ta_func_initialize(VALUE self, VALUE name) 
{
	TA_RetCode ret_code;
  const TA_FuncHandle *handle;
	TA_ParamHolder *ta_param_holder;
	ParamHolder *param_holder;

	Data_Get_Struct(self, ParamHolder, param_holder);		

  ret_code = TA_GetFuncHandle( RSTRING_PTR(name), &handle );
	if ( ret_code == TA_SUCCESS )
	{
		ret_code = TA_ParamHolderAlloc( handle, &ta_param_holder );
		if ( ret_code != TA_SUCCESS )
			rb_raise(rb_eRuntimeError, "unsuccess return code TA_ParamHolderAlloc");
		param_holder->p = ta_param_holder;
		rb_iv_set(self, "@name", name);
		rb_iv_set(self, "@result", rb_ary_new());
		return self;
	}
	rb_raise(rb_eRuntimeError, "unsuccess return code TA_GetFuncHandle");
}


//
// attr_reader :name
//
static VALUE ta_func_get_name(VALUE self) {
    return rb_iv_get(self, "@name");
}


/*
 * Return input parameters count.
 */
static VALUE ta_func_get_input_count(VALUE self)
{
	return INT2NUM((abstract_layer_get_func_info(rb_iv_get(self, "@name")))->nbInput);
}

/*
 * Return option input parameters count.
 */
static VALUE ta_func_get_option_input_count(VALUE self)
{
	return INT2NUM((abstract_layer_get_func_info(rb_iv_get(self, "@name")))->nbOptInput);
}

/*
 * Return output parameters count.
 */
static VALUE ta_func_get_output_count(VALUE self)
{
	return INT2NUM((abstract_layer_get_func_info(rb_iv_get(self, "@name")))->nbOutput);
}

/*
 * Return the list of names for technical function groups.
 */
static VALUE ta_func_get_groups(VALUE self)
{
	return rb_cv_get(self, "@@groups");
}

/*
 * Return the hash of names for technical functions. 
 * The key is a name of funcion group, and value - is a list of function names.
 */
static VALUE ta_func_get_functions(VALUE self)
{
	return rb_cv_get(self, "@@functions");
}

/*
 * call-seq: in(index)
 *
 * Return input parameter info for the given input parameter index.
 */
static VALUE ta_func_input_param_info(VALUE self, VALUE index)
{
	return ta_func_param_info(TA_INPUT_PARAM, self, rb_iv_get(self, "@name"), index);
}

/*
 * call-seq: opt(index)
 *
 * Return option input parameter info for the given option parameter index.
 */
static VALUE ta_func_option_param_info(VALUE self, VALUE index)
{
	return ta_func_param_info(TA_OPTION_INPUT_PARAM, self, rb_iv_get(self, "@name"), index);
}

/*
 * call-seq: out(index)
 *
 * Return output parameter info for the given output parameter index.
 */
static VALUE ta_func_output_param_info(VALUE self, VALUE index)
{
	return ta_func_param_info(TA_OUTPUT_PARAM, self, rb_iv_get(self, "@name"), index);
}

/*
 * call-seq: in_int(index, array)
 *
 * Set input parameter (array of integer) for the given parameter index. 
 */
static VALUE ta_func_setup_in_integer(VALUE self, VALUE param_index, VALUE in_array) 
{
	TA_RetCode ret_code;
	ParamHolder *param_holder;

	Data_Get_Struct(self, ParamHolder, param_holder);		
	ret_code = TA_SetInputParamIntegerPtr( param_holder->p, FIX2INT(param_index), (int*)(RARRAY_PTR(in_array)));
	if ( ret_code != TA_SUCCESS )
		rb_raise(rb_eRuntimeError, "unsuccess return code TA_SetInputParamIntegerPtr");

	return T_TRUE;
}

/*
 * call-seq: in_real(index, array)
 *
 * Set input parameter (array of real) for the given parameter index. 
 */
static VALUE ta_func_setup_in_real(VALUE self, VALUE param_index, VALUE in_array) 
{
	double** dp; 
	TA_RetCode ret_code;
	ParamHolder *param_holder;

	Data_Get_Struct(self, ParamHolder, param_holder);		
        dp = param_holder->in;
	//FIXME: memory leak fixed: johnribera@hotmail.com (see: FL2DBL())
	ret_code = TA_SetInputParamRealPtr( param_holder->p, FIX2INT(param_index), FLT2DBL(&dp[FIX2INT(param_index)], in_array));
	if ( ret_code != TA_SUCCESS )
		rb_raise(rb_eRuntimeError, "unsuccess return code TA_SetInputParamRealPtr");
	return T_TRUE;
}

static VALUE ta_func_setup_in_price(VALUE self, VALUE param_index, VALUE in_open, VALUE in_high, VALUE in_low, VALUE in_close, VALUE in_volume, VALUE in_oi)
{
  double **dp;
	TA_RetCode ret_code;
	ParamHolder *param_holder;
  Data_Get_Struct(self, ParamHolder, param_holder);
  dp = param_holder->in;
  
  ret_code = TA_SetInputParamPricePtr( 
      param_holder->p, 
      FIX2INT(param_index), 
      FLT2DBL(&dp[0], in_open), 
      FLT2DBL(&dp[1], in_high), 
      FLT2DBL(&dp[2], in_low), 
      FLT2DBL(&dp[3], in_close), 
      FLT2DBL(&dp[4], in_volume), 
      FLT2DBL(&dp[5], in_oi)
  );
	if ( ret_code != TA_SUCCESS )
		rb_raise(rb_eRuntimeError, "unsuccess return code TA_SetInputParamPricePtr");

	return T_TRUE;
}

/*
 * call-seq: opt_int(index, value)
 *
 * Set option parameter (integer) for the given parameter index. 
 */
static VALUE ta_func_setup_opt_in_integer(VALUE self, VALUE param_index, VALUE val) 
{
	TA_RetCode ret_code;
	ParamHolder *param_holder;
	Data_Get_Struct(self, ParamHolder, param_holder);		
	ret_code = TA_SetOptInputParamInteger( param_holder->p, FIX2INT(param_index), FIX2INT(val));
	if ( ret_code != TA_SUCCESS )
		rb_raise(rb_eRuntimeError, "unsuccess return code TA_SetOptInputParamIntegerPtr");

	return val;
}

/*
 * call-seq: opt_real(index, value)
 *
 * Set option parameter (real) for the given parameter index. 
 */
static VALUE ta_func_setup_opt_in_real(VALUE self, VALUE param_index, VALUE val) 
{
	TA_RetCode ret_code;
	ParamHolder *param_holder;

	Data_Get_Struct(self, ParamHolder, param_holder);		
	ret_code = TA_SetOptInputParamReal( param_holder->p, FIX2INT(param_index), NUM2DBL(val));
	if ( ret_code != TA_SUCCESS )
		rb_raise(rb_eRuntimeError, "unsuccess return code TA_SetOptInputParamRealPtr");

	return val;
}

/*
 * call-seq: out_real(index, array)
 *
 * Set output parameter (array of real) for the given parameter index. 
 */
static VALUE ta_func_setup_out_real(VALUE self, VALUE param_index, VALUE out_array) 
{
	TA_RetCode ret_code;
	ParamHolder *param_holder;
        long idx = FIX2INT(param_index);
	double **dp;
	if (idx > 2)
		rb_raise(rb_eRuntimeError, "param_index must be 0..2");
	
	Data_Get_Struct(self, ParamHolder, param_holder);		
	rb_ary_store(rb_iv_get(self, "@result"), idx, out_array);
	// FIXME: malloc w/o free: johnribera@hotmail.com fixed
	dp = &(param_holder->out[idx]); 
  if (*dp) free(*dp); // not true only 1st time called (reusing same ptrs)
	
	*dp = (double*)malloc(RARRAY_LEN(out_array) * sizeof(double));
	ret_code = TA_SetOutputParamRealPtr(param_holder->p, idx, *dp);
	
	if ( ret_code != TA_SUCCESS )
		rb_raise(rb_eRuntimeError, "unsuccess return code TA_SetOutputParamRealPtr");

	return out_array; 
}

static VALUE ta_func_setup_out_integer(VALUE self, VALUE param_index, VALUE out_array)
{
	TA_RetCode ret_code;
	ParamHolder *param_holder;
  long idx = FIX2INT(param_index); 
  int **ip;
	if (idx > 2)
		rb_raise(rb_eRuntimeError, "param_index must be 0..2");
	
	Data_Get_Struct(self, ParamHolder, param_holder);
	rb_ary_store(rb_iv_get(self, "@result"), idx, out_array);
	
	// FIXME: malloc w/o free FIXED: johnribera@Hotmail.com
  ip = (int**)&(param_holder->out[idx]); 
  if (*ip) free(*ip); // not true only very 1st time in
	*ip = (int*)malloc(RARRAY_LEN(out_array) * sizeof(int));
	
	ret_code = TA_SetOutputParamIntegerPtr( param_holder->p, idx, *ip);
	if ( ret_code != TA_SUCCESS )
		rb_raise(rb_eRuntimeError, "unsuccess return code TA_SetOutputParamIntegerPtr");

	return out_array;
}

/*
 * call-seq: call(in_start, in_end)
 *
 * Call technical function with input data from in_start..in_end.
 */
static VALUE ta_func_call(VALUE self, VALUE in_start, VALUE in_end)
{
	TA_RetCode ret_code;
	ParamHolder *param_holder;
	TA_Integer out_start, out_num;
	VALUE ary, sub_ary;
	int i,j;

	Data_Get_Struct(self, ParamHolder, param_holder);		
	ret_code = TA_CallFunc( param_holder->p, FIX2INT(in_start), FIX2INT(in_end), &out_start, &out_num);
	if ( ret_code != TA_SUCCESS )
		rb_raise(rb_eRuntimeError, "unsuccess return code TA_CallFunc");
	ary = rb_iv_get(self, "@result");
	for (i = 0; i<RARRAY_LEN(ary); i++)
		if (TYPE(rb_ary_entry(ary, i)) == T_ARRAY)
		{
			sub_ary = rb_ary_entry(ary, i);
			for (j=0; j<out_num; j++)
			{
				double el = ((double*)param_holder->out[i])[j];
				rb_ary_store(sub_ary, j+out_start, rb_float_new(el));
			}
		}
	return rb_ary_new3(2, INT2FIX(out_start), INT2FIX(out_num));
}

static VALUE ta_func_lookback(VALUE self)
{
	TA_RetCode ret_code;
	ParamHolder *param_holder;
	TA_Integer out_lookback;

	Data_Get_Struct(self, ParamHolder, param_holder);
	ret_code = TA_GetLookback(param_holder->p, &out_lookback);
	if ( ret_code != TA_SUCCESS )
		rb_raise(rb_eRuntimeError, "unsuccess return code TA_GetLookback");
	
  return INT2FIX(out_lookback);
}

void Init_talib()
{

	/* 
	 * Ruby extension for technical functions library api.
	 */
	rb_mTaLib = rb_define_module("TaLib");

	rb_define_const(rb_mTaLib, "TA_Input_Price", INT2FIX(TA_Input_Price));
	rb_define_const(rb_mTaLib, "TA_Input_Real", INT2FIX(TA_Input_Real));
	rb_define_const(rb_mTaLib, "TA_Input_Integer", INT2FIX(TA_Input_Integer));
	rb_define_const(rb_mTaLib, "TA_IN_PRICE_OPEN", INT2FIX(TA_IN_PRICE_OPEN));
	rb_define_const(rb_mTaLib, "TA_IN_PRICE_HIGH", INT2FIX(TA_IN_PRICE_HIGH));
	rb_define_const(rb_mTaLib, "TA_IN_PRICE_LOW", INT2FIX(TA_IN_PRICE_LOW));
	rb_define_const(rb_mTaLib, "TA_IN_PRICE_CLOSE", INT2FIX(TA_IN_PRICE_CLOSE));
	rb_define_const(rb_mTaLib, "TA_IN_PRICE_VOLUME", INT2FIX(TA_IN_PRICE_VOLUME));
	rb_define_const(rb_mTaLib, "TA_IN_PRICE_OPENINTEREST", INT2FIX(TA_IN_PRICE_OPENINTEREST));
	rb_define_const(rb_mTaLib, "TA_IN_PRICE_TIMESTAMP", INT2FIX(TA_IN_PRICE_TIMESTAMP));
  rb_define_const(rb_mTaLib, "TA_OptInput_RealRange", INT2FIX(TA_OptInput_RealRange));
  rb_define_const(rb_mTaLib, "TA_OptInput_RealList", INT2FIX(TA_OptInput_RealList));
  rb_define_const(rb_mTaLib, "TA_OptInput_IntegerRange", INT2FIX(TA_OptInput_IntegerRange));
  rb_define_const(rb_mTaLib, "TA_OptInput_IntegerList", INT2FIX(TA_OptInput_IntegerList));
	rb_define_const(rb_mTaLib, "TA_OPTIN_IS_PERCENT", INT2FIX(TA_OPTIN_IS_PERCENT));
	rb_define_const(rb_mTaLib, "TA_OPTIN_IS_DEGREE", INT2FIX(TA_OPTIN_IS_DEGREE));
	rb_define_const(rb_mTaLib, "TA_OPTIN_IS_CURRENCY", INT2FIX(TA_OPTIN_IS_CURRENCY));
	rb_define_const(rb_mTaLib, "TA_OPTIN_ADVANCED", INT2FIX(TA_OPTIN_ADVANCED));
  rb_define_const(rb_mTaLib, "TA_Output_Real", INT2FIX(TA_Output_Real));
  rb_define_const(rb_mTaLib, "TA_Output_Integer", INT2FIX(TA_Output_Integer));
	rb_define_const(rb_mTaLib, "TA_OUT_LINE", INT2FIX(TA_OUT_LINE));
	rb_define_const(rb_mTaLib, "TA_OUT_DOT_LINE", INT2FIX(TA_OUT_DOT_LINE));
	rb_define_const(rb_mTaLib, "TA_OUT_DASH_LINE", INT2FIX(TA_OUT_DASH_LINE));
	rb_define_const(rb_mTaLib, "TA_OUT_DOT", INT2FIX(TA_OUT_DOT));
	rb_define_const(rb_mTaLib, "TA_OUT_HISTO", INT2FIX(TA_OUT_HISTO));
	rb_define_const(rb_mTaLib, "TA_OUT_PATTERN_BOOL", INT2FIX(TA_OUT_PATTERN_BOOL));
	rb_define_const(rb_mTaLib, "TA_OUT_PATTERN_BULL_BEAR", INT2FIX(TA_OUT_PATTERN_BULL_BEAR));
	rb_define_const(rb_mTaLib, "TA_OUT_PATTERN_STRENGTH", INT2FIX(TA_OUT_PATTERN_STRENGTH));
	rb_define_const(rb_mTaLib, "TA_OUT_POSITIVE", INT2FIX(TA_OUT_POSITIVE));
	rb_define_const(rb_mTaLib, "TA_OUT_NEGATIVE", INT2FIX(TA_OUT_NEGATIVE));
	rb_define_const(rb_mTaLib, "TA_OUT_ZERO", INT2FIX(TA_OUT_ZERO));
	rb_define_const(rb_mTaLib, "TA_OUT_UPPER_LIMIT", INT2FIX(TA_OUT_UPPER_LIMIT));
	rb_define_const(rb_mTaLib, "TA_OUT_LOWER_LIMIT", INT2FIX(TA_OUT_LOWER_LIMIT));
  rb_define_const(rb_mTaLib, "TA_MAType_SMA", INT2FIX((int)(TA_MAType_SMA)));
  rb_define_const(rb_mTaLib, "TA_MAType_EMA", INT2FIX((int)(TA_MAType_EMA)));
  rb_define_const(rb_mTaLib, "TA_MAType_WMA", INT2FIX((int)(TA_MAType_WMA)));
  rb_define_const(rb_mTaLib, "TA_MAType_DEMA", INT2FIX((int)(TA_MAType_DEMA)));
  rb_define_const(rb_mTaLib, "TA_MAType_TEMA", INT2FIX((int)(TA_MAType_TEMA)));
  rb_define_const(rb_mTaLib, "TA_MAType_TRIMA", INT2FIX((int)(TA_MAType_TRIMA)));
  rb_define_const(rb_mTaLib, "TA_MAType_KAMA", INT2FIX((int)(TA_MAType_KAMA)));
  rb_define_const(rb_mTaLib, "TA_MAType_MAMA", INT2FIX((int)(TA_MAType_MAMA)));
  rb_define_const(rb_mTaLib, "TA_MAType_T3", INT2FIX((int)(TA_MAType_T3)));

	rb_struct_define("TA_RealRange", "min", "max", "precision", NULL);
	rb_struct_define("TA_IntegerRange", "min", "max", NULL );
	rb_struct_define("TA_RealDataPair", "value", "string", NULL );
	rb_struct_define("TA_IntegerDataPair", "value", "string", NULL );
	rb_struct_define("TA_RealList", "data", "nb_element", NULL );
	rb_struct_define("TA_IntegerList", "data", "nb_element", NULL );

	rb_sInParamInfo = rb_struct_define("TA_InputParameterInfo", "type", "param_name", "flags", NULL );
	rb_sOptInParamInfo = rb_struct_define("TA_OptInputParameterInfo", "type", "param_name", "flags", "display_name", "data_set", "default_value", "hint", "help_file", NULL );
	rb_sOutParamInfo = rb_struct_define("TA_OutputParameterInfo", "type", "param_name", "flags", NULL );

	/*
	 * Class for technical analysis function.
	 */
	rb_cTAFunction = rb_define_class_under(rb_mTaLib, "Function", rb_cObject);
	rb_define_alloc_func(rb_cTAFunction, ta_func_alloc);
	rb_define_method(rb_cTAFunction, "initialize", ta_func_initialize, 1);
	rb_define_attr(rb_cTAFunction, "result", 1, 1);

    // define @@group, @@ functions and their accessors
    // under Function:Class.
    init_tables();
	rb_define_module_function( rb_cTAFunction, "groups", ta_func_get_groups, 		0 );
	rb_define_module_function( rb_cTAFunction, "functions", ta_func_get_functions, 	0 );

    // define accessor for @name.
    rb_define_method( rb_cTAFunction,
                      "name",  ta_func_get_name, 0 );

	rb_define_method( rb_cTAFunction, "ins",  ta_func_get_input_count, 			  0 );
	rb_define_method( rb_cTAFunction, "outs", ta_func_get_output_count, 		  0 );
	rb_define_method( rb_cTAFunction, "opts", ta_func_get_option_input_count, 0 );

	rb_define_method( rb_cTAFunction, "in",  ta_func_input_param_info,  1);
	rb_define_method( rb_cTAFunction, "opt", ta_func_option_param_info, 1);
	rb_define_method( rb_cTAFunction, "out", ta_func_output_param_info, 1);

	rb_define_method( rb_cTAFunction, "in_int", 	ta_func_setup_in_integer, 2);
	rb_define_method( rb_cTAFunction, "in_real", 	ta_func_setup_in_real, 		2);
    rb_define_method( rb_cTAFunction, "in_price",   ta_func_setup_in_price, 7);
	rb_define_method( rb_cTAFunction, "opt_int", 	ta_func_setup_opt_in_integer, 2);
	rb_define_method( rb_cTAFunction, "opt_real", ta_func_setup_opt_in_real, 		2);
    rb_define_method( rb_cTAFunction, "out_int", 	ta_func_setup_out_integer, 		2);
	rb_define_method( rb_cTAFunction, "out_real", ta_func_setup_out_real, 			2);

    rb_define_method( rb_cTAFunction, "lookback", ta_func_lookback, 0);
	rb_define_method( rb_cTAFunction, "call", ta_func_call, 2);
}
