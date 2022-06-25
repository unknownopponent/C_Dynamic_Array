#pragma once

#include <stdint.h> /* uint8_t */
#include <string.h> /* memcpy, memmove */
#include <stdlib.h> /* size_t, malloc, free, realloc */
#include <assert.h> /* assert */

typedef struct DynamicArray
{
	void* data;
	size_t allocated;

	size_t element_size;
	size_t element_count;

	size_t head_offset;
} DynamicArray;

void da_init(DynamicArray* dynamic_array, size_t element_size);
void da_release(DynamicArray* dynamic_array);

/* return the array pointer */
void* da_iterator(DynamicArray* dynamic_array);

/* return 0 if success */
char da_insert(DynamicArray* dynamic_array, size_t index, size_t count);

void da_remove(DynamicArray* dynamic_array, size_t index, size_t count);

/* return 0 if success */
char da_reserve(DynamicArray* dynamic_array, size_t size);

/* return 0 if success */
char da_shrink_to_fit(DynamicArray* dynamic_array);

inline void da_init(DynamicArray* dynamic_array, size_t element_size)
{
	assert(dynamic_array != 0);
	assert(element_size != 0);

	dynamic_array->data = 0;
	dynamic_array->allocated = 0;
	dynamic_array->element_size = element_size;
	dynamic_array->element_count = 0;
	dynamic_array->head_offset = 0;
}

inline void da_release(DynamicArray* dynamic_array)
{
	assert(dynamic_array != 0);

	if (dynamic_array->data)
	{
		free(dynamic_array->data);
		dynamic_array->data = 0;
	}
	dynamic_array->element_count = 0;
	dynamic_array->allocated = 0;
}

inline void* da_iterator(DynamicArray* dynamic_array)
{
	assert(dynamic_array != 0);
	assert(dynamic_array->data != 0);
	assert(dynamic_array->allocated != 0);
	assert(dynamic_array->element_size != 0);
	assert(dynamic_array->element_count != 0);
	assert(dynamic_array->head_offset < dynamic_array->allocated);

	return (uint8_t*)dynamic_array->data + dynamic_array->head_offset;
}

inline char da_insert(DynamicArray* dynamic_array, size_t index, size_t count)
{
	assert(dynamic_array != 0);
	assert(count != 0);
	if (dynamic_array->element_count)
	{
		assert(dynamic_array->head_offset < dynamic_array->allocated);
		assert(index <= dynamic_array->element_count);
	}

	if (!dynamic_array->element_count)
	{
		size_t array_size = count * dynamic_array->element_size;

		if (!dynamic_array->data || dynamic_array->allocated < array_size)
		{
			void* tmp = realloc(dynamic_array->data, array_size);
			if (!tmp)
				return 1;

			dynamic_array->data = tmp;
			dynamic_array->allocated = array_size;
		}

		dynamic_array->head_offset = 0;
		dynamic_array->element_count = count;

		return 0;
	}

	size_t new_size = count * dynamic_array->element_size;

	if (index == 0)
	{
		if (dynamic_array->head_offset >= new_size)
		{
			dynamic_array->head_offset -= new_size;
			dynamic_array->element_count += count;
			return 0;
		}

		if (dynamic_array->allocated >= new_size + dynamic_array->element_count * dynamic_array->element_size)
		{
			size_t tmpoffset = dynamic_array->allocated - dynamic_array->element_count * dynamic_array->element_size;
			memmove(
				(uint8_t*)dynamic_array->data + tmpoffset,
				da_iterator(dynamic_array),
				dynamic_array->element_count * dynamic_array->element_size
			);
			dynamic_array->head_offset = tmpoffset - new_size;
			dynamic_array->element_count += count;
			return 0;
		}

		size_t tmp_size = (dynamic_array->allocated + new_size) * 2;
		void* tmp = malloc(tmp_size);
		if (!tmp)
			return 1;

		memcpy(
			(uint8_t*)tmp + tmp_size - dynamic_array->element_count * dynamic_array->element_size,
			da_iterator(dynamic_array),
			dynamic_array->element_count * dynamic_array->element_size
		);

		free(dynamic_array->data);

		dynamic_array->data = tmp;
		dynamic_array->allocated = tmp_size;
		dynamic_array->element_count += count;
		dynamic_array->head_offset = dynamic_array->allocated - dynamic_array->element_count * dynamic_array->element_size;

		return 0;
	}
	else if (index == dynamic_array->element_count)
	{
		if (dynamic_array->allocated - dynamic_array->head_offset - dynamic_array->element_count * dynamic_array->element_size >= new_size)
		{
			dynamic_array->element_count += count;
			return 0;
		}

		if (dynamic_array->allocated >= new_size + dynamic_array->element_count * dynamic_array->element_size)
		{
			memmove(
				(uint8_t*)dynamic_array->data,
				da_iterator(dynamic_array),
				dynamic_array->element_count * dynamic_array->element_size
			);
			dynamic_array->head_offset = 0;
			dynamic_array->element_count += count;
			return 0;
		}

		size_t tmp_size = (dynamic_array->allocated + new_size) * 2;
		void* tmp = malloc(tmp_size);
		if (!tmp)
			return 1;

		memcpy(
			tmp,
			da_iterator(dynamic_array),
			dynamic_array->element_count * dynamic_array->element_size
		);

		free(dynamic_array->data);

		dynamic_array->data = tmp;
		dynamic_array->allocated = tmp_size;
		dynamic_array->element_count += count;
		dynamic_array->head_offset = 0;

		return 0;
	}

	if ((dynamic_array->element_count + count) * dynamic_array->element_size <= dynamic_array->allocated)
	{
		if (dynamic_array->head_offset >= new_size)
		{
			memmove(
				(uint8_t*)da_iterator(dynamic_array) - new_size,
				da_iterator(dynamic_array),
				index * dynamic_array->element_size
			);

			dynamic_array->head_offset -= new_size;
			dynamic_array->element_count += count;
			
			return 0;
		}

		size_t first_part = index * dynamic_array->element_size;

		if (dynamic_array->allocated - dynamic_array->head_offset - dynamic_array->element_count * dynamic_array->element_size >= new_size)
		{
			memmove(
				(uint8_t*)da_iterator(dynamic_array) + (index + count) * dynamic_array->element_size,
				(uint8_t*)da_iterator(dynamic_array) + first_part,
				(dynamic_array->element_count - index) * dynamic_array->element_size
			);

			dynamic_array->element_count += count;

			return 0;
		}

		if (dynamic_array->head_offset != 0)
		{
			memmove(
				dynamic_array->data,
				da_iterator(dynamic_array),
				first_part
			);
		}

		memmove(
			(uint8_t*)dynamic_array->data + (index + count) * dynamic_array->element_size,
			(uint8_t*)da_iterator(dynamic_array) + first_part,
			(dynamic_array->element_count - index)* dynamic_array->element_size
		);

		dynamic_array->head_offset = 0;
		dynamic_array->element_count += count;

		return 0;
	}

	size_t tmp_size = (dynamic_array->allocated + new_size) * 2;
	void* tmp = malloc(tmp_size);
	if (!tmp)
		return 1;

	size_t first_part = index * dynamic_array->element_size;

	memcpy(
		tmp,
		da_iterator(dynamic_array),
		first_part
	);
	memcpy(
		(uint8_t*)tmp + first_part + count * dynamic_array->element_size,
		(uint8_t*)da_iterator(dynamic_array) + first_part,
		(dynamic_array->element_count - index) * dynamic_array->element_size
	);

	free(dynamic_array->data);

	dynamic_array->data = tmp;
	dynamic_array->allocated = tmp_size;
	dynamic_array->element_count += count;
	dynamic_array->head_offset = 0;

	return 0;
}

inline void da_remove(DynamicArray* dynamic_array, size_t index, size_t count)
{
	assert(dynamic_array != 0);
	assert(dynamic_array->data != 0);
	assert(dynamic_array->allocated != 0);
	assert(dynamic_array->element_size != 0);
	assert(dynamic_array->element_count != 0);
	assert(dynamic_array->head_offset < dynamic_array->allocated);
	assert(count != 0);
	assert(index < dynamic_array->element_count);
	assert(dynamic_array->element_count - index >= count);

	if (index == 0)
	{
		dynamic_array->head_offset += count * dynamic_array->element_size;
		dynamic_array->element_count -= count;
		return;
	}

	if (index == dynamic_array->element_count - count)
	{
		dynamic_array->element_count -= count;
		return;
	}

	void* array_p = da_iterator(dynamic_array);

	memmove(
		(uint8_t*)array_p + index * dynamic_array->element_size, 
		(uint8_t*)array_p + (index + count) * dynamic_array->element_size, 
		count * dynamic_array->element_size
	);

	dynamic_array->element_count -= count;
}

inline char da_reserve(DynamicArray* dynamic_array, size_t size)
{
	assert(dynamic_array != 0);
	assert(size != 0);

	void* tmp = realloc(dynamic_array->data, size);
	if (!tmp)
		return 1;

	dynamic_array->data = tmp;
	dynamic_array->allocated = size;
	return 0;
}

inline char da_shrink_to_fit(DynamicArray* dynamic_array)
{
	assert(dynamic_array != 0);
	assert(dynamic_array->element_size != 0);
	assert(dynamic_array->head_offset < dynamic_array->allocated);

	if (!dynamic_array->data)
		return 0;

	if (dynamic_array->element_count == 0)
	{
		if (dynamic_array->data)
		{
			free(dynamic_array->data);
			dynamic_array->data = 0;
			dynamic_array->allocated = 0;
		}
		return 0;
	}

	size_t array_size = dynamic_array->element_count * dynamic_array->element_size;

	if (!dynamic_array->head_offset)
	{
		if (array_size == dynamic_array->allocated)
			return 0;
	}
	else
	{
		memmove(dynamic_array->data, da_iterator(dynamic_array), array_size);
	}

	void* tmp = realloc(dynamic_array->data, array_size);
	if (!tmp)
		return 1;

	dynamic_array->data = tmp;
	dynamic_array->allocated = array_size;
	dynamic_array->head_offset = 0;
	return 0;
}
