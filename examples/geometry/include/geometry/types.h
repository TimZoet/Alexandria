#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

struct float2
{
    float x = 0;
    float y = 0;
};

struct float3
{
    float x = 0;
    float y = 0;
    float z = 0;
};

struct Vertex
{
    float3 position;
    float3 normal;
    float2 uv;
};

struct Material
{
    alex::InstanceId id;
    std::string      name;
    float3           color;
    float            specular;
};

struct Mesh
{
    alex::InstanceId                id;
    std::string                     name;
    alex::Blob<std::vector<Vertex>> vertices;
    alex::Reference<Material>       material;
};