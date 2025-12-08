#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <windows.h>
#include <sstream>
#include <regex>

// -------------------------------------------------------------
// Ordner des Programms ermitteln
// -------------------------------------------------------------

std::filesystem::path getExecutableDir()
{
    wchar_t buffer[MAX_PATH];
    DWORD result = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (result == 0) throw std::runtime_error("EXE-Pfad konnte nicht bestimmt werden.");

    std::filesystem::path exePath(buffer);
    return exePath.parent_path();
}

// -------------------------------------------------------------
// Kommentare entfernen
// -------------------------------------------------------------

std::string removeComments(const std::string& code)
{
    std::string out;
    out.reserve(code.size());

    enum class State { Normal, LineComment, BlockComment, BlockStar };
    State state = State::Normal;

    for (size_t i = 0; i < code.size(); ++i)
    {
        char c = code[i];
        char next = (i + 1 < code.size()) ? code[i + 1] : '\0';

        switch (state)
        {
            case State::Normal:
                if (c == '/' && next == '/')
                {
                    state = State::LineComment;
                    ++i;
                }
                else if (c == '/' && next == '*')
                {
                    state = State::BlockComment;
                    ++i;
                }
                else
                {
                    out.push_back(c);
                }
                break;

            case State::LineComment:
                if (c == '\n')
                {
                    state = State::Normal;
                    out.push_back('\n');
                }
                break;

            case State::BlockComment:
                if (c == '*') state = State::BlockStar;
                break;

            case State::BlockStar:
                if (c == '/')
                {
                    state = State::Normal;
                    if (i + 1 < code.size() && code[i+1] != '\n')
                    {
                        out.push_back('\n');
                        out.push_back('\n');
                    }
                }
                else if (c != '*')
                {
                    state = State::BlockComment;
                }
                break;
        }
    }

    return out;
}

// -------------------------------------------------------------
// Leerzeilen entfernen
// -------------------------------------------------------------

std::string removeEmptyLines(const std::string& input)
{
    std::istringstream iss(input);
    std::ostringstream oss;

    std::string line;
    while (std::getline(iss, line)) {
        bool onlyWhitespace = true;
        for (char c : line) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                onlyWhitespace = false;
                break;
            }
        }

        if (!onlyWhitespace) {
            oss << line << "\n";
        }
    }

    return oss.str();
}

// -------------------------------------------------------------
// Datei verarbeiten
// -------------------------------------------------------------

void processJavaFile(const std::filesystem::path& file)
{
    std::ifstream in(file.string());
    if (!in)
    {
        std::cerr << "Error opening File: " << file << "\n";
        return;
    }

    std::string content((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    in.close();

    std::string finalCode = removeComments(content);
    finalCode = removeEmptyLines(finalCode);

    std::ofstream out(file.string(), std::ios::trunc);
    if (!out)
    {
        std::cerr << "Error in File: " << file << "\n";
        return;
    }

    out << finalCode;
    out.close();

    std::cout << "processed: " << file << "\n";
}

// -------------------------------------------------------------
// Ordner rekursiv durchlaufen
// -------------------------------------------------------------

void processDirectory(const std::filesystem::path& dir)
{
    for (auto& entry : std::filesystem::recursive_directory_iterator(dir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".java")
        {
            processJavaFile(entry.path());
        }
    }
}

// -------------------------------------------------------------
// Hauptfunktion
// -------------------------------------------------------------

int main()
{
    try
    {
        auto base = getExecutableDir();
        std::cout << "Start in Directory:\n" << base << "\n\n";

        processDirectory(base);

        std::cout << "\nDone. Enter to close Programm.";
        std::cin.get();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << "\n";
        std::cin.get();
    }

    return 0;
}
