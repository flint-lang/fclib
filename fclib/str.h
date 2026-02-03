#pragma once

// Define a header-local implementation macro to prevent inlining the
// implementation of potentially cross-included fclib headers
#ifdef FCLIB_IMPLEMENTATION
#define FCLIB_STR_IMPLEMENTATION
#undef FCLIB_IMPLEMENTATION
#endif

#ifndef FCLIB_API
#define FCLIB_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/// @typedef `str_t`
/// @brief A string is a simple heap-allocated structure which consists of the
/// length of the stirng and it's data in a variable member pattern. The `str_t`
/// type is also used for arrays, the same type is re-used
typedef struct fclib_str_t {
    size_t len;
    char value[];
} fclib_str_t;

/// @function `str_create`
/// @brief Creates a new string with the given length. Each created string holds
/// one element more than the length specified, this way the `string->value`
/// field can be simply used as a `char*` for any other C function which expects
/// a "normal" c string
///
/// @param `len` The length of the string to create
/// @return `str_t *` A pointer to the newly created string with the given len
FCLIB_API fclib_str_t *fclib_str_create(const size_t len);

/// @function `str_init`
/// @brief Initializes a given string by creating a new string of size `len` and
/// copying the data at `value` over to it. This function expects that `value`
/// holds enough data for `len`. If `value` is smaller than `len` then you are
/// dealing with UB and are on your own, so be sure that the value actually
/// contains as much as you want to extract from it.
///
/// @param `value` The value to initialize the string from
/// @param `len` The length of the new string to initialize
/// @return `str_t *` A pointer to the initialized string
FCLIB_API fclib_str_t *fclib_str_init(const char *value, const size_t len);

/// @function `str_assign`
/// @brief Assigns the given `value` to the given `string` by first freeing
/// whatever is stored in the `string` before assigning the new value to it.
/// Strings can be freed very easy since the actual data is stored as a variable
/// member.
///
/// @param `string` The string variable to assign the value to
/// @param `value` The value to assign to the given string variable
FCLIB_API void fclib_str_assign(fclib_str_t **string, fclib_str_t *value);

/// @function `str_assign_lit`
/// @brief Assigns the given `value` of the given `len` to the `string`
/// variable. This internally frees the old string and calls `str_init` to
/// initialize a new string from the given value with the given length and then
/// stores that string on the given variable.
///
/// @param `string` The variable to assign the newly created string to
/// @param `value` The value from which to initialize the new string
/// @param `len` The length of the newly initialized string
FCLIB_API void fclib_str_assign_lit( //
    fclib_str_t **string,            //
    const char *value,               //
    const size_t len                 //
);

/// @function `str_compare`
/// @brief Compares two strings and returns which one is lexigraphically the
/// first one.
///
/// @param `lhs` The string on the left side of the comparison
/// @param `rhs` The string on the right side of the comparison
/// @return `int32_t` The comparison result of the two strings:
///     -1: The lhs is "smaller" than the rhs, for example lhs="ab" and rhs="ba"
///      0: The lhs and rhs are equal both in their lengths and their content
///      1: The lhs is "larger" than the rhs, for example lhs="ba" and rhs="ab"
FCLIB_API int32_t fclib_str_compare( //
    const fclib_str_t *lhs,          //
    const fclib_str_t *rhs           //
);

/// @function `str_equals`
/// @brief Compares to strings and returns whether they are equal both in size
/// and content
///
/// @param `lhs` The string on the left side of the comparison
/// @param `rhs` The string on the right side of the comparison
/// @return `bool` Whether the strings are equal in their size and content
FCLIB_API bool fclib_str_equals(const fclib_str_t *lhs, const fclib_str_t *rhs);

/// @function `str_append`
/// @brief Appends the `source` string to the `dest` string, the `dest` needs to
/// grow for this to happen.
///
/// @param `dest` The pointer to the variable holding the string to append to
/// @param `source` The pointer to the string to append to the `dest`
FCLIB_API void fclib_str_append(fclib_str_t **dest, const fclib_str_t *source);

/// @function `str_append_lit`
/// @brief Appends the `source` string of length `source_len` to the `dest`
/// string, the `dest` needs to grow for this to happen. This function expects
/// that the `source` contains at least `source_len` data, but this function
/// does not check if it actually contains so much data. It is on you to make
/// sure that `source` contains at least `source_len` on data.
///
/// @param `dest` The pointer to the variable holding the string to append to
/// @param `source` The pointer to the data from which to appen `source_len`
/// characters to the `dest` string
/// @param `source_len` The amount of characters to append from `source` to
/// `dest`
FCLIB_API void fclib_str_append_lit( //
    fclib_str_t **dest,              //
    const char *source,              //
    const size_t source_len          //
);

/// @function `str_add`
/// @brief Adds two strings and returns a newly created string from that
/// addition
///
/// @param `lhs` The left side of the addition
/// @param `rhs` The right side of the addition
/// @return `str_t *` The newly created string containing both the `lhs` and
/// `rhs`
FCLIB_API fclib_str_t *fclib_str_add( //
    const fclib_str_t *lhs,           //
    const fclib_str_t *rhs            //
);

/// @function `str_add_lit_str`
/// @brief Prepends the data `lhs` of size `lhs_len` to the `rhs` string and
/// returns the addition as a new string.
///
/// @param `lhs` The data which is prepended to the `rhs` string
/// @param `lhs_len` The length of the data to prepend to the `rhs` string
/// @param `rhs` The string the `lhs` data of size `lhs_len` is prepended at
/// @return `str_t *` The addition of the `lhs` data and the `rhs` string
FCLIB_API fclib_str_t *fclib_str_add_lit_str( //
    const char *lhs,                          //
    const size_t lhs_len,                     //
    const fclib_str_t *rhs                    //
);

/// @function `str_add_str_lit`
/// @brief Addpends the data `rhs` of size `rhs_len` to the `lhs` string and
/// returns the addition as a new string.
///
/// @param `lhs` The string the `rhs` data of size `rhs_len` is appended at
/// @param `rhs` The data which is appended to the `lhs` string
/// @param `rhs_len` The length of the data to append at the `lhs` string
/// @return `str_t *` The addition of the `lhs` string and the `rhs` data
FCLIB_API fclib_str_t *fclib_str_add_str_lit( //
    const fclib_str_t *lhs,                   //
    const char *rhs,                          //
    const size_t rhs_len                      //
);

/// @function `str_get_slice`
/// @brief Returns a slice of the given `src` string from index [`from`..`to`)
///
/// @param `src` The source string to create a slice from
/// @param `from` The inclusive index at which the slicing begins
/// @param `to` The exclusive index at which the slicing ends
/// @return `str_t *` A new string created from the data [`from`..`to`) of the
/// `src` string
FCLIB_API fclib_str_t *fclib_str_get_slice( //
    const fclib_str_t *src,                 //
    const size_t from,                      //
    const size_t to                         //
);

/// @macro `STR_DECL`
/// @brief This macro is used to quickly declare a variable of name `var` with
/// the string literal `rhs`. It is very handy because often times you want to
/// initialize strings from literals
///
/// @param `var` The name of the variable to declare
/// @param `rhs` The content to store in the string variable
///
/// @example You can easily write `STR_DECL(string, "Hello, World!");` which
/// counts the characters in the provided string for you and calls the
/// `str_init` function to initialize a new string which contains *exactly* the
/// provided literal.
#define STR_DECL(var, rhs) str *var = fclib_str_init(rhs, strlen(rhs))

/// @macro `STR_ASSIGN`
/// @brief This macro is used to assign a `rhs` value to a variable `var`. The
/// `rhs` can either be a string itself or a literal.
///
/// @param `var` The name of the variable to assign to
/// @param `rhs` The value to assign to the variable, could be both a literal or
/// a different string variable
///
/// @example
///     - Assigns the string "Hello, World" to the variable `string`:
///         STR_ASSIGN(string, "Hello, World!");
///
///     - Assigns the string stored in the variable `other_string` to the
///       variable `string`:
///         STR_ASSIGN(string, other_string);
#define STR_ASSIGN(var, rhs)                                                   \
    do {                                                                       \
        __typeof__(rhs) _temp_rhs = (rhs);                                     \
        if (__builtin_types_compatible_p(__typeof__(_temp_rhs), str *) ||      \
            __builtin_types_compatible_p(__typeof__(_temp_rhs),                \
                const str *)) {                                                \
            fclib_str_assign(&(var), (str *)_temp_rhs);                        \
        } else {                                                               \
            fclib_str_assign_lit(&(var), (const char *)_temp_rhs,              \
                strlen((const char *)_temp_rhs));                              \
        }                                                                      \
    } while (0)

/// @macro `ADD_STR_LIT`
/// @brief Simple wrapper around the `str_add_str_lit` function which simply
/// calls `strlen` on the provided literal to add to the string variable
///
/// @param `lhs` The string variable to append the `rhs` literal to
/// @param `rhs` The literal to append to the `lhs` string variable
///
/// @example
///     - Adds the strings `hello` and ", World!" together:
///         ADD_STR_LIT(hello, ", World!");
#define ADD_STR_LIT(lhs, rhs) fclib_str_add_str_lit(lhs, rhs, strlen(rhs))

/// @macro `ADD_LIT_STR`
/// @brief Simple wrapper around the `str_add_lit_str` function which simply
/// calls `strlen` on the provided literal to prepend to the string variable
///
/// @param `lhs` The literal to prepend to the `rhs` string variable
/// @param `rhs` The string variable to prepend the `lhs` literal to
///
/// @example
///     - Adds the strings "Hello" and whatever is stored in `world` together:
///         ADD_LIT_STR("Hello", world);
#define ADD_LIT_STR(lhs, rhs) fclib_str_add_lit_str(lhs, strlen(lhs), rhs)

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
typedef fclib_str_t str_t;

FCLIB_API static inline str_t *str_create(const size_t len) {
    return fclib_str_create(len);
}
FCLIB_API static inline str_t *str_init(const char *value, const size_t len) {
    return fclib_str_init(value, len);
}
FCLIB_API static inline void str_assign(str_t **string, str_t *value) {
    fclib_str_assign(string, value);
}
FCLIB_API static inline void str_assign_lit( //
    str_t **string,                          //
    const char *value,                       //
    const size_t len                         //
) {
    fclib_str_assign_lit(string, value, len);
}
FCLIB_API static inline int32_t str_compare( //
    const str_t *lhs,                        //
    const str_t *rhs                         //
) {
    return fclib_str_compare(lhs, rhs);
}
FCLIB_API static inline bool str_equals(const str_t *lhs, const str_t *rhs) {
    return fclib_str_equals(lhs, rhs);
}
FCLIB_API static inline void str_append(str_t **dest, const str_t *source) {
    fclib_str_append(dest, source);
}
FCLIB_API static inline void str_append_lit( //
    str_t **dest,                            //
    const char *source,                      //
    const size_t source_len                  //
) {
    fclib_str_append_lit(dest, source, source_len);
}
FCLIB_API static inline str_t *str_add(const str_t *lhs, const str_t *rhs) {
    return fclib_str_add(lhs, rhs);
}
FCLIB_API static inline str_t *str_add_lit_str( //
    const char *lhs,                            //
    const size_t lhs_len,                       //
    const str_t *rhs                            //
) {
    return fclib_str_add_lit_str(lhs, lhs_len, rhs);
}
FCLIB_API static inline str_t *str_add_str_lit( //
    const str_t *lhs,                           //
    const char *rhs,                            //
    const size_t rhs_len                        //
) {
    return fclib_str_add_str_lit(lhs, rhs, rhs_len);
}
FCLIB_API static inline str_t *str_get_slice( //
    const str_t *src,                         //
    const size_t from,                        //
    const size_t to                           //
) {
    return fclib_str_get_slice(src, from, to);
}
#endif // endof FCLIB_STRIP_PREFIXES

#ifdef __cplusplus
}
#endif

// #define FCLIB_STR_IMPLEMENTATION // Uncomment for debugging purposes
#ifdef FCLIB_STR_IMPLEMENTATION

FCLIB_API fclib_str_t *fclib_str_create(const size_t len) {
    fclib_str_t *string = (fclib_str_t *)malloc(sizeof(fclib_str_t) + len + 1);
    string->len = len;
    string->value[len] = 0;
    return string;
}

FCLIB_API fclib_str_t *fclib_str_init(const char *value, const size_t len) {
    fclib_str_t *string = fclib_str_create(len);
    memcpy(string->value, value, len);
    return string;
}

FCLIB_API void fclib_str_assign(fclib_str_t **string, fclib_str_t *value) {
    free(*string);
    *string = value;
}

FCLIB_API void fclib_str_assign_lit( //
    fclib_str_t **string,            //
    const char *value,               //
    const size_t len                 //
) {
    fclib_str_t *new_string =
        (fclib_str_t *)realloc(*string, sizeof(fclib_str_t) + len + 1);
    *string = new_string;

    new_string->len = len;
    memcpy(new_string->value, value, len);
    new_string->value[len] = 0;
}

FCLIB_API int32_t fclib_str_compare( //
    const fclib_str_t *lhs,          //
    const fclib_str_t *rhs           //
) {
    if (lhs->len < rhs->len) {
        return -1;
    } else if (lhs->len > rhs->len) {
        return 1;
    }

    return memcmp(lhs->value, rhs->value, lhs->len);
}

FCLIB_API bool fclib_str_equals( //
    const fclib_str_t *lhs,      //
    const fclib_str_t *rhs       //
) {
    return fclib_str_compare(lhs, rhs) == 0;
}

FCLIB_API void fclib_str_append(fclib_str_t **dest, const fclib_str_t *source) {
    fclib_str_t *new_dest = (fclib_str_t *)realloc(*dest,
        sizeof(fclib_str_t) + (*dest)->len + source->len + 1);
    *dest = new_dest;

    memcpy(new_dest->value + new_dest->len, source->value, source->len);
    size_t new_len = new_dest->len + source->len;
    new_dest->len = new_len;
    new_dest->value[new_len] = 0;
}

FCLIB_API void fclib_str_append_lit( //
    fclib_str_t **dest,              //
    const char *source,              //
    const size_t source_len          //
) {
    fclib_str_t *new_dest = (fclib_str_t *)realloc(*dest,
        sizeof(fclib_str_t) + (*dest)->len + source_len + 1);
    *dest = new_dest;

    memcpy(new_dest->value + new_dest->len, source, source_len);
    size_t new_len = new_dest->len + source_len;
    new_dest->len = new_len;
    new_dest->value[new_len] = 0;
}

FCLIB_API fclib_str_t *fclib_str_add( //
    const fclib_str_t *lhs,           //
    const fclib_str_t *rhs            //
) {
    fclib_str_t *result = fclib_str_create(lhs->len + rhs->len);
    memcpy(result->value, lhs->value, lhs->len);
    memcpy(result->value + lhs->len, rhs->value, rhs->len);
    return result;
}

FCLIB_API fclib_str_t *fclib_str_add_lit_str( //
    const char *lhs,                          //
    const size_t lhs_len,                     //
    const fclib_str_t *rhs                    //
) {
    fclib_str_t *result = fclib_str_create(lhs_len + rhs->len);
    memcpy(result->value, lhs, lhs_len);
    memcpy(result->value + lhs_len, rhs->value, rhs->len);
    return result;
}

FCLIB_API fclib_str_t *fclib_str_add_str_lit( //
    const fclib_str_t *lhs,                   //
    const char *rhs,                          //
    const size_t rhs_len                      //
) {
    fclib_str_t *result = fclib_str_create(lhs->len + rhs_len);
    memcpy(result->value, lhs->value, lhs->len);
    memcpy(result->value + lhs->len, rhs, rhs_len);
    return result;
}

FCLIB_API fclib_str_t *fclib_str_get_slice( //
    const fclib_str_t *src,                 //
    const size_t from,                      //
    const size_t to                         //
) {
    size_t real_to = to == 0 ? src->len : to;
    if (real_to > src->len) {
        // Print error to tell a oob-slicing attempt was done and clamp to the
        // src len Because slicing is technically an array operation, the array
        // OOB options apply here So this could be a hard crash, silent, unsafe
        // or verbose
        real_to = src->len;
    }
    if (from == real_to) {
        // The range does not contain a single element, which means we simply
        // return an empty string
        return fclib_str_create(0);
    }
    size_t real_from = from;
    if (from > real_to) {
        // The lower bound of the range was above or equal to the upper bound of
        // the range The from value will get clamped to the value (real_to - 1)
        // As above, the OOB options apply to whether a print, crash etc is done
        // in here
        real_from = real_to - 1;
    }
    const size_t len = real_to - real_from;
    return fclib_str_init(src->value, len);
}

#endif // endof FCLIB_STR_IMPLEMENTATION
