#include "alexandria_test/create_library.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

void CreateLibrary::operator()()
{
    // Try all combinations of open, create and openOrCreate with and without existing file.
    const auto file = std::filesystem::current_path() / "library.alex";
    std::filesystem::remove(file);
    expectThrow([&file]() { alex::Library::open(file); });
    expectNoThrow([&file]() { alex::Library::create(file); });
    expectNoThrow([&file]() { alex::Library::open(file); });
    expectThrow([&file]() { alex::Library::create(file); });
    expectNoThrow([&file]() { alex::Library::openOrCreate(file); });
    std::filesystem::remove(file);
    expectNoThrow([&file]() { alex::Library::openOrCreate(file); });
    std::filesystem::remove(file);
}
