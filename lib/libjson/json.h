#ifndef JSON_H
#define JSON_H

#ifndef JSON_ARRAY_INITIAL_CAPACITY
/**
 * @brief Defines the initial capacity for JSON arrays.
 *
 * This value determines the number of elements a JSON array
 * can hold before requiring a reallocation. Adjust this value
 * to optimize memory usage and performance for your specific use case.
 */
#define JSON_ARRAY_INITIAL_CAPACITY 1
#endif

#ifndef JSON_ARRAY_CAPACITY_MULTIPLIER
/**
 * @brief Defines the capacity multiplier for JSON arrays.
 *
 * When a JSON array exceeds its current capacity, its capacity is
 * multiplied by this value to allocate additional space. A higher
 * multiplier reduces the frequency of reallocations but increases
 * memory usage.
 */
#define JSON_ARRAY_CAPACITY_MULTIPLIER 2
#endif

#ifndef JSON_ARRAY_CAPACITY_THRESHOLD
/**
 * @brief Threshold for triggering reallocation in JSON arrays.
 *
 * Defines how full the array must be (relative to capacity)
 * before reallocating. Typically set to 1 (100% full).
 */
#define JSON_ARRAY_CAPACITY_THRESHOLD 1
#endif

#ifndef JSON_OBJECT_INITIAL_CAPACITY
/**
 * @brief Defines the initial capacity for JSON objects.
 *
 * Determines how many key-value pairs a JSON object can store
 * before requiring reallocation. Increase this value to reduce
 * early reallocations for large objects.
 */
#define JSON_OBJECT_INITIAL_CAPACITY 1
#endif

#ifndef JSON_OBJECT_CAPACITY_MULTIPLIER
/**
 * @brief Growth multiplier for JSON object capacity.
 *
 * When the object exceeds its current capacity, the capacity
 * is multiplied by this factor. A larger multiplier reduces
 * the frequency of reallocations but may waste memory.
 */
#define JSON_OBJECT_CAPACITY_MULTIPLIER 2
#endif

#ifndef JSON_OBJECT_CAPACITY_THRESHOLD
/**
 * @brief Threshold for triggering reallocation in JSON objects.
 *
 * Defines how full the object must be (relative to capacity)
 * before reallocating. Typically set to 1 (100% full).
 */
#define JSON_OBJECT_CAPACITY_THRESHOLD 1
#endif

/**
 * @brief Represents a JSON value.
 *
 * This structure encapsulates a JSON value, which can be one of several types
 * including null, boolean, number, string, array, or object. The type of the
 * value is determined by the `type` field, and the actual data is stored in
 * the corresponding member of the union.
 */
struct json_value {
	/**
     * @brief Enum representing the type of the JSON value.
     *
     * The type determines which member of the union is valid.
     *
     * - `JSON_TYPE_NULL`: Represents a null value.
     * - `JSON_TYPE_BOOLEAN`: Represents a boolean value (true/false).
     * - `JSON_TYPE_NUMBER`: Represents a numeric value.
     * - `JSON_TYPE_STRING`: Represents a string value.
     * - `JSON_TYPE_ARRAY`: Represents an array of JSON values.
     * - `JSON_TYPE_OBJECT`: Represents an object with key-value pairs.
     */
	enum {
		JSON_TYPE_NULL, /**< Null value. */
		JSON_TYPE_BOOLEAN, /**< Boolean value (true/false). */
		JSON_TYPE_NUMBER, /**< Numeric value. */
		JSON_TYPE_STRING, /**< String value. */
		JSON_TYPE_ARRAY, /**< Array of JSON values. */
		JSON_TYPE_OBJECT /**< Object with key-value pairs. */
	} type;

	/**
     * @brief Union holding the actual data of the JSON value.
     *
     * The type of data stored depends on the `type` field.
     */
	union {
		/**
         * @brief Holds a numeric value when `type` is `JSON_TYPE_NUMBER`.
         */
		double number;

		/**
         * @brief Holds an array of JSON values when `type` is
         * `JSON_TYPE_ARRAY`.
         *
         * - `capacity`: The total allocated capacity of the array.
         * - `length`: The current number of elements in the array.
         * - `items`: A pointer to an array of pointers to `json_value`
         * elements.
         */
		struct {
			int capacity; /**< Total allocated capacity of the array. */
			int length; /**< Current number of elements in the array. */
			struct json_value *
				*items; /**< Pointer to an array of pointers to
                                          `json_value` elements. */
		} array;

		/**
         * @brief Holds a string value when `type` is `JSON_TYPE_STRING`.
         *
         * - `capacity`: The total allocated capacity of the string.
         * - `length`: The current length of the string (excluding null
         * terminator).
         * - `value`: A pointer to the null-terminated string.
         */
		struct {
			int capacity; /**< Total allocated capacity of the string. */
			int length; /**< Current length of the string (excluding null
                             terminator). */
			char *value; /**< Pointer to the null-terminated string. */
		} string;

		/**
         * @brief Holds an object with key-value pairs when `type` is
         * `JSON_TYPE_OBJECT`.
         *
         * - `capacity`: The total allocated capacity for key-value pairs.
         * - `n_items`: The current number of key-value pairs in the object.
         * - `items`: A pointer to an array of pointers to key-value pair
         * structures. Each key-value pair consists of:
         *     - `key`: A pointer to a null-terminated string representing the
         * key.
         *     - `value`: A pointer to a `json_value` representing the
         * associated value.
         */
		struct {
			int capacity; /**< Total allocated capacity for key-value pairs. */
			int n_items; /**< Current number of key-value pairs in the object.
                           */
			struct {
				char *key; /**< Pointer to a null-terminated string representing
                                             the key. */
				struct json_value *
					value; /**< Pointer to a `json_value` representing
                                             the associated value. */
			} **items; /**< Pointer to an array of pointers to key-value pair
                                             structures. */
		} object;
	};
};

/**
 * @brief Checks if a JSON value is an array.
 *
 * This macro determines whether a given JSON value is of type array.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is an array, 0 otherwise.
 */
#define json_is_array(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_ARRAY)

/**
 * @brief Checks if a JSON value is a boolean.
 *
 * This macro determines whether a given JSON value is of type boolean.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is a boolean, 0 otherwise.
 */
#define json_is_boolean(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_BOOLEAN)

/**
 * @brief Checks if a JSON value is null.
 *
 * This macro determines whether a given JSON value is of type null.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is null, 0 otherwise.
 */
#define json_is_null(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_NULL)

/**
 * @brief Checks if a JSON value is a number.
 *
 * This macro determines whether a given JSON value is of type number.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is a number, 0 otherwise.
 */
#define json_is_number(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_NUMBER)

/**
 * @brief Checks if a JSON value is an object.
 *
 * This macro determines whether a given JSON value is of type object.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is an object, 0 otherwise.
 */
#define json_is_object(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_OBJECT)

/**
 * @brief Checks if a JSON value is a string.
 *
 * This macro determines whether a given JSON value is of type string.
 *
 * @param VALUE The JSON value to check.
 * @return Non-zero if the value is a string, 0 otherwise.
 */
#define json_is_string(VALUE) ((VALUE) && (VALUE)->type == JSON_TYPE_STRING)

/**
 * @brief Retrieves the boolean value from a JSON boolean.
 *
 * This macro extracts the boolean value from a JSON boolean type.
 *
 * @param VALUE The JSON boolean value to retrieve.
 * @return Non-zero if the boolean is true, 0 if false.
 */
#define json_boolean_get(VALUE) ((VALUE)->number != 0.0)

/**
 * @brief Sets the boolean value of a JSON boolean.
 *
 * This macro modifies the boolean value of a JSON boolean type.
 *
 * @param VALUE The JSON boolean value to modify.
 * @param STATE The new boolean value to set (non-zero for true, 0 for false).
 */
#define json_boolean_set(VALUE, STATE) ((VALUE)->number = STATE)

/**
 * @brief Retrieves the numeric value from a JSON number.
 *
 * This macro extracts the numeric value from a JSON number type.
 *
 * @param VALUE The JSON number value to retrieve.
 * @return The numeric value.
 */
#define json_number_get(VALUE) ((VALUE)->number)

/**
 * @brief Sets the numeric value of a JSON number.
 *
 * This macro modifies the numeric value of a JSON number type.
 *
 * @param JSON The JSON number value to modify.
 * @param VALUE The new numeric value to set.
 */
#define json_number_set(JSON, VALUE) ((JSON)->number = VALUE)

/**
 * @brief Retrieves the string value from a JSON string.
 *
 * This macro extracts the string value from a JSON string type.
 *
 * @param JSON The JSON string value to retrieve.
 * @return A pointer to the string value.
 */
#define json_string_get(JSON) ((JSON) ? (JSON)->string.value : NULL)

/**
 * @brief Sets the string value of a JSON string.
 *
 * This macro modifies the string value of a JSON string type.
 *
 * @param JSON The JSON string value to modify.
 * @param VALUE The new string value to set.
 */
#define json_string_set(JSON, VALUE) ((JSON)->string.value = VALUE)

/**
 * @brief Retrieves an element from a JSON array.
 *
 * This function retrieves an element from a JSON array at the specified index.
 *
 * @param array The JSON array to retrieve from.
 * @param index The index of the element to retrieve.
 * @return A pointer to the element at the specified index, or NULL if out of
 * bounds.
 */
struct json_value *json_array_get(struct json_value *array, int index);

/**
 * @brief Sets an element in a JSON array.
 *
 * This macro sets an element in a JSON array at the specified index.
 *
 * @param array The JSON array to modify.
 * @param index The index of the element to set.
 * @param value The new value to set.
 */
#define json_array_set(ARRAY, INDEX, VALUE) \
	((ARRAY)->array.items[(INDEX)] = (VALUE))

/**
 * @brief Retrieves the number of key-value pairs in a JSON object.
 *
 * This macro returns the total count of key-value pairs in a JSON object.
 *
 * @param OBJECT A pointer to the JSON object to query.
 * @return The total count of key-value pairs in the object.
 */
#define json_object_count(OBJECT) ((OBJECT)->object.n_items)

/**
 * @brief Retrieves the number of elements in a JSON array.
 *
 * This macro returns the total count of elements in a JSON array.
 *
 * @param ARRAY A pointer to the JSON array to query.
 * @return The total count of elements in the array.
 */
#define json_array_count(ARRAY) ((ARRAY)->array.length)

/**
 * @brief Encodes a JSON value into a JSON string.
 *
 * This function takes a structured `json_value` and converts it into
 * a JSON-encoded string representation. The caller is responsible for
 * freeing the returned string using `free()`.
 *
 * @param value The JSON value to encode. Must not be NULL.
 * @return A pointer to the JSON-encoded string, or NULL if encoding fails.
 */
char *json_encode(struct json_value *value);

/**
 * @brief Decodes a JSON string into a JSON value.
 *
 * This function parses a JSON-encoded string and converts it into a
 * structured representation using the `json_value` type. The caller
 * is responsible for freeing the returned `json_value` using `json_free()`.
 *
 * @param json The JSON-encoded string to decode. Must be null-terminated.
 * @return A pointer to the decoded `json_value`, or NULL if decoding fails.
 */
struct json_value *json_decode(const char *json);

/**
 * @brief Decodes a JSON string into a JSON value.
 *
 * Decodes up to `length` bytes, see `json_decode` for more.
 *
 * @param json The JSON-encoded string to decode.
 * @param length The length of the string to decode.
 * @return A pointer to the decoded `json_value`, or NULL if decoding fails.
 */
struct json_value *json_decode_with_length(const char *json, int length);

/**
 * @brief Creates a new JSON object.
 *
 * This function allocates and initializes a new JSON object.
 *
 * @return A pointer to the newly created JSON object, or NULL on failure.
 */
struct json_value *json_object_new(void);

/**
 * @brief Frees the memory associated with a JSON object.
 *
 * This function releases all memory used by the JSON object, including
 * its key-value pairs and their associated values.
 *
 * @param object The JSON object to free.
 */
void json_object_free(struct json_value *object);

/**
 * @brief Retrieves the value associated with a key in a JSON object.
 *
 * This function searches for a key in the JSON object and returns the
 * associated value if the key exists.
 *
 * @param object The JSON object to query.
 * @param key The key to look up.
 * @return A pointer to the associated value, or NULL if the key does not exist.
 */
struct json_value *json_object_get(struct json_value *object, const char *key);

/**
 * @brief Sets a key-value pair in a JSON object.
 *
 * This function adds or updates a key-value pair in the JSON object.
 * If the key already exists, its value is updated.
 *
 * @param object The JSON object to modify.
 * @param key The key to set in the object.
 * @param value The value to associate with the key.
 * @return 0 on success, or -1 on failure.
 */
int json_object_set(struct json_value *object, const char *key,
		    struct json_value *value);

/**
 * @brief Checks if a key exists in a JSON object.
 *
 * This function determines whether a specific key exists in the JSON object.
 *
 * @param object The JSON object to query.
 * @param key The key to check for.
 * @return 1 if the key exists, 0 otherwise.
 */
int json_object_has(struct json_value *object, const char *key);

/**
 * @brief Removes a key-value pair from a JSON object.
 *
 * This function deletes a key-value pair from the JSON object. If the key
 * does not exist, the function does nothing.
 *
 * @param object The JSON object to modify.
 * @param key The key to remove.
 */
void json_object_remove(struct json_value *object, const char *key);

/**
 * @brief Iterates over the key-value pairs in a JSON object.
 *
 * This function provides a way to iterate through all key-value pairs
 * in a JSON object. The iteration index should be initialized to 0
 * before the first call.
 *
 * @param object The JSON object to iterate over.
 * @param iter A pointer to the iteration index (should be initialized to 0).
 * @param key A pointer to store the current key.
 * @param value A pointer to store the current value.
 * @return 1 if there are more items to iterate, 0 otherwise.
 */
int json_object_iter(const struct json_value *object, int *iter, char **key,
		     struct json_value **value);

/**
 * @brief Appends a value to the end of a JSON array.
 *
 * This function adds a new value to the end of the JSON array.
 *
 * @param array The JSON array to modify.
 * @param value The value to append.
 * @return 0 on success, or -1 on failure.
 */
int json_array_push(struct json_value *array, struct json_value *value);

/**
 * @brief Creates a new JSON array.
 *
 * This function allocates and initializes a new JSON array.
 *
 * @return A pointer to the newly created JSON array, or NULL on failure.
 */
struct json_value *json_array_new(void);

/**
 * @brief Frees the memory associated with a JSON array.
 *
 * This function releases all memory used by the JSON array, including
 * its elements.
 *
 * @param array The JSON array to free.
 */
void json_array_free(struct json_value *array);

/**
 * @brief Initializes a JSON array.
 *
 * This function sets up the internal structure of a JSON array,
 * preparing it for use.
 *
 * @param array The JSON array to initialize.
 */
void json_array_init(struct json_value *array);

/**
 * @brief Frees the memory associated with a JSON string.
 *
 * This function releases all memory used by the JSON string.
 *
 * @param string The JSON string to free.
 */
void json_string_free(struct json_value *string);

/**
 * @brief Removes a value at a specific index in a JSON array.
 *
 * This function deletes the value at the specified index in the JSON array.
 * If the index is out of bounds, the function fails.
 *
 * @param array The JSON array to modify.
 * @param index The index to remove the value from.
 */
void json_array_remove(struct json_value *array, int index);

/**
 * @brief Retrieves the length of a JSON array.
 *
 * This function returns the number of elements in the JSON array.
 *
 * @param array The JSON array to query.
 * @return The number of elements in the array.
 */
int json_array_length(struct json_value *array);

/**
 * @brief Iterates over the elements in a JSON array.
 *
 * This function provides a way to iterate through all elements in a JSON array.
 * The iteration index should be initialized to 0 before the first call.
 *
 * @param array The JSON array to iterate over.
 * @param index A pointer to the iteration index (should be initialized to 0).
 * @param value A pointer to store the current value.
 * @return 1 on success, or 0 if the iteration is complete.
 */
int json_array_iter(struct json_value *array, int *index,
		    struct json_value **value);

/**
 * @brief Creates a new JSON string with the specified value.
 *
 * This function allocates and initializes a new JSON string with the
 * provided string value.
 *
 * @param value The string value to initialize the JSON string with.
 * @return A pointer to the newly created JSON string, or NULL on failure.
 */
struct json_value *json_string_new(const char *value);

/**
 * @brief Creates a new JSON number with the specified value.
 *
 * This function allocates and initializes a new JSON number with the
 * provided numeric value.
 *
 * @param value The numeric value to initialize the JSON number with.
 * @return A pointer to the newly created JSON number, or NULL on failure.
 */
struct json_value *json_number_new(double value);

/**
 * @brief Creates a new JSON boolean with the specified value.
 *
 * This function allocates and initializes a new JSON boolean with the
 * provided boolean value.
 *
 * @param value The boolean value to initialize the JSON boolean with.
 * @return A pointer to the newly created JSON boolean, or NULL on failure.
 */
struct json_value *json_boolean_new(int value);

/**
 * @brief Deallocates memory associated with a JSON value and its descendants.
 *
 * This function performs a comprehensive cleanup of the specified JSON value,
 * including recursively freeing all child elements, whether they are objects,
 * arrays, or strings. It ensures that all allocated memory is properly released.
 *
 * @param value A pointer to the JSON value to be deallocated.
 */
void json_free(struct json_value *value);

/**
 * @brief Copy the whole value and all its children into a new json_value.
 *
 * In theory, no memory should be shared with the previous value.
 *
 * @param value The JSON value to deep-copy.
 */
struct json_value *json_deep_copy(struct json_value *value);

#endif
