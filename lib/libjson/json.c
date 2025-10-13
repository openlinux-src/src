#include "json.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

inline struct json_value *json_array_get(struct json_value *array, int index)
{
	return index < array->array.length ? array->array.items[index] : NULL;
}

/**
 * @brief Represents a JSON parser for decoding JSON strings.
 *
 * This structure is used internally by the JSON library to parse
 * JSON-encoded strings into structured `json_value` objects.
 *
 * @note This structure is private and should not be accessed directly
 *       outside of the JSON library's internal implementation.
 */
struct json_parser {
	/**
     * @brief The input JSON string to be parsed.
     *
     * This is a pointer to the null-terminated JSON string that the parser
     * operates on.
     */
	const char *input;

	/**
     * @brief The length of the input JSON string.
     *
     * This value represents the total number of characters in the input
     * JSON string, excluding the null terminator.
     */
	int length;

	/**
     * @brief The current position in the input JSON string.
     *
     * This value indicates the index of the character currently being
     * processed by the parser.
     */
	int position;

#if defined(JSON_ERROR)
	/**
     * @brief Represents error information for the JSON parser.
     *
     * This structure is used to store details about any errors encountered
     * during the parsing process.
     */
	struct {
		/**
         * @brief Enum representing the type of error encountered.
         *
         * - `JSON_ERROR_NONE`: No error occurred.
         * - `JSON_ERROR_SYNTAX`: A syntax error was encountered.
         * - `JSON_ERROR_MEMORY`: A memory allocation error occurred.
         * - `JSON_ERROR_EOF`: Unexpected end of input was encountered.
         */
		enum {
			JSON_ERROR_NONE, /**< No error occurred. */
			JSON_ERROR_SYNTAX, /**< A syntax error was encountered. */
			JSON_ERROR_MEMORY, /**< A memory allocation error occurred. */
			JSON_ERROR_EOF, /**< Unexpected end of input was encountered. */
			JSON_ERROR_INVALID_NUMBER, /**< Failed to parse number */
		} code;

		/**
         * @brief The line number where the error occurred.
         *
         * This value is used for debugging purposes to identify the location
         * of the error in the source code.
         */
		int line;

		/**
         * @brief The name of the function where the error occurred.
         *
         * This is a pointer to a string containing the name of the function
         * in which the error was detected.
         */
		const char *func;

		/**
         * @brief A descriptive message about the error.
         *
         * This is a pointer to a string containing a human-readable
         * description of the error.
         */
		const char *message;
	} error;
#endif
};

/**
 * Initializes a JSON object.
 *
 * This function is responsible for setting up the internal structure
 * of a JSON object, ensuring it is ready for use. It is invoked by
 * both json_object_new() and json_object_free() to manage the lifecycle
 * of JSON objects.
 *
 * @param object A pointer to the JSON object to be initialized.
 *
 * @note This function is marked as privative and should not be used
 *       directly outside of the JSON library's internal implementation.
 */
void json_object_init(struct json_value *object);

#if defined(JSON_ERROR)
/**
 * @brief Sets an error in the JSON parser.
 *
 * This macro assigns error details to the JSON parser's error structure.
 * It captures the error code, the line number, the function name, and
 * a descriptive error message.
 *
 * @param PARSER The JSON parser where the error occurred.
 * @param CODE The error code to set.
 * @param MESSAGE A descriptive message about the error.
 */
#define JSON_PARSER_ERROR(PARSER, CODE, MESSAGE) \
	(PARSER)->error.code = (CODE);           \
	(PARSER)->error.line = __LINE__;         \
	(PARSER)->error.func = __func__;         \
	(PARSER)->error.message = (MESSAGE)
#else
/**
 * @brief No-op macro for JSON parser errors.
 *
 * When JSON_ERROR is not defined, this macro does nothing.
 *
 * @param PARSER The JSON parser (unused).
 * @param CODE The error code (unused).
 * @param MESSAGE The error message (unused).
 */
#define JSON_PARSER_ERROR(PARSER, CODE, MESSAGE)
#endif

static int json__decode_value(struct json_parser *parser,
			      struct json_value *value);

static int json__strlen(const char *str)
{
	int len = 0;
	while (*str++)
		len++;
	return len;
}

static int json__streq(const char *s1, const char *s2)
{
	if (json__strlen(s1) != json__strlen(s2))
		return 0;

	while (*s1 && *s1 == *s2) {
		s1++;
		s2++;
	}

	if (*s1 == 0 && *s2 == 0)
		return 1;

	return 0;
}

static int json__streqn(const char *s1, const char *s2, int limit)
{
	while (*s1 && *s2 && *s1 == *s2 && limit-- > 1) {
		s1++;
		s2++;
	}

	return *s1 == *s2 ? 1 : 0;
}

static void json__parse_whitespace(struct json_parser *parser)
{
	const char *ptr = parser->input;
	int pos = parser->position;
	int len = parser->length;

	while (pos < len && (ptr[pos] == ' ' || ptr[pos] == '\t' ||
			     ptr[pos] == '\n' || ptr[pos] == '\r'))
		pos++;

	parser->position = pos;
}

static int json__decode_string(struct json_parser *parser,
			       struct json_value *value)
{
	int start = parser->position + 1;
	int end = start;
	int buffer_size = 16;
	int buffer_length = 0;
	char *buffer = malloc(buffer_size);

	if (buffer == NULL) {
		JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY,
				  "Memory allocation failed");
		return -1;
	}

	while (end < parser->length && parser->input[end] != '"') {
		if (parser->input[end] == '\\') {
			end++;
			if (end >= parser->length) {
				free(buffer);
				JSON_PARSER_ERROR(
					parser, JSON_ERROR_SYNTAX,
					"Unterminated escape sequence");
				return -1;
			}

			if (parser->input[end] == 'u') {
				// Handle \uXXXX Unicode escape sequences
				end++;
				if (end + 3 >= parser->length) {
					free(buffer);
					JSON_PARSER_ERROR(
						parser, JSON_ERROR_SYNTAX,
						"Incomplete Unicode escape sequence");
					return -1;
				}

				// Parse 4 hex digits
				unsigned int code_point = 0;
				for (int i = 0; i < 4; i++) {
					char c = parser->input[end + i];
					code_point <<= 4;

					if (c >= '0' && c <= '9') {
						code_point |= (c - '0');
					} else if (c >= 'a' && c <= 'f') {
						code_point |= (c - 'a' + 10);
					} else if (c >= 'A' && c <= 'F') {
						code_point |= (c - 'A' + 10);
					} else {
						free(buffer);
						JSON_PARSER_ERROR(
							parser,
							JSON_ERROR_SYNTAX,
							"Invalid hex digit in Unicode escape");
						return -1;
					}
				}

				// Check for surrogate pair
				if (code_point >= 0xD800 &&
				    code_point <= 0xDBFF) {
					// High surrogate - need to get the low surrogate
					end += 4; // Move past the first \uXXXX

					// Check for the sequence \uYYYY where YYYY is the low surrogate
					if (end + 1 >= parser->length ||
					    parser->input[end] != '\\' ||
					    parser->input[end + 1] != 'u') {
						free(buffer);
						JSON_PARSER_ERROR(
							parser,
							JSON_ERROR_SYNTAX,
							"Expected low surrogate after high surrogate");
						return -1;
					}

					end += 2; // Move past \u
					if (end + 3 >= parser->length) {
						free(buffer);
						JSON_PARSER_ERROR(
							parser,
							JSON_ERROR_SYNTAX,
							"Incomplete Unicode escape sequence for low surrogate");
						return -1;
					}

					// Parse 4 hex digits for low surrogate
					unsigned int low_surrogate = 0;
					for (int i = 0; i < 4; i++) {
						char c = parser->input[end + i];
						low_surrogate <<= 4;

						if (c >= '0' && c <= '9') {
							low_surrogate |=
								(c - '0');
						} else if (c >= 'a' &&
							   c <= 'f') {
							low_surrogate |=
								(c - 'a' + 10);
						} else if (c >= 'A' &&
							   c <= 'F') {
							low_surrogate |=
								(c - 'A' + 10);
						} else {
							free(buffer);
							JSON_PARSER_ERROR(
								parser,
								JSON_ERROR_SYNTAX,
								"Invalid hex digit in low surrogate");
							return -1;
						}
					}

					// Validate low surrogate range
					if (low_surrogate < 0xDC00 ||
					    low_surrogate > 0xDFFF) {
						free(buffer);
						JSON_PARSER_ERROR(
							parser,
							JSON_ERROR_SYNTAX,
							"Invalid low surrogate value");
						return -1;
					}

					// Combine surrogate pair to get actual code point
					code_point =
						0x10000 +
						((code_point - 0xD800) << 10) +
						(low_surrogate - 0xDC00);
					end += 4; // Move past the second \uXXXX
				} else {
					end += 4; // Move past \uXXXX for non-surrogate case
				}

				// Encode code point as UTF-8
				if (code_point < 0x80) {
					// 1-byte character
					if (buffer_length + 1 >= buffer_size) {
						buffer_size *= 2;
						char *new_buffer = realloc(
							buffer, buffer_size);
						if (new_buffer == NULL) {
							free(buffer);
							JSON_PARSER_ERROR(
								parser,
								JSON_ERROR_MEMORY,
								"Memory allocation failed");
							return -1;
						}
						buffer = new_buffer;
					}
					buffer[buffer_length++] =
						(char)code_point;
				} else if (code_point < 0x800) {
					// 2-byte character
					if (buffer_length + 2 >= buffer_size) {
						buffer_size *= 2;
						char *new_buffer = realloc(
							buffer, buffer_size);
						if (new_buffer == NULL) {
							free(buffer);
							JSON_PARSER_ERROR(
								parser,
								JSON_ERROR_MEMORY,
								"Memory allocation failed");
							return -1;
						}
						buffer = new_buffer;
					}
					buffer[buffer_length++] =
						(char)(0xC0 |
						       (code_point >> 6));
					buffer[buffer_length++] =
						(char)(0x80 |
						       (code_point & 0x3F));
				} else if (code_point < 0x10000) {
					// 3-byte character
					if (buffer_length + 3 >= buffer_size) {
						buffer_size *= 2;
						char *new_buffer = realloc(
							buffer, buffer_size);
						if (new_buffer == NULL) {
							free(buffer);
							JSON_PARSER_ERROR(
								parser,
								JSON_ERROR_MEMORY,
								"Memory allocation failed");
							return -1;
						}
						buffer = new_buffer;
					}
					buffer[buffer_length++] =
						(char)(0xE0 |
						       (code_point >> 12));
					buffer[buffer_length++] =
						(char)(0x80 |
						       ((code_point >> 6) &
							0x3F));
					buffer[buffer_length++] =
						(char)(0x80 |
						       (code_point & 0x3F));
				} else {
					// 4-byte character
					if (buffer_length + 4 >= buffer_size) {
						buffer_size *= 2;
						char *new_buffer = realloc(
							buffer, buffer_size);
						if (new_buffer == NULL) {
							free(buffer);
							JSON_PARSER_ERROR(
								parser,
								JSON_ERROR_MEMORY,
								"Memory allocation failed");
							return -1;
						}
						buffer = new_buffer;
					}
					buffer[buffer_length++] =
						(char)(0xF0 |
						       (code_point >> 18));
					buffer[buffer_length++] =
						(char)(0x80 |
						       ((code_point >> 12) &
							0x3F));
					buffer[buffer_length++] =
						(char)(0x80 |
						       ((code_point >> 6) &
							0x3F));
					buffer[buffer_length++] =
						(char)(0x80 |
						       (code_point & 0x3F));
				}

				continue; // Skip the increment at the end of the loop
			} else {
				// Handle standard escape sequences
				char escaped_char;
				switch (parser->input[end]) {
				case '"':
					escaped_char = '"';
					break;
				case '\\':
					escaped_char = '\\';
					break;
				case '/':
					escaped_char = '/';
					break;
				case 'b':
					escaped_char = '\b';
					break;
				case 'f':
					escaped_char = '\f';
					break;
				case 'n':
					escaped_char = '\n';
					break;
				case 'r':
					escaped_char = '\r';
					break;
				case 't':
					escaped_char = '\t';
					break;
				default:
					free(buffer);
					JSON_PARSER_ERROR(
						parser, JSON_ERROR_SYNTAX,
						"Invalid escape character");
					return -1;
				}

				if (buffer_length + 1 >= buffer_size) {
					buffer_size *= 2;
					char *new_buffer =
						realloc(buffer, buffer_size);
					if (new_buffer == NULL) {
						free(buffer);
						JSON_PARSER_ERROR(
							parser,
							JSON_ERROR_MEMORY,
							"Memory allocation failed");
						return -1;
					}
					buffer = new_buffer;
				}

				buffer[buffer_length++] = escaped_char;
			}
		} else {
			if (buffer_length + 1 >= buffer_size) {
				buffer_size *= 2;
				char *new_buffer = realloc(buffer, buffer_size);
				if (new_buffer == NULL) {
					free(buffer);
					JSON_PARSER_ERROR(
						parser, JSON_ERROR_MEMORY,
						"Memory allocation failed");
					return -1;
				}
				buffer = new_buffer;
			}

			buffer[buffer_length++] = parser->input[end];
		}
		end++;
	}

	if (end == parser->length) {
		free(buffer);
		JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
				  "Unterminated string");
		return -1;
	}

	buffer[buffer_length] = 0;

	value->type = JSON_TYPE_STRING;
	value->string.value = buffer;
	value->string.length = buffer_length;

	parser->position = end + 1;
	return 0;
}

static void json__parser_consume_digits(struct json_parser *parser)
{
	while (parser->position < parser->length &&
	       parser->input[parser->position] >= '0' &&
	       parser->input[parser->position] <= '9') {
		parser->position++;
	}
}

static int json__decode_number(struct json_parser *parser,
			       struct json_value *value)
{
	int start = parser->position;
	char *endptr;

	if (parser->position < parser->length &&
	    parser->input[parser->position] == '-')
		parser->position++;

	if (parser->position < parser->length &&
	    parser->input[parser->position] == '0') {
		parser->position++;
	} else if (parser->position < parser->length &&
		   parser->input[parser->position] >= '1' &&
		   parser->input[parser->position] <= '9') {
		json__parser_consume_digits(parser);
	} else {
		JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER,
				  "Invalid number");
		return -1;
	}

	if (parser->position < parser->length &&
	    parser->input[parser->position] == '.') {
		parser->position++;
		if (parser->position < parser->length &&
		    parser->input[parser->position] >= '0' &&
		    parser->input[parser->position] <= '9') {
			json__parser_consume_digits(parser);
		} else {
			JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER,
					  "Invalid number");
			return -1;
		}
	}

	if (parser->position < parser->length &&
	    (parser->input[parser->position] == 'e' ||
	     parser->input[parser->position] == 'E')) {
		parser->position++;
		if (parser->position < parser->length &&
		    (parser->input[parser->position] == '-' ||
		     parser->input[parser->position] == '+')) {
			parser->position++;
		}
		if (parser->position < parser->length &&
		    parser->input[parser->position] >= '0' &&
		    parser->input[parser->position] <= '9') {
			json__parser_consume_digits(parser);
		} else {
			JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER,
					  "Invalid number");
			return -1;
		}
	}

	if (start == parser->position) {
		JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER,
				  "Invalid number");
		return -1;
	}

	value->type = JSON_TYPE_NUMBER;
	endptr = NULL;
	value->number = strtod(parser->input + start, &endptr);

	if (endptr != parser->input + parser->position) {
		JSON_PARSER_ERROR(parser, JSON_ERROR_INVALID_NUMBER,
				  "Invalid number");
		return -1;
	}

	return 0;
}

static int json__decode_array(struct json_parser *parser,
			      struct json_value *array)
{
	if (parser->length - parser->position < 1) {
		JSON_PARSER_ERROR(parser, JSON_ERROR_EOF,
				  "Unexpected end of input");
		return -1;
	}

	if (parser->input[parser->position] != '[') {
		JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected '['");
		return -1;
	}

	array->type = JSON_TYPE_ARRAY;
	json_array_init(array);

	if (parser->position + 1 < parser->length &&
	    parser->input[parser->position + 1] == ']') {
		parser->position += 2;
		return 0;
	}

	parser->position += 1; // Skip '['
	while (parser->position < parser->length &&
	       parser->input[parser->position] != ']') {
		json__parse_whitespace(parser);

		struct json_value *item;
		if ((item = malloc(sizeof(struct json_value))) == NULL) {
			JSON_PARSER_ERROR(parser, JSON_ERROR_MEMORY,
					  "Memory allocation failed");
			json_array_free(array);
			return -1;
		}

		if (json__decode_value(parser, item) != 0) {
			free(item);
			json_array_free(array);
			return -1;
		}

		json_array_push(array, item);
		json__parse_whitespace(parser);

		if (parser->position < parser->length &&
		    parser->input[parser->position] == ',') // Skip ','
			parser->position++;
	}

	if (parser->position >= parser->length ||
	    parser->input[parser->position] != ']') {
		JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
				  "Expected closing ']' for array");
		json_array_free(array);
		return -1;
	}

	if (parser->position < parser->length) // Skip ']'
		parser->position++;
	return 0;
}

static int json__decode_object(struct json_parser *parser,
			       struct json_value *object)
{
	object->type = JSON_TYPE_OBJECT;
	json_object_init(object);

	parser->position++;
	json__parse_whitespace(parser);

	while (parser->position < parser->length &&
	       parser->input[parser->position] != '}') {
		struct json_value key, *value;
		json__parse_whitespace(parser);

		if (parser->input[parser->position] != '"') {
			JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
					  "Expected string key");
			return -1;
		}

		if (json__decode_string(parser, &key) != 0) {
			JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
					  "Failed to parse string key");
			return -1;
		}

		json__parse_whitespace(parser);
		if (parser->position >= parser->length ||
		    parser->input[parser->position] != ':') {
			free(key.string.value);
			JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
					  "Expected ':' after string key");
			return -1;
		}
		parser->position++; // Skip the colon

		value = malloc(sizeof(struct json_value));
		if (value == NULL) {
			free(key.string.value);
			JSON_PARSER_ERROR(
				parser, JSON_ERROR_MEMORY,
				"Failed to allocate memory for JSON item");
			return -1;
		}

		json__parse_whitespace(parser);
		if (json__decode_value(parser, value) != 0) {
			free(value);
			free(key.string.value);
			JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
					  "Failed to parse JSON value");
			return -1;
		}

		if (json_object_set(object, key.string.value, value) != 0) {
			free(value);
			free(key.string.value);
			JSON_PARSER_ERROR(
				parser, JSON_ERROR_MEMORY,
				"Failed to set key-value pair in object");
			return -1;
		}

		free(key.string.value);
		json__parse_whitespace(parser);
		if (parser->position < parser->length &&
		    parser->input[parser->position] == ',')
			parser->position++; // Skip comma or closing brace
	}

	if (parser->position >= parser->length ||
	    parser->input[parser->position] != '}') {
		JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX,
				  "Expected '}' after JSON object");
		return -1;
	}

	parser->position++; // Skip the closing brace

	return 0;
}

static int json__decode_value(struct json_parser *parser,
			      struct json_value *value)
{
	int rc;
	char c = parser->input[parser->position];

	if (c == '"') {
		rc = json__decode_string(parser, value);
	} else if (c == '[') {
		rc = json__decode_array(parser, value);
	} else if (c == '{') {
		rc = json__decode_object(parser, value);
	} else if (json__streqn(parser->input + parser->position, "true", 4)) {
		value->type = JSON_TYPE_BOOLEAN;
		value->number = 1;
		parser->position += 4;
		return 0;
	} else if (json__streqn(parser->input + parser->position, "false", 4)) {
		value->type = JSON_TYPE_BOOLEAN;
		value->number = 0;
		parser->position += 5;
		return 0;
	} else if (json__streqn(parser->input + parser->position, "null", 4)) {
		value->type = JSON_TYPE_NULL;
		parser->position += 4;
		return 0;
	} else {
		rc = json__decode_number(parser, value);
	}

#if defined(JSON_ERROR)
	if (rc != 0 && parser->error.code == JSON_ERROR_NONE) {
		JSON_PARSER_ERROR(parser, JSON_ERROR_SYNTAX, "Expected value");
	}
#endif

	return rc;
}

struct json_value *json_decode_with_length(const char *json, int length)
{
	struct json_parser parser;
	struct json_value *value = NULL;

	parser.input = json;
	parser.length = length;
	parser.position = 0;

#if defined(JSON_ERROR)
	parser.error.code = JSON_ERROR_NONE;
	parser.error.message = NULL;
#endif

	if ((value = malloc(sizeof(struct json_value))) == NULL)
		return NULL;

	if (json__decode_value(&parser, value) != 0) {
#if defined(JSON_ERROR) && !defined(JSON_ERROR_HANDLER)
		if (parser.error.code != JSON_ERROR_NONE) {
			fprintf(stderr,
				"JSON(\033[1merror\033[m): %s:%d %s at\n",
				parser.error.func, parser.error.line,
				parser.error.message);
		}
#elif defined(JSON_ERROR) && defined(JSON_ERROR_HANDLER)
		if (parser.error.code != JSON_ERROR_NONE) {
			JSON_ERROR_HANDLER(parser.error.code,
					   parser.error.message);
		}
#endif
		free(value);
		return NULL;
	}

	return value;
}

struct json_value *json_decode(const char *json)
{
	return json_decode_with_length(json, json__strlen(json));
}

static char *json__encode_array(struct json_value *value)
{
	int length;
	int buffer_size;
	char *encoded_array;
	if (value == NULL) {
		return NULL;
	}

	buffer_size = 512;
	encoded_array = malloc(buffer_size);
	if (encoded_array == NULL) {
		return NULL;
	}

	length = 1;
	encoded_array[0] = '[';

	for (int i = 0; i < value->array.length; i++) {
		int needed;
		int encoded_item_len;
		char *encoded_item = json_encode(value->array.items[i]);
		if (encoded_item == NULL) {
			free(encoded_array);
			return NULL;
		}

		encoded_item_len = json__strlen(encoded_item);

		needed = length + encoded_item_len + (i > 0 ? 1 : 0) + 1;
		if (needed > buffer_size) {
			char *new_encoded_array = realloc(
				encoded_array, (buffer_size = needed * 2));
			if (new_encoded_array == NULL) {
				free(encoded_array);
				free(encoded_item);
				return NULL;
			}
			encoded_array = new_encoded_array;
		}

		if (i > 0) {
			encoded_array[length++] = ',';
		}

		for (int i = 0; i < encoded_item_len; i++)
			encoded_array[length + i] = encoded_item[i];

		length += encoded_item_len;

		free(encoded_item);
	}

	encoded_array[length++] = ']';
	encoded_array[length] = '\0';

	return encoded_array;
}

static char *json__encode_object(struct json_value *object)
{
	int length;
	int buffer_size = 256;
	char *encoded_object = malloc(buffer_size);
	if (encoded_object == NULL) {
		return NULL;
	}

	encoded_object[0] = '{';
	length = 1;

	for (int i = 0; i < object->object.n_items; i++) {
		char *new_buffer;
		char *key = object->object.items[i]->key;
		char *value = json_encode(object->object.items[i]->value);

		int key_length = json__strlen(key);
		int value_length = json__strlen(value);

		int needed = length + key_length + value_length + 4 +
			     (i < object->object.n_items - 1 ? 1 : 0);

		if (needed > buffer_size) {
			buffer_size = needed * 2;
			new_buffer = realloc(encoded_object, buffer_size);
			if (new_buffer == NULL) {
				free(encoded_object);
				free(value);
				return NULL;
			}
			encoded_object = new_buffer;
		}

		encoded_object[length++] = '"';
		for (int j = 0; j < key_length; j++)
			encoded_object[length++] = key[j];

		encoded_object[length++] = '"';
		encoded_object[length++] = ':';
		for (int j = 0; j < value_length; j++)
			encoded_object[length++] = value[j];

		free(value);

		if (i < object->object.n_items - 1) {
			encoded_object[length++] = ',';
		}
	}

	encoded_object[length++] = '}';
	encoded_object[length] = '\0';

	return encoded_object;
}

static char *json__encode_string(struct json_value *value)
{
	const char *str = value->string.value;
	int length = value->string.length;
	int buffer_size =
		length * 6 +
		3; // Maximum possible size (all chars escaped + quotes)
	char *buffer = malloc(buffer_size);
	int pos = 0;

	if (!buffer)
		return NULL;

	buffer[pos++] = '"';

	for (int i = 0; i < length; i++) {
		unsigned char c = (unsigned char)str[i];

		if (c == '"' || c == '\\' || c == '/') {
			buffer[pos++] = '\\';
			buffer[pos++] = c;
		} else if (c == '\b') {
			buffer[pos++] = '\\';
			buffer[pos++] = 'b';
		} else if (c == '\f') {
			buffer[pos++] = '\\';
			buffer[pos++] = 'f';
		} else if (c == '\n') {
			buffer[pos++] = '\\';
			buffer[pos++] = 'n';
		} else if (c == '\r') {
			buffer[pos++] = '\\';
			buffer[pos++] = 'r';
		} else if (c == '\t') {
			buffer[pos++] = '\\';
			buffer[pos++] = 't';
		} else if (c < 32) {
			// Control characters must be escaped as \uXXXX
			buffer[pos++] = '\\';
			buffer[pos++] = 'u';
			buffer[pos++] = '0';
			buffer[pos++] = '0';
			buffer[pos++] = "0123456789ABCDEF"[(c >> 4) & 0xF];
			buffer[pos++] = "0123456789ABCDEF"[c & 0xF];
		} else if (c < 128) {
			// ASCII characters
			buffer[pos++] = c;
		} else {
			// UTF-8 encoded characters
			// Check if this is a multi-byte sequence
			const unsigned char *bytes =
				(const unsigned char *)&str[i];
			if ((c & 0xE0) == 0xC0) {
				// Two-byte sequence
				if (i + 1 >= length ||
				    (bytes[i + 1] & 0xC0) != 0x80) {
					// Invalid UTF-8 sequence, escape as replacement character
					buffer[pos++] = '\\';
					buffer[pos++] = 'u';
					buffer[pos++] = 'F';
					buffer[pos++] = 'F';
					buffer[pos++] = 'F';
					buffer[pos++] = 'D';
				} else {
					// Valid two-byte sequence, keep as is
					buffer[pos++] = c;
					buffer[pos++] = str[++i];
				}
			} else if ((c & 0xF0) == 0xE0) {
				// Three-byte sequence
				if (i + 2 >= length ||
				    (bytes[i + 1] & 0xC0) != 0x80 ||
				    (bytes[i + 2] & 0xC0) != 0x80) {
					// Invalid UTF-8 sequence
					buffer[pos++] = '\\';
					buffer[pos++] = 'u';
					buffer[pos++] = 'F';
					buffer[pos++] = 'F';
					buffer[pos++] = 'F';
					buffer[pos++] = 'D';
				} else {
					// Valid three-byte sequence, keep as is
					buffer[pos++] = c;
					buffer[pos++] = str[++i];
					buffer[pos++] = str[++i];
				}
			} else if ((c & 0xF8) == 0xF0) {
				// Four-byte sequence
				if (i + 3 >= length ||
				    (bytes[i + 1] & 0xC0) != 0x80 ||
				    (bytes[i + 2] & 0xC0) != 0x80 ||
				    (bytes[i + 3] & 0xC0) != 0x80) {
					// Invalid UTF-8 sequence
					buffer[pos++] = '\\';
					buffer[pos++] = 'u';
					buffer[pos++] = 'F';
					buffer[pos++] = 'F';
					buffer[pos++] = 'F';
					buffer[pos++] = 'D';
				} else {
					// Valid four-byte sequence, keep as is
					buffer[pos++] = c;
					buffer[pos++] = str[++i];
					buffer[pos++] = str[++i];
					buffer[pos++] = str[++i];
				}
			} else {
				// Invalid UTF-8 start byte
				buffer[pos++] = '\\';
				buffer[pos++] = 'u';
				buffer[pos++] = 'F';
				buffer[pos++] = 'F';
				buffer[pos++] = 'F';
				buffer[pos++] = 'D';
			}
		}
	}

	buffer[pos++] = '"';
	buffer[pos] = '\0';

	// Reallocate to actual size
	char *result = realloc(buffer, pos + 1);
	return result ? result : buffer;
}

char *json_encode(struct json_value *value)
{
	switch (value->type) {
	case JSON_TYPE_STRING:
		return json__encode_string(value);
	case JSON_TYPE_NUMBER: {
		char *ptr;
		char buffer[32];
		int length = snprintf(buffer, sizeof(buffer), "%.17g",
				      value->number);

		if ((ptr = malloc(length + 1)) == NULL)
			return NULL;

		for (int i = 0; i < length; i++)
			ptr[i] = buffer[i];

		ptr[length] = 0;
		return ptr;
	}
	case JSON_TYPE_BOOLEAN: {
		char *ptr;
		const char *boolean_str = value->number != 0.0 ? "true" :
								 "false";
		int length = json__strlen(boolean_str);

		if ((ptr = malloc(length + 1)) == NULL)
			return NULL;

		for (int i = 0; i < length; i++)
			ptr[i] = boolean_str[i];

		ptr[length] = 0;
		return ptr;
	}
	case JSON_TYPE_NULL: {
		char *ptr;
		const char *null_str = "null";
		int length = json__strlen(null_str);

		if ((ptr = malloc(length + 1)) == NULL)
			return NULL;

		for (int i = 0; i < length; i++)
			ptr[i] = null_str[i];

		ptr[length] = 0;
		return ptr;
	}
	case JSON_TYPE_ARRAY:
		return json__encode_array(value);
	case JSON_TYPE_OBJECT:
		return json__encode_object(value);
	default:
		return NULL;
	}
}

void json_object_init(struct json_value *object)
{
	object->object.n_items = 0;
	object->object.capacity = 0;
	object->object.items = NULL;
}

struct json_value *json_object_new(void)
{
	struct json_value *object = malloc(sizeof(struct json_value));
	if (!object) {
		return NULL;
	}

	object->type = JSON_TYPE_OBJECT;
	json_object_init(object);

	return object;
}

void json_object_free(struct json_value *object)
{
	for (int i = 0; i < object->object.n_items; i++) {
		json_free(object->object.items[i]->value);
		free(object->object.items[i]->key);
		free(object->object.items[i]);
	}

	free(object->object.items);
	free(object);
}

int json_object_set(struct json_value *object, const char *key,
		    struct json_value *value)
{
	if (object->object.n_items >=
	    object->object.capacity * JSON_OBJECT_CAPACITY_THRESHOLD) {
		void *items;
		int capacity;

		if (object->object.capacity)
			capacity = object->object.capacity *
				   JSON_OBJECT_CAPACITY_MULTIPLIER;
		else
			capacity = JSON_OBJECT_INITIAL_CAPACITY;

		if ((items = realloc(
			     object->object.items,
			     capacity * sizeof(*object->object.items))) == NULL)
			return -1;

		object->object.items = items;
		object->object.capacity = capacity;
	}

	for (int i = 0; i < object->object.n_items; i++) {
		if (json__streq(object->object.items[i]->key, key)) {
			json_free(object->object.items[i]->value);
			object->object.items[i]->value = value;
			return 0;
		}
	}

	int idx = object->object.n_items++;
	object->object.items[idx] = malloc(sizeof(*object->object.items[idx]));

	if (object->object.items[idx] == NULL)
		return -1;

	int key_len = json__strlen(key);
	object->object.items[idx]->key = malloc(key_len + 1);
	strncpy(object->object.items[idx]->key, key, key_len);
	object->object.items[idx]->key[key_len] = '\0';
	object->object.items[idx]->value = value;

	return 0;
}

int json_object_has(struct json_value *object, const char *key)
{
	for (int i = 0; i < object->object.n_items; i++)
		if (json__streq(object->object.items[i]->key, key))
			return 1;

	return 0;
}

struct json_value *json_object_get(struct json_value *object, const char *key)
{
	if (object == NULL || object->object.items == NULL)
		return NULL;

	for (int i = 0; i < object->object.n_items; i++)
		if (object->object.items[i] != NULL &&
		    object->object.items[i]->key != NULL &&
		    json__streq(object->object.items[i]->key, key))
			return object->object.items[i]->value;

	return NULL;
}

void json_object_remove(struct json_value *object, const char *key)
{
	for (int i = 0; i < object->object.n_items; i++) {
		if (json__streq(object->object.items[i]->key, key)) {
			switch (object->object.items[i]->value->type) {
			case JSON_TYPE_ARRAY:
				json_array_free(object->object.items[i]->value);
				break;
			case JSON_TYPE_OBJECT:
				json_object_free(
					object->object.items[i]->value);
				break;
			case JSON_TYPE_STRING:
				json_string_free(
					object->object.items[i]->value);
				break;
			default:
				free(object->object.items[i]->value);
				break;
			}

			free(object->object.items[i]->key);
			free(object->object.items[i]);

			if (i < object->object.n_items - 1) {
				object->object.items[i] =
					object->object
						.items[object->object.n_items -
						       1];
			}

			object->object.n_items--;
		}
	}
}

int json_object_iter(const struct json_value *object, int *iter, char **key,
		     struct json_value **value)
{
	if (*iter >= object->object.n_items)
		return 0;

	*key = object->object.items[*iter]->key;
	*value = object->object.items[(*iter)++]->value;

	return 1;
}

void json_object_clear(struct json_value *object)
{
	char *key;
	int iter = 0;
	struct json_value *value;

	while (json_object_iter(object, &iter, &key, &value)) {
		json_object_remove(object, key);
		iter--;
	}
}

void json_array_init(struct json_value *array)
{
	array->array.length = 0;
	array->array.capacity = 0;
	array->array.items = NULL;
}

struct json_value *json_array_new(void)
{
	struct json_value *value;
	if ((value = malloc(sizeof(struct json_value))) == NULL)
		return NULL;

	value->type = JSON_TYPE_ARRAY;
	json_array_init(value);

	return value;
}

void json_array_free(struct json_value *value)
{
	for (int i = 0; i < value->array.length; i++) {
		switch (value->array.items[i]->type) {
		case JSON_TYPE_OBJECT:
			json_object_free(value->array.items[i]);
			break;
		case JSON_TYPE_ARRAY:
			json_array_free(value->array.items[i]);
			break;
		case JSON_TYPE_STRING:
			json_string_free(value->array.items[i]);
			break;
		default:
			free(value->array.items[i]);
			break;
		}
	}
	free(value->array.items);
	free(value);
}

void json_free(struct json_value *value)
{
	switch (value->type) {
	case JSON_TYPE_OBJECT:
		json_object_free(value);
		break;
	case JSON_TYPE_ARRAY:
		json_array_free(value);
		break;
	case JSON_TYPE_STRING:
		json_string_free(value);
		break;
	default:
		free(value);
	}
}

struct json_value *json_deep_copy(struct json_value *value)
{
	struct json_value *new_value;
	struct json_value *element;
	char *key;
	int iter;

	iter = 0;

	switch (value->type) {
	case JSON_TYPE_OBJECT:
		new_value = json_object_new();
		while (json_object_iter(value, &iter, &key, &element))
			json_object_set(new_value, key,
					json_deep_copy(element));
		return new_value;

	case JSON_TYPE_ARRAY:
		new_value = json_array_new();
		while (json_array_iter(value, &iter, &element))
			json_array_push(new_value, json_deep_copy(element));
		return new_value;

	case JSON_TYPE_STRING:
		return json_string_new(value->string.value);

	default:
		new_value = malloc(sizeof(struct json_value));
		memcpy(new_value, value, sizeof(*value));
		return new_value;
	}

	return NULL;
}

void json_array_remove(struct json_value *array, int index)
{
	switch (array->array.items[index]->type) {
	case JSON_TYPE_OBJECT:
		json_object_free(array->array.items[index]);
		break;
	case JSON_TYPE_ARRAY:
		json_array_free(array->array.items[index]);
		break;
	case JSON_TYPE_STRING:
		json_string_free(array->array.items[index]);
		break;
	default:
		break;
	}

	for (int i = index; i < array->array.length - 1; i++)
		array->array.items[i] = array->array.items[i + 1];
	array->array.length--;
}

inline int json_array_length(struct json_value *array)
{
	return array->array.length;
}

int json_array_push(struct json_value *array, struct json_value *value)
{
	if (array == NULL) {
		return -1;
	}

	int index = array->array.length;
	int capacity = array->array.capacity;
	int capacity_threshold = capacity * JSON_ARRAY_CAPACITY_THRESHOLD;

	if (index >= capacity_threshold) {
		int capacity;
		if (array->array.capacity > 0)
			capacity = array->array.capacity *
				   JSON_ARRAY_CAPACITY_MULTIPLIER;
		else
			capacity = JSON_ARRAY_INITIAL_CAPACITY;

		struct json_value **items;
		int size = capacity * sizeof(struct json_value *);
		if ((items = realloc(array->array.items, size)) == NULL)
			return -1;

		array->array.items = items;
		array->array.capacity = capacity;
	}

	array->array.items[array->array.length++] = value;
	return 0;
}

int json_array_iter(struct json_value *array, int *index,
		    struct json_value **value)
{
	if (*index >= array->array.length)
		return 0;

	*value = array->array.items[(*index)++];
	return 1;
}

void json_array_clear(struct json_value *array)
{
	int iter = 0;
	struct json_value *value;

	while (json_array_iter(array, &iter, &value))
		json_array_remove(array, iter--);
}

struct json_value *json_string_new(const char *string)
{
	struct json_value *value;
	if ((value = malloc(sizeof(struct json_value))) == NULL)
		return NULL;

	value->type = JSON_TYPE_STRING;
	value->string.length = json__strlen(string);
	if ((value->string.value = malloc(value->string.length + 1)) == NULL) {
		free(value);
		return NULL;
	}

	for (int i = 0; i < value->string.length; i++)
		value->string.value[i] = string[i];

	value->string.value[value->string.length] = 0;
	return value;
}

struct json_value *json_number_new(double value)
{
	struct json_value *number;
	if ((number = malloc(sizeof(struct json_value))) == NULL)
		return NULL;

	number->type = JSON_TYPE_NUMBER;
	number->number = value;

	return number;
}

struct json_value *json_boolean_new(int value)
{
	struct json_value *boolean;
	if ((boolean = malloc(sizeof(struct json_value))) == NULL)
		return NULL;

	boolean->type = JSON_TYPE_BOOLEAN;
	boolean->number = value;

	return boolean;
}

void json_string_free(struct json_value *string)
{
	free(string->string.value);
	free(string);
}

static inline void json__print_indent(int indent)
{
	for (int i = 0; i < indent; i++) {
		putchar(' ');
	}
}

static void json__print_internal(struct json_value *value, int indent)
{
	if (value == NULL) {
		printf("null");
		return;
	}

	switch (value->type) {
	case JSON_TYPE_STRING:
		printf("\"%s\"", value->string.value);
		break;
	case JSON_TYPE_NUMBER:
		printf("%.17g", value->number);
		break;
	case JSON_TYPE_BOOLEAN:
		printf("%s", value->number == 0.0 ? "false" : "true");
		break;
	case JSON_TYPE_NULL:
		printf("null");
		break;
	case JSON_TYPE_ARRAY:
		printf("[\n");
		for (int i = 0; i < value->array.length; i++) {
			json__print_indent(indent + 2);
			json__print_internal(value->array.items[i], indent + 2);
			if (i < value->array.length - 1) {
				printf(",\n");
			} else {
				printf("\n");
			}
		}
		json__print_indent(indent);
		printf("]");
		break;
	case JSON_TYPE_OBJECT:
		printf("{\n");
		for (int i = 0; i < value->object.n_items; i++) {
			json__print_indent(indent + 2);
			printf("\"%s\": ", value->object.items[i]->key);
			json__print_internal(value->object.items[i]->value,
					     indent + 2);
			if (i < value->object.n_items - 1) {
				printf(",\n");
			} else {
				printf("\n");
			}
		}
		json__print_indent(indent);
		printf("}");
		break;
	default:
		break;
	}
}

void json_print(struct json_value *value)
{
	json__print_internal(value, 0);
}

void json_println(struct json_value *value)
{
	json_print(value);
	putchar('\n');
}
