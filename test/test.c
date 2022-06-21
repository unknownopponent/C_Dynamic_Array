
#include "../C_Dynamic_Array.h"

#include <stdio.h>

char check_array(int* result, int* reference, size_t count)
{
	for (size_t i = 0; i < count; i++)
		if (result[i] != reference[i])
			return 1;

	return 0;
}

int main(void)
{
	DynamicArray da;
	da_init(&da, sizeof(int));
	da_reserve(&da, 8);
	da_insert(&da, 0, 8);

	int* p = da_iterator(&da);

	for (int i = 0; i < 8; i++)
		p[i] = i;

	//change count test

	da_remove(&da, 7, 1);

	if (da.element_count != 7)
	{
		fprintf(stderr, "da_remove incorrect element count %zu, 7 expected\n", da.element_count);
		return 1;
	}
	int remove1[] = { 0, 1, 2, 3, 4, 5, 6 };
	if (check_array(da_iterator(&da), remove1, 7))
	{
		fputs("da_remove incorrect result 1\n", stderr);
		return 1;
	}

	//change head test

	da_remove(&da, 0, 2);

	if (da.element_count != 5)
	{
		fprintf(stderr, "da_remove incorrect element count %zu, 5 expected\n", da.element_count);
		return 1;
	}
	int remove2[] = { 2, 3, 4, 5, 6 };
	if (check_array(da_iterator(&da), remove2, 5))
	{
		fputs("da_remove incorrect result 2\n", stderr);
		return 1;
	}

	//memmove test

	da_remove(&da, 1, 3);

	if (da.element_count != 2)
	{
		fprintf(stderr, "da_remove incorrect element count %zu, 2 expected\n", da.element_count);
		return 1;
	}
	int remove3[] = { 2, 6 };
	if (check_array(da_iterator(&da), remove3, 2))
	{
		fputs("da_remove incorrect result 3\n", stderr);
		return 1;
	}

	da_release(&da);

	puts("da_remove checks passed");

	//no elements tests

	da_init(&da, sizeof(int));
	da_insert(&da, 0, 2);
	da_remove(&da, 0, 1);
	da_remove(&da, 0, 1);
	da_insert(&da, 0, 3);

	if (da.element_count != 3)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 3 expected\n", da.element_count);
		return 1;
	}

	p = da_iterator(&da);

	for (int i = 0; i < 3; i++)
		p[i] = i;

	//change head test

	da_remove(&da, 0, 1);
	da_insert(&da, 0, 1);

	*(int*)da_iterator(&da) = 0;

	if (da.element_count != 3)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 3 expected\n", da.element_count);
		return 1;
	}
	int insert1[] = { 0, 1, 2 };
	if (check_array(da_iterator(&da), insert1, 3))
	{
		fputs("da_insert incorrect result 2\n", stderr);
		return 1;
	}

	//change element count test

	da_remove(&da, 2, 1);
	da_insert(&da, 2, 1);

	*((int*)da_iterator(&da) + 2)= 2;

	if (da.element_count != 3)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 3 expected\n", da.element_count);
		return 1;
	}
	if (check_array(da_iterator(&da), insert1, 3))
	{
		fputs("da_insert incorrect result 3\n", stderr);
		return 1;
	}

	//memmove index 0 test

	da_remove(&da, 2, 1);
	da_insert(&da, 0, 1);

	*(int*)da_iterator(&da) = 2;

	if (da.element_count != 3)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 3 expected\n", da.element_count);
		return 1;
	}
	int insert2[] = { 2, 0, 1 };
	if (check_array(da_iterator(&da), insert2, 3))
	{
		fputs("da_insert incorrect result 4\n", stderr);
		return 1;
	}

	//memmove last index test

	da_remove(&da, 0, 1);
	da_insert(&da, 2, 1);

	*((int*)da_iterator(&da) + 2) = 2;

	if (da.element_count != 3)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 3 expected\n", da.element_count);
		return 1;
	}
	if (check_array(da_iterator(&da), insert1, 3))
	{
		fputs("da_insert incorrect result 5\n", stderr);
		return 1;
	}

	//realloc index 0 test

	da_insert(&da, 0, 1);

	*(int*)da_iterator(&da) = 3;

	if (da.element_count != 4)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 4 expected\n", da.element_count);
		return 1;
	}
	int insert3[] = { 3, 0, 1, 2 };
	if (check_array(da_iterator(&da), insert3, 4))
	{
		fputs("da_insert incorrect result 6\n", stderr);
		return 1;
	}
	da_shrink_to_fit(&da);

	//realloc last index test

	da_insert(&da, 4, 1);

	*((int*)da_iterator(&da) + 4) = 4;

	if (da.element_count != 5)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 5 expected\n", da.element_count);
		return 1;
	}
	int insert4[] = { 3, 0, 1, 2, 4 };
	if (check_array(da_iterator(&da), insert4, 5))
	{
		fputs("da_insert incorrect result 7\n", stderr);
		return 1;
	}
	da_shrink_to_fit(&da);

	//memmove head 

	da_remove(&da, 0, 1);
	da_insert(&da, 3, 1);

	*((int*)da_iterator(&da) + 3) = 3;

	if (da.element_count != 5)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 5 expected\n", da.element_count);
		return 1;
	}
	int insert5[] = { 0, 1, 2, 3, 4 };
	if (check_array(da_iterator(&da), insert5, 5))
	{
		fputs("da_insert incorrect result 8\n", stderr);
		return 1;
	}

	//memmove last

	da_remove(&da, 4, 1);
	da_insert(&da, 3, 1);

	*((int*)da_iterator(&da) + 3) = 3;

	if (da.element_count != 5)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 5 expected\n", da.element_count);
		return 1;
	}
	int insert6[] = { 0, 1, 2, 3, 3 };
	if (check_array(da_iterator(&da), insert6, 5))
	{
		fputs("da_insert incorrect result 9\n", stderr);
		return 1;
	}

	//memmove 

	da_remove(&da, 4, 1);
	da_remove(&da, 0, 1);
	da_insert(&da, 1, 2);

	*((int*)da_iterator(&da) + 1) = 0;
	*((int*)da_iterator(&da) + 2) = 4;

	if (da.element_count != 5)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 5 expected\n", da.element_count);
		return 1;
	}
	int insert7[] = { 1, 0, 4, 2, 3 };
	if (check_array(da_iterator(&da), insert7, 5))
	{
		fputs("da_insert incorrect result 10\n", stderr);
		return 1;
	}

	//realloc

	da_insert(&da, 1, 2);

	*((int*)da_iterator(&da) + 1) = 5;
	*((int*)da_iterator(&da) + 2) = 6;

	if (da.element_count != 7)
	{
		fprintf(stderr, "da_insert incorrect element count %zu, 7 expected\n", da.element_count);
		return 1;
	}
	int insert8[] = { 1, 5, 6, 0, 4, 2, 3 };
	if (check_array(da_iterator(&da), insert8, 7))
	{
		fputs("da_insert incorrect result 11\n", stderr);
		return 1;
	}

	da_release(&da);

	puts("da_insert checks passed");

	return 0;
}