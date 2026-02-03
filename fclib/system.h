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

#ifdef __WIN32__
#include <direct.h>
#define PATH_MAX 260
#else
#include <unistd.h>
#ifndef PATH_MAX
#include <limits.h>
#endif
#ifndef PATH_MAX
#define PATH_MAX 260
#endif
#endif

#include "str.h"

#include <stdio.h>

// C99 and later does not support implicit function declarations, so well here
// are popen and pclose if we are on linux
#ifndef __WIN32__
FILE *popen(const char *command, const char *type);
int pclose(FILE *stream);
#endif
int fileno(FILE *stream);

/// @typedef `command_result_t`
/// @brief The return value of the `system_command` function
typedef struct fclib_command_result_t {
    int exit_code;
    fclib_str_t *output;
} fclib_command_result_t;

/// @function `system_command`
/// @brief Executes the given system command, captures all output and returns
/// the output of the system command together with the exit code
///
/// @param `command` The command to execute
/// @return `command_result_t` The result of the system command containing the
/// exit code of the executed command and the output of the stdout file stream
/// of the executed command stored in a string
FCLIB_API fclib_command_result_t fclib_system_command( //
    fclib_str_t *const command                         //
);

/// @function `system_get_cwd`
/// @brief Returns the path to the curent working directory
///
/// @return `str_t *` A string containing the path to the current working
/// directory
FCLIB_API fclib_str_t *fclib_system_get_cwd(void);

/// @function `system_get_path_linux`
/// @brief Converts the given `path` to a proper linux-path by escaping spaces,
/// converting backslashes to slashes etc. This function is used to normalize
/// paths depending on platform
///
/// @param `path` The string which needs to be converted to a linux path
/// @return `str_t *` The converted proper linux path
FCLIB_API fclib_str_t *fclib_system_get_path_linux(const fclib_str_t *path);

/// @function `system_get_path_windows`
/// @brief Converts the given `path` to a proper windows-path by converting
/// slashes to backslashes etc. This function is used to normalize paths
/// depending on platform
///
/// @param `path` The string which needs to be converted to a linux path
/// @return `str_t *` The converted proper windows path
FCLIB_API fclib_str_t *fclib_system_get_path_windows(const fclib_str_t *path);

/// @function `system_get_path`
/// @brief Returns the path normalized for the platform, so on Windows all `/`
/// are replaced with `\\` and on Linux all `\\` which are not followed by a
/// space are replaced with `/`. If the path contains spaces then the whole path
/// is quoted using `"` symbols when on Windows. On linux, all spaces are
/// escaped.
///
/// @param `path` The string contianing the path to convert
/// @param `is_linux` Whether the given path should be in linux or
/// windows-format
/// @return `str_t *` The converted proper path
///
/// @attention: The `is_linux` argument is *NOT* part of the actual function
/// signature, it is only added to this function to make it debuggable on both
/// Windows and Linux and to make the behaviour of both targets prototypable.
FCLIB_API fclib_str_t *fclib_system_get_path( //
    const fclib_str_t *path,                  //
    const bool is_linux                       //
);

/// @function `system_start_capture`
/// @brief Starts capturing all output to stdout and stderr. Nothing will be
/// printed to the console until `system_end_capture` is called.
FCLIB_API void fclib_system_start_capture(void);

/// @function `system_end_capture`
/// @brief Stops capturing output, restores stdout/stderr, and returns the
/// unified captured content (stdout + stderr interleaved).
///
/// @return `str_t *` The captured output as a string
FCLIB_API fclib_str_t *fclib_system_end_capture(void);

// #define FCLIB_STRIP_PREFIXES // Uncomment for debugging purposes
#ifdef FCLIB_STRIP_PREFIXES

typedef fclib_command_result_t command_result_t;

FCLIB_API static inline command_result_t system_command( //
    fclib_str_t *const command                           //
) {
    return fclib_system_command(command);
}

FCLIB_API static inline fclib_str_t *system_get_cwd(void) {
    return fclib_system_get_cwd();
}

FCLIB_API static inline fclib_str_t *system_get_path_linux( //
    const fclib_str_t *path                                 //
) {
    return fclib_system_get_path_linux(path);
}

FCLIB_API static inline fclib_str_t *system_get_path_windows( //
    const fclib_str_t *path                                   //
) {
    return fclib_system_get_path_windows(path);
}

FCLIB_API fclib_str_t *system_get_path( //
    const fclib_str_t *path,            //
    const bool is_linux                 //
) {
    return fclib_system_get_path(path, is_linux);
}

FCLIB_API void system_start_capture(void) {
    fclib_system_start_capture();
}

FCLIB_API fclib_str_t *system_end_capture(void) {
    return fclib_system_end_capture();
}

#endif // endof FCLIB_STRIP_PREFIXES

#ifdef __cplusplus
}
#endif

// #define FCLIB_IMPLEMENTATION // Uncomment for debugging purposes
#ifdef FCLIB_IMPLEMENTATION

FCLIB_API fclib_command_result_t fclib_system_command( //
    fclib_str_t *const command                         //
) {
    fclib_command_result_t result = {-1, NULL};
    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    // Allocate initial output buffer to be an empty string
    result.output = fclib_str_create(0);

    if (command->len == 0) {
        // ErrSystem.EmptyCommand
        free(result.output);
        result.output = NULL;
        return result;
    }

    // Replace all '/' characters with '\\' ones on Windows
    fclib_str_t *command_to_use = command;
#ifdef __WIN32__
    fclib_str_t *command_cpy = fclib_str_init(command->value, command->len);
    size_t idx = 0;
    do {
        char curr_char = command_cpy->value[idx];
        if (curr_char == '/') {
            command_cpy->value[idx] = '\\';
        }
        idx++;
        if (idx >= command_cpy->len) {
            break;
        }
    } while (command_cpy->value[idx] != ' ');
#endif

    // Create command with stderr redirection
    fclib_str_t *full_command = ADD_STR_LIT(command_to_use, " 2>&1");
    char *c_command = (char *)full_command->value;
#ifdef __WIN32__
    FILE *pipe = _popen(c_command, "r");
#else
    FILE *pipe = popen(c_command, "r");
#endif
    free(full_command);
    if (!pipe) {
        // ErrSystem.SpawnFailed
        free(result.output);
        result.output = NULL;
        return result;
    }

    // Read output from pipe
    while (fgets(buffer, BUFFER_SIZE, pipe) != NULL) {
        // Append buffer to output
        int buffer_len = strlen(buffer);
        fclib_str_append_lit(&result.output, buffer, buffer_len);
    }

    // Get command exit status
#ifdef __WIN32__
    int status = _pclose(pipe);
#else
    int status = pclose(pipe);
#endif
    result.exit_code = status & 0xFF;

    return result;
}

FCLIB_API fclib_str_t *fclib_system_get_cwd(void) {
    // Maximum path length (PATH_MAX is POSIX, MAX_PATH is Windows)
    char buffer[PATH_MAX];
#ifdef __WIN32__
    if (_getcwd(buffer, sizeof(buffer)) == NULL) {
#else
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
#endif
        // Could not get cwd, return empty string or handle error as needed
        return fclib_str_create(0);
    }
    return fclib_str_init(buffer, strlen(buffer));
}

FCLIB_API fclib_str_t *fclib_system_get_path_linux(const fclib_str_t *path) {
    const size_t BUFFER_SIZE = 256;
    char buffer[BUFFER_SIZE];
    size_t buffer_len = 0;
    const size_t path_len = path->len;
    if (path_len >= BUFFER_SIZE) {
        return fclib_str_create(0);
    }

    for (size_t i = 0; i < path_len; i++) {
        char ci = path->value[i];
        if (ci != '\\') {
            buffer[buffer_len++] = ci;
            continue;
        }
        // Check if the next character is a space, if not change the path to be
        // a '/'
        if (i + 1 == path_len || path->value[i + 1] != ' ') {
            buffer[buffer_len++] = '/';
        } else {
            buffer[buffer_len++] = '\\';
        }
    }

    return fclib_str_init(buffer, buffer_len);
}

FCLIB_API fclib_str_t *fclib_system_get_path_windows(const fclib_str_t *path) {
    const size_t BUFFER_SIZE = 256;
    char buffer[BUFFER_SIZE];
    size_t buffer_len = 0;
    const size_t path_len = path->len;
    if (path_len >= BUFFER_SIZE) {
        return fclib_str_create(0);
    }

    bool path_contains_space = false;
    for (size_t i = 0; i < path_len; i++) {
        char ci = path->value[i];
        if (ci == '\\' && i + 1 < path_len && path->value[i + 1] == ' ') {
            // If the next character is a space, replace the two characters with
            // a single space and set the 'path_contains_space' value to true
            buffer[buffer_len++] = ' ';
            i++;
            path_contains_space = true;
            continue;
        }
        if (ci == '/') {
            buffer[buffer_len++] = '\\';
            continue;
        } else if (ci == ' ') {
            path_contains_space = true;
        }
        buffer[buffer_len++] = ci;
    }
    if (path_contains_space) {
        if (buffer_len + 2 >= BUFFER_SIZE) {
            return fclib_str_create(0);
        }
        memmove(buffer + 1, buffer, buffer_len);
        buffer[0] = '"';
        buffer[buffer_len + 1] = '"';
        buffer_len += 2;
    }
    return fclib_str_init(buffer, buffer_len);
}

FCLIB_API fclib_str_t *fclib_system_get_path( //
    const fclib_str_t *path,                  //
    const bool is_linux                       //
) {
    if (is_linux) {
        return fclib_system_get_path_linux(path);
    } else {
        return fclib_system_get_path_windows(path);
    }
}

// Globals to track capture state (static to avoid external linkage)
static int orig_stdout_fd = -1;
static int orig_stderr_fd = -1;
static FILE *capture_file = NULL;

FCLIB_API void fclib_system_start_capture(void) {
    if (capture_file != NULL) {
        // Already capturing; ignore or handle error
        return;
    }

    // Flush existing buffers
    fflush(stdout);
    fflush(stderr);

    // Save original fds
    orig_stdout_fd = dup(fileno(stdout));
    orig_stderr_fd = dup(fileno(stderr));

    // Create temp file for capture
    capture_file = tmpfile();
    if (capture_file == NULL) {
        // Handle error (e.g., perror("tmpfile")); for now, abort capture
        orig_stdout_fd = -1;
        orig_stderr_fd = -1;
        return;
    }

    // Redirect stdout to capture file
    dup2(fileno(capture_file), fileno(stdout));

    // Redirect stderr to stdout (unifies and preserves order)
    dup2(fileno(stdout), fileno(stderr));
}

FCLIB_API fclib_str_t *fclib_system_end_capture(void) {
    if (capture_file == NULL) {
        // Not capturing; return empty string
        return fclib_str_create(0);
    }

    // Flush buffers to ensure all output is in the file
    fflush(stdout);
    fflush(stderr);

    // Restore original fds
    dup2(orig_stdout_fd, fileno(stdout));
    dup2(orig_stderr_fd, fileno(stderr));
    close(orig_stdout_fd);
    close(orig_stderr_fd);
    orig_stdout_fd = -1;
    orig_stderr_fd = -1;

    // Rewind and read the capture file
    rewind(capture_file);
    fclib_str_t *captured = fclib_str_create(0);
    char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), capture_file)) > 0) {
        fclib_str_append_lit(&captured, buffer, bytes_read);
    }

    // Clean up
    fclose(capture_file);
    capture_file = NULL;

    return captured;
}

#endif // endof FCLIB_IMPLEMENTATION
