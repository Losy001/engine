#pragma once

#define FreeList(T, N) \
	struct \
	{ \
		struct \
		{ \
			T data; \
			uint32_t next; \
		} elements[N]; \
		\
		uint32_t head; \
		uint32_t first_free; \
		uint32_t count; \
	} 

#define fl_default() \
	{ \
		.head = (uint32_t)-1, \
		.first_free = (uint32_t)-1, \
		.count = 0 \
	} 

#define fl_init(list) \
	({ \
		(list)->head = (uint32_t)-1; \
		(list)->first_free = (uint32_t)-1; \
		(list)->count = 0; \
	});

#define fl_push(list, ...) \
	({ \
		uint32_t idx; \
		\
		if ((list)->first_free == (uint32_t)-1) \
		{ \
			idx = (list)->count++; \
		} \
		else \
		{ \
			idx = (uint32_t)(list)->first_free; \
			(list)->first_free = (list)->elements[idx].next; \
		} \
		\
		(list)->elements[idx].next = (list)->head; \
		(list)->head = idx; \
		(list)->elements[idx].data = __VA_ARGS__; \
		\
		idx; \
	})

#define fl_get(list, idx) ({ &(list)->elements[idx].data; })

#define fl_remove(list, idx) \
	({ \
		if ((list)->head == idx) \
		{ \
			(list)->head = (list)->elements[idx].next; \
		} \
		else \
		{ \
			uint32_t prev = (list)->head; \
			while (prev != (uint32_t)-1 && (list)->elements[prev].next != idx) \
			{ \
				prev = (list)->elements[prev].next; \
			} \
			if (prev != (uint32_t)-1) \
			{ \
				(list)->elements[prev].next = (list)->elements[idx].next; \
			} \
		} \
		\
		(list)->elements[idx].next = (list)->first_free; \
		(list)->first_free = idx; \
	})

#define fl_for_each(list, ...) \
	do \
	{ \
		uint32_t idx = (list)->head; \
		\
		while (idx != (uint32_t)-1) \
		{ \
			__VA_ARGS__ \
			idx = (list)->elements[idx].next; \
		} \
	} while (0)
	