
#ifndef cheddar_generated_imageflow_pinvoke_h
#define cheddar_generated_imageflow_pinvoke_h


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


// Incremented for breaking changes
#define IMAGEFLOW_ABI_VER_MAJOR 2

// Incremented for non-breaking additions
#define IMAGEFLOW_ABI_VER_MINOR 0




typedef signed byte int8_t;
typedef signed long int64_t;
typedef signed int int32_t;
typedef unsigned byte uint8_t;
        

///
/// What is possible with the IO object
typedef enum IoMode {
	None = 0,
	ReadSequential = 1,
	WriteSequential = 2,
	ReadSeekable = 5,
	WriteSeekable = 6,
	ReadWriteSeekable = 15,
} IoMode;

/// Input or output?
typedef enum Direction {
	Out = 8,
	In = 4,
} Direction;

///
/// How long the provided pointer/buffer will remain valid.
/// Callers must prevent the memory from being freed or moved until this contract expires.
///
typedef enum Lifetime {
	/// Pointer will outlive function call. If the host language has a garbage collector, call the appropriate method to ensure the object pointed to will not be collected or moved until the call returns. You may think host languages do this automatically in their FFI system. Most do not.
	OutlivesFunctionCall = 0,
	/// Pointer will outlive context. If the host language has a GC, ensure that you are using a data type guaranteed to neither be moved or collected automatically.
	OutlivesContext = 1,
} Lifetime;

///
/// Call this method before doing anything else to ensure that your header or FFI bindings are compatible
/// with the libimageflow that is currently loaded.
///
/// Provide the values IMAGEFLOW_ABI_VER_MAJOR and IMAGEFLOW_ABI_VER_MINOR to this function.
///
/// False means that
///
bool imageflow_abi_compatible(uint32_t imageflow_abi_ver_major, uint32_t imageflow_abi_ver_minor);

uint32_t imageflow_abi_version_major(void);

uint32_t imageflow_abi_version_minor(void);

/// Creates and returns an imageflow context.
/// An imageflow context is required for all other imageflow API calls.
///
/// An imageflow context tracks
/// * error state
/// * error messages
/// * stack traces for errors (in C land, at least)
/// * context-managed memory allocations
/// * performance profiling information
///
/// **Contexts are not thread-safe!** Once you create a context, *you* are responsible for ensuring that it is never involved in two overlapping API calls.
///
/// Returns a null pointer if allocation fails or the provided interface version is incompatible
void* imageflow_context_create(uint32_t imageflow_abi_ver_major, uint32_t imageflow_abi_ver_minor);

/// Begins the process of destroying the context, yet leaves error information intact
/// so that any errors in the tear-down process can be
/// debugged with imageflow_context_error_write_to_buffer.
///
/// Returns true if no errors occurred. Returns false if there were tear-down issues.
bool imageflow_context_begin_terminate(void* context);

/// Destroys the imageflow context and frees the context object.
/// Only use this with contexts created using imageflow_context_create
void imageflow_context_destroy(void* context);

/// Returns true if the context is in an error state. You must immediately deal with the error,
/// as subsequent API calls will fail or cause undefined behavior until the error state is cleared
bool imageflow_context_has_error(void* context);

/// Returns true if the context is "ok" or in an error state that is recoverable.
/// You must immediately deal with the error,
/// as subsequent API calls will fail or cause undefined behavior until the error state is cleared
bool imageflow_context_error_recoverable(void* context);

/// Returns true if the context is "ok" or in an error state that is recoverable.
/// You must immediately deal with the error,
/// as subsequent API calls will fail or cause undefined behavior until the error state is cleared
bool imageflow_context_error_try_clear(void* context);

/// Returns the numeric code associated with the error category. 0 means no error.
///
/// These will be stabilized after 1.0, once error categories have passed rigorous real-world testing
/// `imageflow_context_error_as_exit_code` and `imageflow_context_error_as_http_status` are suggested in the meantime.
///
int32_t imageflow_context_error_code(void* context);

/// Converts the error (or lack thereof) into an unix process exit code
///
/// ## Values
///
/// * 0 - No error
/// * 64 - Invalid usage (graph invalid, node argument invalid, action not supported)
/// * 65 - Invalid Json, Image malformed, Image type not supported
/// * 66 - Primary or secondary file or resource not found.
/// * 69 - Upstream server errored or timed out
/// * 70 - Possible bug: internal error, custom error, unknown error, or no graph solution found
/// * 71 - Out Of Memory condition (malloc/calloc/realloc failed).
/// * 74 - I/O Error
/// * 77 - Action forbidden under imageflow security policy
/// * 402 - License error
/// * 401 - Imageflow server authorization required
///
int32_t imageflow_context_error_as_exit_code(void* context);

/// Converts the error (or lack thereof) into an equivalent http status code
///
/// ## Values
///
/// * 200 - No error
/// * 400 - Bad argument/node parameters/graph/json/image/image type
/// * 401 - Authorization to imageflow server required
/// * 402 - License error
/// * 403 - Action forbidden under imageflow security policy
/// * 404 - Primary resource/file not found
/// * 500 - Secondary resource/file not found, IO error, no solution error, unknown error, custom error, internal error
/// * 502 - Upstream server error
/// * 503 - Out Of Memory condition (malloc/calloc/realloc failed).
/// * 504 - Upstream timeout
int32_t imageflow_context_error_as_http_code(void* context);

/// Prints the error messages (and optional stack frames) to the given buffer in UTF-8 form; writes a null
/// character to terminate the string, and *ALSO* provides the number of bytes written (excluding the null terminator)
///
/// Returns false if the buffer was too small (or null) and the output was truncated.
/// Returns true if all data was written OR if there was a bug in error serialization (that gets written, too).
///
/// If the data is truncated, "\n[truncated]\n" is written to the buffer
///
/// Please be accurate with the buffer length, or a buffer overflow will occur.
bool imageflow_context_error_write_to_buffer(void* context, char* buffer, size_t buffer_length, size_t* bytes_written);

/// Prints the error to stderr and exits the process if an error has been raised on the context.
/// If no error is present, the function returns false.
///
/// THIS PRINTS DIRECTLY TO STDERR! Do not use in any kind of service! Command-line usage only!
bool imageflow_context_print_and_exit_if_error(void* context);

///
/// Writes fields from the given imageflow_json_response to the locations referenced.
/// The buffer pointer sent out will be a UTF-8 byte array of the given length (not null-terminated). It will
/// also become invalid if the void associated is freed, or if the context is destroyed.
///
/// See `imageflow_context_error_as_http_code` for just the http status code equivalent.
///
/// Most errors are not recoverable; you must destroy the context and retry.
///
bool imageflow_json_response_read(void* context, void const* response_in, int64_t* status_as_http_code_out, uint8_t const** buffer_utf8_no_nulls_out, size_t* buffer_size_out);

/// Frees memory associated with the given object (and owned objects) after
/// running any owned or attached destructors. Returns false if something went wrong during tear-down.
///
/// Returns true if the object to destroy is a null pointer, or if tear-down was successful.
///
/// Behavior is undefined if the pointer is dangling or not a valid memory reference.
/// Although certain implementations catch
/// some kinds of invalid pointers, a segfault is likely in future revisions).
///
/// Behavior is undefined if the context provided does not match the context with which the
/// object was created.
///
bool imageflow_json_response_destroy(void* context, void* response);

///
/// Sends a JSON message to the imageflow_context using endpoint `method`.
///
/// ## Endpoints
///
/// * 'v0.1/build`
///
/// For endpoints supported by the latest nightly build, see
/// https://s3-us-west-1.amazonaws.com/imageflow-nightlies/master/doc/context_json_api.txt
///
/// ## Notes
///
/// * `method` and `json_buffer` are only borrowed for the duration of the function call. You are
///    responsible for their cleanup (if necessary - static strings are handy for things like
///    `method`).
/// * `method` should be a UTF-8 null-terminated string.
///   `json_buffer` should be a UTF-8 encoded buffer (not null terminated) of length json_buffer_size.
///
/// You should call `imageflow_context_has_error()` to see if this succeeded.
///
/// A void is returned for success and most error conditions.
/// Call `imageflow_json_response_destroy` when you're done with it (or dispose the context).
void const* imageflow_context_send_json(void* context, char const* method, uint8_t const* json_buffer, size_t json_buffer_size);

///
/// Creates an imageflow_io object to wrap a filename.
///
/// The filename should be a null-terminated string. It should be written in codepage used by your operating system for handling `fopen` calls.
/// https://msdn.microsoft.com/en-us/library/yeby3zcb.aspx
///
/// If the filename is fopen compatible, you're probably OK.
///
/// As always, `mode` is not enforced except for the file open flags.
///
bool imageflow_context_add_file(void* context, int32_t io_id, Direction direction, IoMode mode, char const* filename);

///
/// Creates an imageflow_io structure for reading from the provided buffer.
/// You are ALWAYS responsible for freeing the memory provided (at the time specified by Lifetime).
/// If you specify OutlivesFunctionCall, then the buffer will be copied.
///
///
bool imageflow_context_add_input_buffer(void* context, int32_t io_id, uint8_t const* buffer, size_t buffer_byte_count, Lifetime lifetime);

///
/// Creates an imageflow_io structure for writing to an expanding memory buffer.
///
/// Reads/seeks, are, in theory, supported, but unless you've written, there will be nothing to read.
///
/// The I/O structure and buffer will be freed with the context.
///
///
/// Returns null if allocation failed; check the context for error details.
bool imageflow_context_add_output_buffer(void* context, int32_t io_id);

///
/// Provides access to the underlying buffer for the given imageflow_io object.
///
bool imageflow_context_get_output_buffer_by_id(void* context, int32_t io_id, uint8_t const** result_buffer, size_t* result_buffer_length);

///
/// Allocates zeroed memory that will be freed with the context.
///
/// * filename/line may be used for debugging purposes. They are optional. Provide null/-1 to skip.
/// * If provided, `filename` should be an null-terminated UTF-8 or ASCII string which will outlive the context.
///
/// Returns null(0) on failure.
///
void* imageflow_context_memory_allocate(void* context, size_t bytes, char const* filename, int32_t line);

///
/// Frees memory allocated with imageflow_context_memory_allocate early.
///
/// * filename/line may be used for debugging purposes. They are optional. Provide null/-1 to skip.
/// * If provided, `filename` should be an null-terminated UTF-8 or ASCII string which will outlive the context.
///
/// Returns false on failure. Returns true on success, or if `pointer` is null.
///
bool imageflow_context_memory_free(void* context, void* pointer, char const* filename, int32_t line);



#ifdef __cplusplus
}
#endif


#endif
