# GErrorManagerLib

A lightweight, single-header C++ error manager for small applications. You
define your application's error codes and descriptions once in a table; the
error manager then keeps track of the current error, validates the codes you
set, and gives you the matching description.

No dependencies beyond the standard library.

> **Project status:** this library is at version 0.0.1, an early release. It
> is still being tested and will be further developed, so its API may change
> in future versions. Feedback and bug reports are welcome.
>
> This library has been tested only on Windows and compiled with MSVC 2022 so far

## Features

- **One table of errors**: all error codes and their descriptions are defined
  in one place and registered when the manager is created.
- **Validated codes**: setting an error code that was not registered is
  rejected.
- **Descriptions on demand**: get the human-readable description of the
  current error, e.g. to show it in a message box or write it to a log.
- **Thread-safe**: setting and reading the current error is protected by a
  mutex, so worker threads can report errors to a shared manager.
- **Header-only**: copy one file into your project and include it.

## Getting started

Copy `GErrorManagerLib.h` into your project and include it:

```cpp
#include "GErrorManagerLib.h"

using namespace gerrormanagerlib;
```

Requires C++11 or later.

## Example

### 1. Define your error codes and descriptions

```cpp
enum EAppError
{
    ERR_NONE             = 0,
    ERR_FILE_NOT_FOUND   = 1,
    ERR_INVALID_ARGUMENT = 2,
    ERR_TIMEOUT          = 3
};

static const TErrorDefinition g_app_errors[] =
{
    { ERR_FILE_NOT_FOUND,   "File not found" },
    { ERR_INVALID_ARGUMENT, "Invalid argument" },
    { ERR_TIMEOUT,          "Operation timed out" }
};
```

### 2. Create the error manager

The constructor takes the error table, its size, and the definition of the
"no error" state:

```cpp
CErrorManager error_manager(g_app_errors,
                            sizeof(g_app_errors) / sizeof(g_app_errors[0]),
                            { ERR_NONE, "No error" });
```

The manager starts in the "no error" state.

### 3. Set and query errors

```cpp
bool LoadFile(const std::string& path, CErrorManager& error_manager)
{
    std::ifstream file(path);
    if (!file)
    {
        error_manager.SetError(ERR_FILE_NOT_FOUND);
        return false;
    }
    // ...
    return true;
}

if (!LoadFile("data.txt", error_manager))
{
    std::cerr << "Error " << error_manager.GetErrorCode() << ": "
              << error_manager.GetErrorDescription() << '\n';
    // Error 1: File not found
}

// Codes that were not registered are rejected
bool ok = error_manager.SetError(42);   // false, current error unchanged

// Clear the error by setting the "no error" code
error_manager.SetError(ERR_NONE);
bool has_error = error_manager.IsErrorPresent();   // false
```

## API overview

| Member                                   | Description                                                                                                   |
| ---------------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| `TErrorDefinition`                       | One entry of the error table: an integer code and a description.                                              |
| `CErrorManager(errors, count, no_error)` | Registers the error table and the "no error" definition; the current error starts as "no error".              |
| `SetError(code)`                         | Sets the current error. Returns `false`, leaving the current error unchanged, if the code was not registered. |
| `GetErrorCode()`                         | Returns the current error code.                                                                               |
| `IsErrorPresent()`                       | Returns `true` if the current error is not the "no error" code.                                               |
| `GetErrorDescription()`                  | Returns the description of the current error.                                                                 |
| `ListAllErrors()`                        | Prints all registered codes and descriptions to standard output.                                              |

## Notes

- **Error codes**: use any integers, as long as each code is unique and differs
  from the "no error" code. Using 0 for "no error" and positive numbers for
  errors is the usual convention.
- **One current error**: the manager stores only the most recent error;
  setting a new error replaces the previous one.
- **Clearing the error**: there is no separate reset function; set the
  "no error" code to return to the "no error" state.
- **Error table lifetime**: descriptions are copied when the manager is
  created, so the error table does not need to outlive the manager.
- **Listing order**: `ListAllErrors()` prints the errors in no particular
  order.

## License

Released under the [Boost Software License 1.0](LICENSE).

SPDX: `BSL-1.0`
