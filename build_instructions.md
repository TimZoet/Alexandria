# Build Instructions

## Getting The Code

To retrieve the code from GitHub:

```cmd
git clone https://github.com/TimZoet/Alexandria.git source
```

## Exporting to Conan

To export the `alexandria` package to your local Conan cache:

```cmd
conan export --user timzoet --channel v1.0.0 source
```

Make sure to update the channel when the version is different.

## Building Tests

Invoke `conan install`:

```cmd
conan install -pr:h=source/buildtools/profiles/alexandria-test-vs2022-release -pr:b=source/buildtools/profiles/alexandria-test-vs2022-release -s build_type=Release --build=missing -of=build source
```

Then generate and build with CMake:

```cmd
cmake -S source -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:FILEPATH=conan_toolchain.cmake
cmake --build build --config Release
```

Finally, run the test application:

```cmd
cd build/bin/tests
.\alexandria_test
```

## Building Examples

Invoke `conan install`:

```cmd
conan install -pr:h=source/buildtools/profiles/alexandria-package-vs2022-release -pr:b=source/buildtools/profiles/alexandria-package-vs2022-release -s build_type=Release --build=missing -of=build -o build_examples=True source
```

Then generate and build with CMake:

```cmd
cmake -S source -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE:FILEPATH=conan_toolchain.cmake
cmake --build build --config Release
```

Finally, run the example application:

```cmd
cd build/bin
.\geometry
```
