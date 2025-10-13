#ifndef __STDATOMIC_H
#define __STDATOMIC_H

#define atomic_bool _Atomic _Bool
#define atomic_char _Atomic char
#define atomic_schar _Atomic signed char
#define atomic_uchar _Atomic unsigned char
#define atomic_short _Atomic short
#define atomic_ushort _Atomic unsigned short
#define atomic_int _Atomic int
#define atomic_uint _Atomic unsigned int
#define atomic_long _Atomic long
#define atomic_ulong _Atomic unsigned long
#define atomic_llong _Atomic long long
#define atomic_ullong _Atomic unsigned long long
#define atomic_char16_t _Atomic char16_t
#define atomic_char32_t _Atomic char32_t
#define atomic_wchar_t _Atomic wchar_t
#define atomic_int_least8_t _Atomic int_least8_t
#define atomic_uint_least8_t _Atomic uint_least8_t
#define atomic_int_least16_t _Atomic int_least16_t
#define atomic_uint_least16_t _Atomic uint_least16_t
#define atomic_int_least32_t _Atomic int_least32_t
#define atomic_uint_least32_t _Atomic uint_least32_t
#define atomic_int_least64_t _Atomic int_least64_t
#define atomic_uint_least64_t _Atomic uint_least64_t
#define atomic_int_fast8_t _Atomic int_fast8_t
#define atomic_uint_fast8_t _Atomic uint_fast8_t
#define atomic_int_fast16_t _Atomic int_fast16_t
#define atomic_uint_fast16_t _Atomic uint_fast16_t
#define atomic_int_fast32_t _Atomic int_fast32_t
#define atomic_uint_fast32_t _Atomic uint_fast32_t
#define atomic_int_fast64_t _Atomic int_fast64_t
#define atomic_uint_fast64_t _Atomic uint_fast64_t
#define atomic_intptr_t _Atomic intptr_t
#define atomic_uintptr_t _Atomic uintptr_t
#define atomic_size_t _Atomic size_t
#define atomic_ptrdiff_t _Atomic ptrdiff_t
#define atomic_intmax_t _Atomic intmax_t
#define atomic_uintmax_t _Atomic uintmax_t

typedef struct {
	volatile _Atomic int __value;
} atomic_flag;

#if defined(__GNUC__)
typedef enum {
	memory_order_relaxed = __ATOMIC_RELAXED,
	memory_order_consume = __ATOMIC_CONSUME,
	memory_order_acquire = __ATOMIC_ACQUIRE,
	memory_order_release = __ATOMIC_RELEASE,
	memory_order_acq_rel = __ATOMIC_ACQ_REL,
	memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

#define atomic_flag_clear_explicit(__ATOMIC_FLAG, __MEMORY_ORDER) \
	__atomic_clear((__ATOMIC_FLAG), (__MEMORY_ORDER))

#define atomic_flag_clear(__ATOMIC_FLAG) \
	atomic_flag_clear_explicit((__ATOMIC_FLAG), memory_order_seq_cst)

#define atomic_flag_test_and_set_explicit(__ATOMIC_FLAG, __MEMORY_ORDER) \
	__atomic_test_and_set((__ATOMIC_FLAG), (__MEMORY_ORDER))

#define atomic_flag_test_and_set(__ATOMIC_FLAG) \
	atomic_flag_test_and_set_explicit((__ATOMIC_FLAG), memory_order_seq_cst)

#define atomic_signal_fence(__MEMORY_ORDER) \
	__atomic_signal_fence(__MEMORY_ORDER)

#define atomic_thread_fence(__MEMORY_ORDER) \
	__atomic_thread_fence(__MEMORY_ORDER)

#define atomic_compare_exchange_strong_explicit(__ATOMIC_OBJECT_PTR,      \
						__VALUE_PTR, __DESIRED,   \
						__SUCCESS_MEMORDER,       \
						__FAILURE_MEMORDER)       \
	__atomic_compare_exchange_n((__ATOMIC_OBJECT_PTR), (__VALUE_PTR), \
				    (__DESIRED), 0, (__SUCCESS_MEMORDER), \
				    (__FAILURE_MEMORDER))

#define atomic_compare_exchange_strong(__ATOMIC_OBJECT_PTR, __VALUE_PTR,    \
				       __DESIRED)                           \
	atomic_compare_exchange_strong_explicit((__ATOMIC_OBJECT_PTR),      \
						(__VALUE_PTR), (__DESIRED), \
						memory_order_seq_cst,       \
						memory_order_seq_cst)

#define atomic_compare_exchange_weak_explicit(__ATOMIC_OBJECT_PTR,        \
					      __VALUE_PTR, __DESIRED,     \
					      __SUCCESS_MEMORDER,         \
					      __FAILURE_MEMORDER)         \
	__atomic_compare_exchange_n((__ATOMIC_OBJECT_PTR), (__VALUE_PTR), \
				    (__DESIRED), 1, (__SUCCESS_MEMORDER), \
				    (__FAILURE_MEMORDER))

#define atomic_compare_exchange_weak(__ATOMIC_OBJECT_PTR, __VALUE_PTR,    \
				     __DESIRED)                           \
	atomic_compare_exchange_weak_explicit((__ATOMIC_OBJECT_PTR),      \
					      (__VALUE_PTR), (__DESIRED), \
					      memory_order_seq_cst,       \
					      memory_order_seq_cst)

#define atomic_exchange_explicit(__ATOMIC_OBJECT_PTR, __DESIRED, \
				 __MEMORY_ORDER)                 \
	__atomic_exchange_n((__ATOMIC_OBJECT_PTR), (__DESIRED),  \
			    (__MEMORY_ORDER))

#define atomic_exchange(__ATOMIC_OBJECT_PTR, __DESIRED)              \
	atomic_exchange_explicit((__ATOMIC_OBJECT_PTR), (__DESIRED), \
				 memory_order_seq_cst)

#define atomic_fetch_add_explicit(__ATOMIC_OBJECT_PTR, __ARG, __MEMORY_ORDER) \
	__atomic_fetch_add((__ATOMIC_OBJECT_PTR), (__ARG), (__MEMORY_ORDER))

#define atomic_fetch_add(__ATOMIC_OBJECT_PTR, __ARG)              \
	atomic_fetch_add_explicit((__ATOMIC_OBJECT_PTR), (__ARG), \
				  memory_order_seq_cst)

#define atomic_fetch_and_explicit(__ATOMIC_OBJECT_PTR, __ARG, __MEMORY_ORDER) \
	__atomic_fetch_and((__ATOMIC_OBJECT_PTR), (__ARG), (__MEMORY_ORDER))

#define atomic_fetch_and(__ATOMIC_OBJECT_PTR, __ARG)              \
	atomic_fetch_and_explicit((__ATOMIC_OBJECT_PTR), (__ARG), \
				  memory_order_seq_cst)

#define atomic_fetch_or_explicit(__ATOMIC_OBJECT_PTR, __ARG, __MEMORY_ORDER) \
	__atomic_fetch_or((__ATOMIC_OBJECT_PTR), (__ARG), (__MEMORY_ORDER))

#define atomic_fetch_or(__ATOMIC_OBJECT_PTR, __ARG)              \
	atomic_fetch_or_explicit((__ATOMIC_OBJECT_PTR), (__ARG), \
				 memory_order_seq_cst)

#define atomic_fetch_sub_explicit(__ATOMIC_OBJECT_PTR, __ARG, __MEMORY_ORDER) \
	__atomic_fetch_sub((__ATOMIC_OBJECT_PTR), (__ARG), (__MEMORY_ORDER))

#define atomic_fetch_sub(__ATOMIC_OBJECT_PTR, __ARG)              \
	atomic_fetch_sub_explicit((__ATOMIC_OBJECT_PTR), (__ARG), \
				  memory_order_seq_cst)

#define atomic_fetch_xor_explicit(__ATOMIC_OBJECT_PTR, __ARG, __MEMORY_ORDER) \
	__atomic_fetch_xor((__ATOMIC_OBJECT_PTR), (__ARG), (__MEMORY_ORDER))

#define atomic_fetch_xor(__ATOMIC_OBJECT_PTR, __ARG)              \
	atomic_fetch_xor_explicit((__ATOMIC_OBJECT_PTR), (__ARG), \
				  memory_order_seq_cst)

#define atomic_load_explicit(__ATOMIC_OBJECT_PTR, __MEMORY_ORDER) \
	__atomic_load_n((__ATOMIC_OBJECT_PTR), (__MEMORY_ORDER))

#define atomic_load(__ATOMIC_OBJECT_PTR) \
	atomic_load_explicit((__ATOMIC_OBJECT_PTR), memory_order_seq_cst)

#define atomic_store_explicit(__ATOMIC_OBJECT_PTR, __DESIRED, __MEMORY_ORDER) \
	__atomic_store_n((__ATOMIC_OBJECT_PTR), (__DESIRED), (__MEMORY_ORDER))

#define atomic_store(__ATOMIC_OBJECT_PTR, __DESIRED)              \
	atomic_store_explicit((__ATOMIC_OBJECT_PTR), (__DESIRED), \
			      memory_order_seq_cst)

#define atomic_init(__ATOMIC_OBJECT_PTR, __DESIRED) \
	__atomic_store_n((__ATOMIC_OBJECT_PTR), (__DESIRED), __ATOMIC_RELAXED)

#define atomic_is_lock_free(__ATOMIC_OBJECT_PTR)              \
	__atomic_is_lock_free(sizeof(*(__ATOMIC_OBJECT_PTR)), \
			      (__ATOMIC_OBJECT_PTR))

#define ATOMIC_FLAG_INIT { 0 }

#define kill_dependency(x) (x)

#else
#error "Atomic operations are only supported with GCC"
#endif

#endif
