# Code-Cleaner-Java

A small command-line tool written in C++ that removes all comments and empty lines from `.java` files. It was originally written for a school assignment.

## Features

- Removes line comments (`// ...`), block comments (`/* ... */`) and Javadoc (`/** ... */`)
- Removes empty lines and lines that contain only whitespace
- Leaves string literals (`"..."`), character literals (`'...'`) and text blocks (`"""..."""`) untouched, so code like `"https://example.com"` is not broken
- Handles escape sequences such as `\"` and `\\` inside strings
- Processes all `.java` files in the program's folder and all of its subfolders

## Example

Before:

```java
/**
 * Simple example class.
 */
public class Example {

    // Website of the project
    String url = "https://example.com"; /* not a comment: // inside a string */

    int half(int x) {
        return x / 2; // integer division
    }
}
```

After:

```java
public class Example {
    String url = "https://example.com";
    int half(int x) {
        return x / 2;
    }
}
```

## How it works

The code is read character by character and processed by a simple state machine with these states:

| State          | Meaning                                       |
|----------------|-----------------------------------------------|
| `Normal`       | Regular code, copied to the output            |
| `LineComment`  | Inside `// ...`, skipped until the line ends  |
| `BlockComment` | Inside `/* ... */`, skipped                   |
| `BlockStar`    | Saw a `*` inside a block comment, checks for `/` |
| `String`       | Inside `"..."`, copied unchanged              |
| `Char`         | Inside `'...'`, copied unchanged              |
| `TextBlock`    | Inside `"""..."""`, copied unchanged          |

After the comments are removed, a second pass deletes all empty lines. The cleaned code is then written back to the same file.

## Usage

> **Warning: The files are overwritten in place.** Make a backup or commit your code to Git before running the tool.

1. Copy the compiled `.exe` into the folder that contains your Java project.
2. Run the program (double-click or from the command line).
3. Every processed file is listed in the console. Press Enter to close the window.

A precompiled version for Windows is located in `cmake-build-debug`.

## Building

Requirements: Windows, CMake and a C++17 compiler (e.g. MSVC or MinGW).

```bash
cmake -B build
cmake --build build
```

## Limitations

- Windows only, because the program folder is determined with the Windows API (`GetModuleFileNameW`)
- All empty lines are removed, including ones used intentionally for readability
- Files are expected to be ASCII or UTF-8 encoded
