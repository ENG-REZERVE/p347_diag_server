#ifndef __SMARTENUM_H__
#define __SMARTENUM_H__

//Author: David Saltares ( https://gist.github.com/siondream )
//Modified by Konstanting Utkin: added possibility to use arrays in different modules
//create const char* a_names[] = { HW_ACTIONS_LIST(SMARTENUM_STRING) };

#include <cstring>

//#define SMARTENUM_VALUE(typeName, value) e##typeName##_##value,
#define SMARTENUM_VALUE(typeName, value) value,
#define SMARTENUM_STRING(typeName, value) #value, 

#define SMARTENUM_DEFINE_ENUM(typeName, values) enum typeName { values(SMARTENUM_VALUE) e##typeName##_Count, };

//#define SMARTENUM_DEFINE_NAMES(typeName, values, dst_array) \
//		if ((const char*)dst_array != NULL) dst_array[] = { values(SMARTENUM_STRING) };
/*
#define SMARTENUM_DEFINE_GET_VALUE_FROM_STRING(typeName, name)	\
	typeName get##typeName##FromString(const char* str) 					\
	{				                      																\
		for (int i = 0; i < e##typeName##_Count; ++i)       				\
		if (!strcmp(##typeName##Array[i], str))											\
			return (##typeName##)i;																		\
		return e##typeName##_Count;																	\
	}
*/
#define getStringFromEnumValue(typeName, value) typeName##Array[##value]
//#define getEnumValueFromString(typeName, name)	get##typeName##FromString(##name)

#define getEnumValueFromString(dst_value_ptr, typeName, name, names_array)\
	if ((names_array != NULL) && (name != "") && (dst_value_ptr != NULL)) {\
		for (int sm_i=0; sm_i<e##typeName##_Count; sm_i++) {\
			if (!strcmp(names_array[sm_i],name)) {\
				*dst_value_ptr = (typeName)sm_i;\
			}\
		}\
	} else {\
		*dst_value_ptr = (typeName)0;\
	}

#endif  // __SMARTENUM_H__
