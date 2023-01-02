#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/core/type_descriptor.h"
#include "alexandria/member_types/blob.h"
#include "alexandria/member_types/reference.h"

////////////////////////////////////////////////////////////////
// Types.
////////////////////////////////////////////////////////////////

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
    float            specular = 0;
};

struct Mesh
{
    alex::InstanceId                id;
    std::string                     name;
    alex::Blob<std::vector<Vertex>> vertices;
    alex::Reference<Material>       material;
};

////////////////////////////////////////////////////////////////
// Utility functions.
////////////////////////////////////////////////////////////////

float2 average(const float2& lhs, const float2& rhs);

float3 average(const float3& lhs, const float3& rhs);

Vertex average(const Vertex& lhs, const Vertex& rhs);

////////////////////////////////////////////////////////////////
// Typedefs.
////////////////////////////////////////////////////////////////

using float2_t = alex::MemberList<alex::Member<&float2::x>, alex::Member<&float2::y>>;
using float3_t = alex::MemberList<alex::Member<&float3::x>, alex::Member<&float3::y>, alex::Member<&float3::z>>;
using vertex_t = alex::MemberList<alex::NestedMember<float3_t, &Vertex::position>,
                                  alex::NestedMember<float3_t, &Vertex::normal>,
                                  alex::NestedMember<float2_t, &Vertex::uv>>;

using MaterialDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Material::id>,
                                                        alex::Member<&Material::name>,
                                                        alex::NestedMember<float3_t, &Material::color>,
                                                        alex::Member<&Material::specular>>;

using MeshDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Mesh::id>,
                                                    alex::Member<&Mesh::name>,
                                                    alex::Member<&Mesh::vertices>,
                                                    alex::Member<&Mesh::material>>;
