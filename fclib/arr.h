#pragma once

#ifndef FCLIB_API
#define FCLIB_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FCLIB_ALIGNCAST
#define FCLIB_ALIGNCAST(to_type, value_ptr)                                    \
    (to_type *)__builtin_assume_aligned(value_ptr, _Alignof(to_type))
#endif

#include "str.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

// The str struct is just a wrapper around a byte array, so this means that
// it can be used for arrays of any type. The 'len' field of the 'str'
// struct is the dimensionality of the array, and the first 4xdimensionality
// bytes of the 'val' array in the 'str' struct are used to store the
// lengths of all dimensions of the array. And then, the rest of the 'val'
// field is the real data of the multi-dimensional array.
typedef fclib_str_t fclib_arr_t;

/// @function `arr_create`
/// @brief Creates an array with the given `dimensionality`, `element_size` and
/// the lengths of all dimensions as the vararg
///
/// @param `dimensionality` The number of dimensions of the rectangular array
/// @param `element_size` The number of bytes every element in the array is
/// taking up
/// @param `lengths` The lengths of all dimensions
/// @return `str *` The created array
FCLIB_API fclib_arr_t *fclib_arr_create( //
    const size_t dimensionality,         //
    const size_t element_size,           //
    const size_t *lengths                //
);

/// @function `arr_fill_seq`
/// @brief Fills all elements of the array with the provided value sequentially
///
/// @param `arr` The array to initialize
/// @param `element_size` The size of each element in bytes
/// @param `value` Pointer to the value to copy into each element
FCLIB_API void fclib_arr_fill_seq( //
    fclib_arr_t *arr,              //
    const size_t element_size,     //
    const void *value              //
);

/// @function `fill_arr_exp`
/// @brief Fills all elements of the array with the provided value using
/// exponential filling
///
/// @param `arr` The array to initialize
/// @param `element_size` The size of each element in bytes
/// @param `value` Pointer to the value to copy into each element
FCLIB_API void fclib_arr_fill_exp( //
    fclib_arr_t *arr,              //
    const size_t element_size,     //
    const void *value              //
);

/// @function `arr_get_slice_1d`
/// @brief Creates a slice of a one-dimensional array, which more or less boils
/// down to a single malloc and memcpy call. For one-dimensional array slicing
/// we do not need all that complex machinery of the generic function
///
/// @param `src` The source to get a slice from
/// @param `element_size` The size of each element in bytes
/// @param `range` The range of the slicing
/// @return `str *` The sliced array containing values [from, to) for the range
FCLIB_API fclib_arr_t *fclib_arr_get_slice_1d( //
    const fclib_arr_t *src,                    //
    const size_t element_size,                 //
    const size_t from,                         //
    const size_t to                            //
);

/// @function `arr_get_slice`
/// @brief Creates a slice of the given array, changing the dimensionality of
/// the access, for example doing 'arr[0, 0..5]' will result in a
/// one-dimensional array and 'arr[2..4, 4..8]' will cause a two-dimensional
/// rectangle array as a slice of the src array.
///
/// @param `src` The source array to get a slice from
/// @param `element_size` The size of each element in bytes
/// @param `ranges` The ranges of the slicing
/// @return `str *` The sliced array containing values [from, to) for all ranges
///
/// @note We also need to check whether the ranges even are in range
/// of our dimensionality lengths at all, that's pretty important actually
FCLIB_API fclib_arr_t *fclib_arr_get_slice( //
    const fclib_arr_t *src,                 //
    const size_t element_size,              //
    const size_t *ranges                    //
);

/// @function `arr_free`
/// @brief Frees the given array, where the complexity is the array depth (how
/// many complex data structures there are in the array)
///
/// @param `arr` The array to deep free
/// @param `complexity` The complexity of the array, e.g. how many more arrays
/// it contains
FCLIB_API void fclib_arr_free(fclib_arr_t *arr, const size_t complexity);

/// @function `arr_fill_deep`
/// @brief Fills the array and copies the value saved at the value pointer into
/// every slot of the array
///
/// @param `arr` The array to initialize
/// @param `value_size` The size of the complex structure to copy
/// @param `value` Pointer to the value to copy into each element
FCLIB_API void fclib_arr_fill_deep( //
    fclib_arr_t *arr,               //
    const size_t value_size,        //
    const void *value               //
);

/// @function `fill_arr_inline`
/// @brief Fills the given array inline by copying over the value from the given
/// value field
///
/// @param `arr` The array in which to fill the elements
/// @param `element_size` The size of a signle element
/// @param `value` The value containing the element to copy
FCLIB_API void fclib_arr_fill_inline( //
    fclib_arr_t *arr,                 //
    const size_t element_size,        //
    const void *value                 //
);

/// @function `arr_fill_val`
/// @brief Fills the array with the provided value using a hybrid approach
/// between sequential and exponential filling methods, based on the array
/// sizes. Only copies `element_size` bytes from the provided value. The `value`
/// parameter is only a container for the value to copy, the value can have any
/// size below or equal to 8 bytes.
///
/// @param `arr` The array to initialize
/// @param `element_size` The size of each element in bytes
/// @param `value` The value container which contains the value to copy (useful
/// for primitives)
FCLIB_API void fclib_arr_fill_val( //
    fclib_arr_t *arr,              //
    const size_t element_size,     //
    const size_t value             //
);

/// @function `arr_access`
/// @brief Returns a pointer to the element at position xyz... of the var args
///
/// @param `arr` The array to access
/// @param `element_size` The size of each element in bytes
/// @param `indices` The position of the element to access
/// @return `char *` Pointer to the element
FCLIB_API char *fclib_arr_access( //
    fclib_arr_t *arr,             //
    const size_t element_size,    //
    const size_t *indices         //
);

/// @function `arr_access_val`
/// @brief Returns the value of the element at the index of indices, packed in a
/// 8 byte container
///
/// @param `arr` The array to access
/// @param `element_size` The size of each element in bytes
/// @param `indices` The position of the element to access
/// @return `size_t` The value of the element
FCLIB_API size_t fclib_arr_access_val( //
    fclib_arr_t *arr,                  //
    const size_t element_size,         //
    const size_t *indices              //
);

/// @function `arr_assign_at`
/// @brief Assigns the value to the element at position xyz... of the var args
///
/// @param `arr` The array to access
/// @param `element_size` The size of each element in bytes
/// @param `indices` The position of the element to access
/// @param `value` The value to assign
FCLIB_API void fclib_arr_assign_at( //
    fclib_arr_t *arr,               //
    const size_t element_size,      //
    const size_t *indices,          //
    const void *value               //
);

/// @function `arr_assign_val_at`
/// @brief Assigns the value to the element at position xyz... of the var args
///
/// @param `arr` The array to access
/// @param `element_size` The size of each element in bytes
/// @param `indices` The position of the element to access
/// @param `value` The value to assign
FCLIB_API void fclib_arr_assign_val_at( //
    fclib_arr_t *arr,                   //
    const size_t element_size,          //
    const size_t *indices,              //
    const size_t value                  //
);

// #define FCLIB_STRIP_PREFIXES // Uncomment for debugging purposes
#ifdef FCLIB_STRIP_PREFIXES
// Inline-wrappers that forward to the non-stripped function. This is needed
// because if we would just do a "dumb" `#define fclib_str_create str_create`
// then we would also rename all the implementations and then we would have a
// potential symbol mismatch between translation units if in one TU the prefixes
// have been stripped but in the other one they have not. This way there exists
// 0 chance of different symbols in the different TUs, since the "stripped"
// functions are just inline-wrappers and they *should* disappear entirely at
// compile-time.
typedef fclib_str_t arr_t;

FCLIB_API static inline arr_t *arr_create( //
    const size_t dimensionality,           //
    const size_t element_size,             //
    const size_t *lengths                  //
) {
    return fclib_arr_create(dimensionality, element_size, lengths);
}
FCLIB_API static inline void arr_fill_seq( //
    arr_t *arr,                            //
    const size_t element_size,             //
    const void *value                      //
) {
    fclib_arr_fill_seq(arr, element_size, value);
}
FCLIB_API static inline void arr_fill_exp( //
    arr_t *arr,                            //
    const size_t element_size,             //
    const void *value                      //
) {
    fclib_arr_fill_exp(arr, element_size, value);
}
FCLIB_API static inline arr_t *arr_get_slice_1d( //
    const arr_t *src,                            //
    const size_t element_size,                   //
    const size_t from,                           //
    const size_t to                              //
) {
    return fclib_arr_get_slice_1d(src, element_size, from, to);
}
FCLIB_API static inline arr_t *arr_get_slice( //
    const arr_t *src,                         //
    const size_t element_size,                //
    const size_t *ranges                      //
) {
    return fclib_arr_get_slice(src, element_size, ranges);
}
FCLIB_API static inline void arr_free(arr_t *arr, const size_t complexity) {
    fclib_arr_free(arr, complexity);
}
FCLIB_API static inline void arr_fill_deep( //
    arr_t *arr,                             //
    const size_t value_size,                //
    const void *value                       //
) {
    fclib_arr_fill_deep(arr, value_size, value);
}
FCLIB_API static inline void arr_fill_inline( //
    arr_t *arr,                               //
    const size_t element_size,                //
    const void *value                         //
) {
    fclib_arr_fill_inline(arr, element_size, value);
}
FCLIB_API static inline void arr_fill_val( //
    arr_t *arr,                            //
    const size_t element_size,             //
    const size_t value                     //
) {
    fclib_arr_fill_val(arr, element_size, value);
}
FCLIB_API static inline char *arr_access( //
    arr_t *arr,                           //
    const size_t element_size,            //
    const size_t *indices                 //
) {
    return fclib_arr_access(arr, element_size, indices);
}
FCLIB_API static inline size_t arr_access_val( //
    arr_t *arr,                                //
    const size_t element_size,                 //
    const size_t *indices                      //
) {
    return fclib_arr_access_val(arr, element_size, indices);
}
FCLIB_API static inline void arr_assign_at( //
    arr_t *arr,                             //
    const size_t element_size,              //
    const size_t *indices,                  //
    const void *value                       //
) {
    fclib_arr_assign_at(arr, element_size, indices, value);
}
FCLIB_API static inline void arr_assign_val_at( //
    arr_t *arr,                                 //
    const size_t element_size,                  //
    const size_t *indices,                      //
    const size_t value                          //
) {
    fclib_arr_assign_val_at(arr, element_size, indices, value);
}
#endif // endof FCLIB_STRIP_PREFIXES

#ifdef __cplusplus
}
#endif

// #define FCLIB_IMPLEMENTATION // Uncomment for debugging purposes
#ifdef FCLIB_IMPLEMENTATION

FCLIB_API fclib_arr_t *fclib_arr_create( //
    const size_t dimensionality,         //
    const size_t element_size,           //
    const size_t *lengths                //
) {
    size_t arr_len = 1;
    size_t total_size = sizeof(fclib_arr_t) + dimensionality * sizeof(size_t);

    // Read the lengths of each dimension from the lengths
    for (size_t i = 0; i < dimensionality; i++) {
        arr_len *= lengths[i];
    }

    // Allocate memory for the str struct + its array
    fclib_arr_t *arr =
        (fclib_arr_t *)malloc(total_size + arr_len * element_size);

    // Set the dimensionality (len field)
    arr->len = dimensionality;

    // Store the lengths of each dimension in the value array
    memcpy(arr->value, lengths, dimensionality * sizeof(size_t));

    return arr;
}

FCLIB_API void fclib_arr_fill_seq( //
    fclib_arr_t *arr,              //
    const size_t element_size,     //
    const void *value              //
) {
    const size_t dimensionality = arr->len;
    size_t *const dim_lengths = FCLIB_ALIGNCAST(size_t, arr->value);

    // Calculate the total number of elements
    size_t total_elements = 1;
    for (size_t i = 0; i < dimensionality; i++) {
        total_elements *= dim_lengths[i];
    }

    // Get pointer to the start of the data area (after dimension lengths)
    char *const data_start = (char *)(dim_lengths + dimensionality);
    // Copy the initial value to the first element
    memcpy(data_start, value, element_size);

    // Fill all the other elements by copying the last element over to the next
    for (size_t i = 1; i < total_elements; i++) {
        memcpy(data_start + (i * element_size),
            data_start + ((i - 1) * element_size), element_size);
    }
}

FCLIB_API void fclib_arr_fill_exp( //
    fclib_arr_t *arr,              //
    const size_t element_size,     //
    const void *value              //
) {
    const size_t dimensionality = arr->len;
    size_t *const dim_lengths = FCLIB_ALIGNCAST(size_t, arr->value);

    // Calculate total number of elements
    size_t total_elements = 1;
    for (size_t i = 0; i < dimensionality; i++) {
        total_elements *= dim_lengths[i];
    }

    // Get pointer to the start of the data area (after dimension lengths)
    char *data_start = (char *)(dim_lengths + dimensionality);
    // Copy the initial value to the first element
    memcpy(data_start, value, element_size);

    // Use exponential copying to fill the rest of the array
    size_t filled = 1;
    while (filled < total_elements) {
        // Determine how much to copy in this iteration
        size_t to_copy = (filled <= total_elements - filled)
            ? filled
            : total_elements - filled;
        // Copy from the beginning of the array to the next unfilled portion
        memcpy(                                   //
            data_start + (filled * element_size), // dest
            data_start,                           // src
            to_copy * element_size                // len
        );
        // Update the count of filled elements
        filled += to_copy;
    }
}

FCLIB_API fclib_arr_t *fclib_arr_get_slice_1d( //
    const fclib_arr_t *src,                    //
    const size_t element_size,                 //
    const size_t from,                         //
    const size_t to                            //
) {
    const size_t src_len = *FCLIB_ALIGNCAST(size_t, src->value);
    size_t real_to = to == 0 ? src_len : to;
    if (real_to > src_len) {
        // Print error to tell a oob-slicing attempt was done and clamp to the
        // src len Because slicing is technically an array operation, the array
        // OOB options apply here So this could be a hard crash, silent, unsafe
        // or verbose
        real_to = src_len;
    }
    if (from == real_to) {
        // If real_to is equal to from, this block hard crashes no matter the
        // setting, as we would now handle with an explicit 'x..x' range, which
        // is not allowed, since the range does not contain a single element
        abort();
    }
    size_t real_from = from;
    if (from > real_to) {
        if (real_to == 0) {
            // This is a hard crash scenario, as 'from' would potentially be < 0
            // which is undefined for indexing
            abort();
        }
        // The lower bound of the range was above or equal to the upper bound of
        // the range The from value will get clamped to the value (real_to - 1)
        // As above, the OOB options apply to whether a print, crash etc is done
        // in here
        real_from = real_to - 1;
    }
    const size_t len = real_to - real_from;
    fclib_arr_t *slice = fclib_arr_create(1, element_size, &len);
    char *dest_ptr = (char *)(FCLIB_ALIGNCAST(size_t, slice->value) + 1);
    char *src_ptr = (char *)(FCLIB_ALIGNCAST(size_t, src->value) + 1);
    memcpy(dest_ptr, src_ptr + (real_from * element_size), len * element_size);
    return slice;
}

FCLIB_API fclib_arr_t *fclib_arr_get_slice( //
    const fclib_arr_t *src,                 //
    const size_t element_size,              //
    const size_t *ranges                    //
) {
    const size_t src_dimensionality = src->len;
    size_t *const src_dim_lengths = FCLIB_ALIGNCAST(size_t, src->value);

    // First, validate ranges and count new dimensionality
    size_t new_dimensionality = 0;
    for (size_t i = 0; i < src_dimensionality; i++) {
        const size_t from = ranges[i * 2];
        const size_t to = ranges[i * 2 + 1];
        if (from != to) {
            // Validate range bounds
            if (to > src_dim_lengths[i]) {
                // Out of bounds range
                return NULL;
            } else if (to - from < 2) {
                // "Range" is less than 2, so it's actually a single value,
                // which means that the dimensionality would decrase. But in
                // that case not a range but a single value should have been
                // provided instead
                return NULL;
            }
            new_dimensionality++;
        } else {
            // Validate single index
            if (from >= src_dim_lengths[i]) {
                // Index out of bounds
                return NULL;
            }
        }
    }
    assert(new_dimensionality > 0);
    const bool is_first_range = ranges[0] != ranges[1];
    if (src_dimensionality == 1 && new_dimensionality == 1) {
        assert(is_first_range);
        return fclib_arr_get_slice_1d(src, element_size, ranges[0], ranges[1]);
    }

    // Calculate new dimension lengths for ranges only
    size_t *new_dim_lengths = (size_t *)malloc( //
        new_dimensionality * sizeof(size_t)     //
    );
    size_t new_dim_index = 0;
    for (size_t i = 0; i < src_dimensionality; i++) {
        const size_t from = ranges[i * 2];
        const size_t to = ranges[i * 2 + 1];
        if (from != to) {
            new_dim_lengths[new_dim_index] = to - from;
            new_dim_index++;
        }
    }

    // Create the new sliced array
    fclib_arr_t *result = fclib_arr_create(               //
        new_dimensionality, element_size, new_dim_lengths //
    );
    char *const src_data = (char *const)(src_dim_lengths + src_dimensionality);
    char *const dest_data = (char *const)(                          //
        FCLIB_ALIGNCAST(size_t, result->value) + new_dimensionality //
    );

    // Calculate strides for each dimension in source array
    size_t *src_strides = (size_t *)malloc(src_dimensionality * sizeof(size_t));
    src_strides[0] = 1;
    for (size_t i = 1; i < src_dimensionality; i++) {
        src_strides[i] = src_strides[i - 1] * src_dim_lengths[i - 1];
    }

    // Calculate total elements in the result
    size_t total_result_elements = 1;
    for (size_t i = 0; i < new_dimensionality; i++) {
        total_result_elements *= new_dim_lengths[i];
    }

    // Copy elements from source to destination
    size_t dest_index = 0;

    // We need to iterate through all combinations of the ranges
    // This is a recursive-like problem, but we'll do it iteratively
    size_t *current_indices =
        (size_t *)malloc(src_dimensionality * sizeof(size_t));

    // Initialize indices to the start of each range/index
    for (size_t i = 0; i < src_dimensionality; i++) {
        current_indices[i] = ranges[i * 2];
    }

    // Get the chunk size of the extraction
    const size_t chunk_size = is_first_range ? ranges[1] - ranges[0] : 1;

    // Calculate how many chunks we need (total elements divided by chunk size)
    assert(total_result_elements % chunk_size == 0);
    const size_t num_chunks = total_result_elements / chunk_size;
    for (size_t chunk = 0; chunk < num_chunks; chunk++) {
        if (chunk > 0) {
            // Increment indices - skip the first dimension if it's a range
            // since we copied the whole chunk
            size_t start_dim = (size_t)is_first_range;
            for (size_t i = src_dimensionality - 1; i >= start_dim; i--) {
                const size_t from = ranges[i * 2];
                const size_t to = ranges[i * 2 + 1];
                if (from != to) {
                    current_indices[i]++;
                    if (current_indices[i] < to) {
                        break;
                    } else {
                        current_indices[i] = from;
                    }
                }
            }
        }
        // Calculate source offset
        size_t src_offset = 0;
        for (size_t i = 0; i < src_dimensionality; i++) {
            src_offset += current_indices[i] * src_strides[i];
        }

        // Copy the chunk (chunk_size elements)
        memcpy(dest_data + dest_index * element_size,
            src_data + src_offset * element_size, chunk_size * element_size);
        dest_index += chunk_size;
    }

    // Clean up
    free(new_dim_lengths);
    free(src_strides);
    free(current_indices);

    return result;
}

FCLIB_API void fclib_arr_free(fclib_arr_t *arr, const size_t complexity) {
    if (complexity == 0) {
        free(arr);
        return;
    }
    size_t dimensionality = arr->len;
    size_t *const lens = FCLIB_ALIGNCAST(size_t, arr->value);
    size_t length = 1;
    for (size_t i = 0; i < dimensionality; i++) {
        length *= lens[i];
    }
    fclib_arr_t **fields = (fclib_arr_t **)(lens + dimensionality);
    size_t next_complexity = complexity - 1;
    for (size_t i = 0; i < length; i++) {
        fclib_arr_free(fields[i], next_complexity);
    }
    free(arr);
}

FCLIB_API void fclib_arr_fill_deep( //
    fclib_arr_t *arr,               //
    const size_t value_size,        //
    const void *value               //
) {
    const size_t dimensionality = arr->len;
    size_t *const dim_lengths = FCLIB_ALIGNCAST(size_t, arr->value);
    size_t total_elements = 1;
    for (size_t i = 0; i < dimensionality; i++) {
        total_elements *= dim_lengths[i];
    }
    void **data_start = (void **)(dim_lengths + dimensionality);

    // Sequential fill
    void **element_ptr;
    for (size_t i = 0; i < total_elements; i++) {
        element_ptr = data_start + i;
        *element_ptr = malloc(value_size);
        memcpy(*element_ptr, value, value_size);
    }
}

FCLIB_API void fclib_arr_fill_inline( //
    fclib_arr_t *arr,                 //
    const size_t element_size,        //
    const void *value                 //
) {
    const size_t dimensionality = arr->len;
    size_t *const dim_lengths = FCLIB_ALIGNCAST(size_t, arr->value);
    size_t total_elements = 1;
    for (size_t i = 0; i < dimensionality; i++) {
        total_elements *= dim_lengths[i];
    }
    char *data_start = (char *)(dim_lengths + dimensionality);
    memcpy(data_start, value, element_size);
    // Use exponential approach for small elements or sequential for large
    // elements
    if (element_size < 128) { // Threshold based on benchmarks
        // Exponential fill
        size_t filled = 1;
        while (filled < total_elements) {
            size_t to_copy = (filled <= total_elements - filled)
                ? filled
                : total_elements - filled;
            memcpy(data_start + (filled * element_size), data_start,
                to_copy * element_size);
            filled += to_copy;
        }
        return;
    }
    // Sequential fill
    for (size_t i = 1; i < total_elements; i++) {
        memcpy(data_start + (i * element_size),
            data_start + ((i - 1) * element_size), element_size);
    }
}

FCLIB_API void fclib_arr_fill_val( //
    fclib_arr_t *arr,              //
    const size_t element_size,     //
    const size_t value             //
) {
    const size_t dimensionality = arr->len;
    size_t *const dim_lengths = FCLIB_ALIGNCAST(size_t, arr->value);
    size_t total_elements = 1;
    for (size_t i = 0; i < dimensionality; i++) {
        total_elements *= dim_lengths[i];
    }
    char *data_start = (char *)(dim_lengths + dimensionality);
    memcpy(data_start, &value, element_size);

    // Use exponential approach for small elements or sequential for large
    // elements
    if (element_size < 128) { // Threshold based on benchmarks
        // Exponential fill
        size_t filled = 1;
        while (filled < total_elements) {
            size_t to_copy = (filled <= total_elements - filled)
                ? filled
                : total_elements - filled;
            memcpy(data_start + (filled * element_size), data_start,
                to_copy * element_size);
            filled += to_copy;
        }
        return;
    }
    // Sequential fill
    for (size_t i = 1; i < total_elements; i++) {
        memcpy(data_start + (i * element_size),
            data_start + ((i - 1) * element_size), element_size);
    }
}

FCLIB_API char *fclib_arr_access( //
    fclib_arr_t *arr,             //
    const size_t element_size,    //
    const size_t *indices         //
) {
    const size_t dimensionality = arr->len;
    size_t *const dim_lengths = FCLIB_ALIGNCAST(size_t, arr->value);

    // Calculate the offset
    size_t offset = 0;
    size_t stride = 1; // Stride for each dimension
    for (size_t i = 0; i < dimensionality; i++) {
        size_t index = indices[i];
        if (index >= dim_lengths[i]) {
            // Out of bounds access
            return NULL;
        }
        offset += index * stride;
        // Update stride for the next dimension
        stride *= dim_lengths[i];
    }

    // Calculate the pointer to the desired element
    char *data_start =
        (char *)(dim_lengths + dimensionality); // Skip the dimension lengths
    return data_start + offset * element_size;
}

FCLIB_API size_t fclib_arr_access_val( //
    fclib_arr_t *arr,                  //
    const size_t element_size,         //
    const size_t *indices              //
) {
    char *element = fclib_arr_access(arr, element_size, indices);
    size_t value;
    memcpy(&value, element, element_size);
    return value;
}

FCLIB_API void fclib_arr_assign_at( //
    fclib_arr_t *arr,               //
    const size_t element_size,      //
    const size_t *indices,          //
    const void *value               //
) {
    char *element = fclib_arr_access(arr, element_size, indices);
    memcpy(element, value, element_size);
}

FCLIB_API void fclib_arr_assign_val_at( //
    fclib_arr_t *arr,                   //
    const size_t element_size,          //
    const size_t *indices,              //
    const size_t value                  //
) {
    char *element = fclib_arr_access(arr, element_size, indices);
    memcpy(element, &value, element_size);
}

#endif // endof FCLIB_IMPLEMENTATION
