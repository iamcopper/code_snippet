/*
 * Traverse Struct Array
 * SampleCode: code_snippet/rpc_intf/src/rpc_cmd.c
 */
#include <stdio.h>

int main(int argc, char *argv[])
{
	int i = 0;
	char *strs[] = {
		"chen",
		"yu",
		"hello",
		"word",
		NULL,
	};

	struct person1 {
		char *name;
		int age;
	};
	struct person1 persons1[] = {
		{"zhang", 25},
		{"wang", 32},
		{"li", 29},
		{"zhao", 15},
		{ NULL },
	};

	struct person2 {
		int age;
		char *name;
	};
	struct person2 persons2[] = {
		{25, "zhang"},
		{32, "wang"},
		{29, "li"},
		{15, "zhao"},
		{ 0 },
	};

	for (i = 0; strs[i] != NULL; ++i) {
		printf("strs[%d]=%s\n", i, strs[i]);
	}

	printf("\n");
	for (i = 0; persons1[i].name != NULL; ++i) {
		printf("persons1[%d]: name=%s, age=%d\n", i, persons1[i].name, persons1[i].age);
	}

	printf("\n");
	for (i = 0; persons2[i].age != 0; ++i) {
		printf("persons2[%d]: age=%d, name=%s\n", i, persons2[i].age, persons2[i].name);
	}

	return 0;
}
