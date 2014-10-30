#include "channel_data_structures.h"
#include "channel_manager.pb.h"
#include <errno.h>
#include <stdio.h>

const char* t_rot_data_names[] = {
#define X(type, name, format, stype, pclass) #name,
	XFIELDS_ROTDATA
#undef X
};

const char* t_rot_data_typenames[] = {
#define X(type, name, format, stype, pclass) #type,
	XFIELDS_ROTDATA
#undef X
};

const int t_rot_data_fields_number =
#define X(type, name, format, stype, pclass) +1
	XFIELDS_ROTDATA
#undef X
;

int extractStructureFromClass(void* dst_structure, void* src_class, t_reflected_structures entity_type) {
	if ((dst_structure == NULL) || (src_class == NULL)) return -EINVAL;
	int ret = 0;
/*
//TODO: do something with that crap
#define Y(subtype, subname, subclass, type, name, format, stype, pclass) \
	pclass* ptr = static_cast<pclass*>(src_class); \
	if (ptr->has_##subname()) \
		if (ptr->subname)
*/
#define X(type, name, format, stype, pclass) \
	if (static_cast<pclass*>(src_class)->has_##name()) static_cast<stype*>(dst_structure)->name = static_cast<pclass*>(src_class)->name(); \
	else return -ENOENT;
	switch (entity_type) {
		case TRS_ROTDATA: {
			XFIELDS_ROTDATA
		break; }
		case TRS_CHANNEL_MANAGER_INIT_PARAMS: {
			XFIELDS_ROTCIP
		break; }
		default: {
			ret = -ERANGE;
		break; }
	}
#undef X

	return ret;
}

#define STR(inp) #inp
#define XSTR(inp) STR(inp)

void printStructure(void* src_structure, t_reflected_structures entity_type) {
	if (src_structure != NULL) {
#define X(type, name, format, stype, pclass) \
	printf("%s->%s = "format"\n",XSTR(stype),#name,static_cast<stype*>(src_structure)->name);
		switch (entity_type) {
			case TRS_ROTDATA: {
				XFIELDS_ROTDATA
			break; }
			case TRS_CHANNEL_MANAGER_INIT_PARAMS: {
				XFIELDS_ROTCIP
			break; }
			default: {
				printf("unknown entity\n");
			break; }
		}
#undef X
	} else {
		printf("You have passed null pointer!\n");
	}
}

void printClass(void* src_class, t_reflected_structures entity_type) {
	if (src_class != NULL) {
#define X(type, name, format, stype, pclass) \
	printf("%s->%s() = "format"\n",XSTR(pclass),#name,static_cast<pclass*>(src_class)->name());
		switch (entity_type) {
			case TRS_ROTDATA: {
				XFIELDS_ROTDATA
			break; }
			case TRS_CHANNEL_MANAGER_INIT_PARAMS: {
				XFIELDS_ROTCIP
			break; }
			default: {
				printf("unknows entity\n");
			break; }
		}
#undef X
	} else {
		printf("You have passed null pointer!\n");
	}
}
