
#ifndef H_ERROR_MANAGER
#define H_ERROR_MANAGER

#pragma once

#include <mutex>
#include <iostream>
#include <unordered_map>
#include <string>

namespace gerrormanagerlib
{
	/**
	* @struct ErrorDefinition
	* @brief Defines a single error with an associated code and description.
	*
	* This structure is used in conjunction with the `CErrorManager` class to define
	* a custom set of application-specific errors. Each error is identified by an integer
	* code and accompanied by a descriptive string.
	*
	* @note Error codes must start from 1. Code `0` is reserved to indicate "no error".
	*
	* ### Example Usage:
	* In a header file, define error codes:
	* @code
	* #define ERROR_FILE_NOT_FOUND     1
	* #define ERROR_INVALID_ARGUMENT   2
	* #define ERROR_OUT_OF_MEMORY      3
	* #define ERROR_TIMEOUT            4
	* @endcode
	*
	* Then declare the error array:
	* @code
	* static const ErrorDefinition ErrorsArray[] =
	* {
	*     { ERROR_FILE_NOT_FOUND,    "File not found" },
	*     { ERROR_INVALID_ARGUMENT,  "Invalid argument" },
	*     { ERROR_OUT_OF_MEMORY,     "Out of memory" },
	*     { ERROR_TIMEOUT,           "Operation timed out" }
	* };
	* @endcode
	*
	* Pass this array to the constructor of `CErrorManager` to register the errors.
	*
	* @see CErrorManager
	*/
	struct TErrorDefinition
	{
		int m_error_code;
		const char* m_error_description;
	};

	/**
	* @class CErrorManager
	* @brief Manages error codes and their descriptions for a custom application.
	*
	* This class provides functionality to set, query, and list error codes
	* along with their human-readable descriptions. It supports thread-safe
	* setting of the current error code.
	*/
	class CErrorManager
	{
	public:
		/**
		* @brief Constructs an error manager and registers known errors.
		*
		* @param f_errors Pointer to an array of ErrorDefinition structs representing all available errors.
		* @param f_errors_number Number of error definitions in the array.
		* @param f_no_error_item The ErrorDefinition representing the "no error" state.
		*/
		CErrorManager(const TErrorDefinition* f_errors,
					  const int f_errors_number,
					  const TErrorDefinition& f_no_error_item) :
								m_current_error_code(-1),
								m_unknown_error_string("Unknow error"),
								m_no_error_code(0)
		{
			if (f_errors)
			{
				RegisterErrors(f_errors, f_errors_number, f_no_error_item);
			}
		}

		/**
		* @brief Sets the current error code if it exists in the registered errors.
		*
		* Thread-safe operation.
		*
		* @param f_error_code The error code to set as current.
		* @return true if the error code is valid and was set; false otherwise.
		*/
		inline bool SetError(int f_error_code)
		{
			std::lock_guard<std::mutex> l_lock(m_set_error_mutex);
			if (m_errors_map.count(f_error_code))
			{
				m_current_error_code = f_error_code;
				return true;
			}
			return false;
		}

		/**
		* @brief Gets the currently set error code.
		*
		* @return The current error code.
		*/
		inline int GetErrorCode() const
		{
			std::lock_guard<std::mutex> lock(m_set_error_mutex);
			return m_current_error_code;
		}

		/**
		* @brief Checks if an error is currently present.
		*
		* @return true if the current error code is not the "no error" code; false otherwise.
		*/
		inline bool IsErrorPresent() const
		{
			std::lock_guard<std::mutex> lock(m_set_error_mutex);
			if (m_current_error_code != m_no_error_code)
			{
				return true;
			}

			return false;
		}

		/**
		* @brief Retrieves a textual description of the current error.
		*
		* @return A string describing the current error, or "Unknown error code" if invalid.
		*/
		inline const std::string& GetErrorDescription() const
		{
			std::lock_guard<std::mutex> lock(m_set_error_mutex);
			if (m_current_error_code && m_errors_map.count(m_current_error_code))
			{
				return m_errors_map.at(m_current_error_code);
			}
			return m_unknown_error_string;
		}

		/**
		* @brief Lists all registered error codes and their descriptions to standard output.
		*/
		inline void ListAllErrors() const
		{
			{
				for (auto l_err_it = m_errors_map.begin(); l_err_it != m_errors_map.end(); l_err_it++)
				{
					std::cout << "Error Code: " << l_err_it->first << " -> " << l_err_it->second << '\n';
				}
			}
		}

	private:

		/**
		* @brief Registers the array of errors and sets up the no-error definition.
		*
		* @param f_errors Pointer to the array of error definitions.
		* @param f_error_count Number of error definitions.
		* @param f_no_error_definition The no-error error definition.
		*/
		inline void RegisterErrors(const TErrorDefinition* f_errors,
								   int f_error_count,
								   const TErrorDefinition& f_no_error_definition)
		{
			{
				for (int i = 0; i < f_error_count; ++i)
				{
					m_errors_map[f_errors[i].m_error_code] = f_errors[i].m_error_description;
				}
				m_errors_map.insert(std::make_pair(f_no_error_definition.m_error_code,
					f_no_error_definition.m_error_description));

				m_current_error_code = f_no_error_definition.m_error_code;
				m_no_error_code = f_no_error_definition.m_error_code;
			}
		}

		std::unordered_map<int, std::string> m_errors_map;
		int m_current_error_code;
		int m_no_error_code;

		mutable std::mutex m_set_error_mutex;

		std::string m_unknown_error_string;
	};
}

#endif H_ERROR_MANAGER
