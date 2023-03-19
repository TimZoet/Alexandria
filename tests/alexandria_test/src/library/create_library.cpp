#include "alexandria_test/library/create_library.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/core/library.h"

void CreateLibrary::operator()()
{
    // Try all combinations of open, create and openOrCreate with and without existing file.
    const auto file = std::filesystem::current_path() / "library.alex";
    std::filesystem::remove(file);
    expectThrow([&file] { alex::Library::open(file); });
    expectNoThrow([&file] { alex::Library::create(file); });
    expectNoThrow([&file] { alex::Library::open(file); });
    expectThrow([&file] { alex::Library::create(file); });
    expectNoThrow([&file, this] {
        auto [lib, created] = alex::Library::openOrCreate(file);
        compareFalse(created);
    });
    std::filesystem::remove(file);
    expectNoThrow([&file, this] {
        auto [lib, created] = alex::Library::openOrCreate(file);
        compareTrue(created);
    });
    std::filesystem::remove(file);
}
