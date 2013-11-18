#ifndef __COMPARE_AND_SWAP_H__
#define __COMPARE_AND_SWAP_H__


/*
 * Do an atomic compare-and-swap operation.
 */
inline bool CAS(uint64_t& value_ref, uint64_t expected_value, uint64_t updated_value)
	__attribute__((always_inline));

#endif
