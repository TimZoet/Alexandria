# Geometry

The `geometry` example application contains a Wavefront OBJ importer and exporter. It showcases a few features of `Alexandria`. Note that it is in no way intended to be a complete, robust application. Models are simplified upon import, and you will definitely be able to crash the application when passing in invalid parameters. Extending and improving the application might be a good way to get more familiar with the library.

This example comes with 3 ready to use .obj files inside of the `models` subfolder. Placing them next to the executable would allow you to run the following commands:

```
open geometry.db
Creating new library file geometry.db
import cube.obj
import monkey.obj
import sphere.obj
list
---- Meshes ----
1 cube
2 monkey
3 sphere
---- Materials ----
1 red
2 green
3 blue
subdivide 3
export sphere
Writing mesh sphere to "sphere_0.obj"
Writing material blue to "sphere_0.mtl"
```

## Files

* `main` reads the input and runs the appropriate command.
* `types` defines the C++ data structures to be stored in the database.
* `actions` contains a function for each command.
* `import` and `export` contain the functions that read and write the OBJ models and materials.

## Types

There's a total of 5 types. 3 of these types are just utility types:

```cpp
struct float2
{
    float x;
    float y;
};
struct float3
{
    float x;
    float y;
    float z;
};
struct Vertex
{
    float3 position;
    float3 normal;
    float2 uv;
};
```

The first type we are actually going to be instantiating is the `Material`. It is heavily simplified, having just a `color` and `specular` value.

```cpp
struct Material
{
    alex::InstanceId id;
    std::string      name;
    float3           color;
    float            specular;
};
```

The most important type is the `Mesh`. It has a list of `vertices` which describe a triangulated model. Each triplet of vertices describe a triangle. Additonally, the mesh has a single reference to a `material` which is applied to all triangles.

```cpp
struct Mesh
{
    alex::InstanceId                id;
    std::string                     name;
    alex::Blob<std::vector<Vertex>> vertices;
    alex::Reference<Material>       material;
};
```

## Commands

The `open <path>` command will create or open a library file. If the library did not yet exist, all types are registered. After that, the object handlers for the `Mesh` and `Material` classes are created.

The `import <path>` command will open an OBJ file and load all meshes and materials from it. Multiple objects within a single file are split into separate meshes. Some simplifications to the mesh are made. See the code for more details.

The `list` command will display the names and identifiers of all materials and meshes currently in the database.

The `link <material ID> <mesh ID>` will assign the specified material to the specified mesh.

The `subdivide <mesh ID>` command will do a very basic subdivision of all the triangles of the mesh. Be careful not to go overboard with it, as the amount of data is doubled each iteration. It can be fun to play around with caching settings and subdivision to examine memory usage of the application.

The `export <mesh name>` command exports a mesh to an OBJ file of the same name. When a file with the given name already exists, a unique name with an index is generated. If there are multiple meshes with the same name, they are all exported. 

The `cache <bool>` sets the default caching method of newly encountered instances. The `release` and `clear` commands can be used to . Be sure to read up on this functionality, as the differences between all caching related methods are important.
