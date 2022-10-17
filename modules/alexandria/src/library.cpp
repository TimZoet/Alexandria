#include "alexandria/library.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/column.h"
#include "cppql-core/statement.h"
#include "cppql-core/table.h"
#include "cppql-typed/queries/insert.h"
#include "cppql-typed/queries/select.h"
#include "dot/graph.h"

using namespace std::string_literals;

namespace alex
{
    Library::Library(sql::DatabasePtr db) :
        database(std::move(db)),
        typeTable(database->getTable("Types"s)),
        propertyTable(database->getTable("Properties"s)),
        memberTable(database->getTable("Members"s))
    {
    }

    Library::~Library() = default;

    ////////////////////////////////////////////////////////////////
    // Static open/create methods.
    ////////////////////////////////////////////////////////////////

    LibraryPtr Library::create(const std::filesystem::path& file)
    {
        if (exists(file)) throw std::runtime_error("Library file already exists");

        auto db = sql::Database::create(file);

        // Create Types table.
        auto& typesTable = db->createTable("Types"s);
        auto& typesID    = typesTable.createColumn("ID", sql::Column::Type::Int)
                          .setPrimaryKey(true)
                          .setNotNull(true)
                          .setAutoIncrement(true);
        typesTable.createColumn("Name"s, sql::Column::Type::Text).setNotNull(true);
        typesTable.commit();

        // Create Properties table.
        auto& propsTable = db->createTable("Properties"s);
        auto& propsID    = propsTable.createColumn("ID", sql::Column::Type::Int)
                          .setPrimaryKey(true)
                          .setNotNull(true)
                          .setAutoIncrement(true);
        propsTable.createColumn("Name"s, sql::Column::Type::Text);
        propsTable.createColumn("DataType"s, sql::Column::Type::Text);
        propsTable.createColumn("Type"s, sql::Column::Type::Int).setForeignKey(typesID);
        propsTable.createColumn("IsReference"s, sql::Column::Type::Int);
        propsTable.createColumn("IsArray"s, sql::Column::Type::Int);
        propsTable.createColumn("IsBlob"s, sql::Column::Type::Int);
        propsTable.createColumn("DefaultValue"s, sql::Column::Type::Text);
        propsTable.commit();

        // Create Members table.
        auto& membersTable = db->createTable("Members"s);
        membersTable.createColumn("ID", sql::Column::Type::Int)
          .setPrimaryKey(true)
          .setNotNull(true)
          .setAutoIncrement(true);
        membersTable.createColumn("Type"s, sql::Column::Type::Int).setForeignKey(typesID);
        membersTable.createColumn("Property"s, sql::Column::Type::Int).setForeignKey(propsID);
        membersTable.commit();

        // Create and return library.
        return std::make_unique<Library>(std::move(db));
    }

    LibraryPtr Library::open(const std::filesystem::path& file)
    {
        if (!exists(file)) throw std::runtime_error("Library file does not exist");

        auto db  = sql::Database::open(file);
        auto lib = std::make_unique<Library>(std::move(db));
        lib->readSpecification();
        return lib;
    }

    std::pair<LibraryPtr, bool> Library::openOrCreate(const std::filesystem::path& file)
    {
        return exists(file) ? std::make_pair(open(file), false) : std::make_pair(create(file), true);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    sql::Database& Library::getDatabase() noexcept { return *database; }

    const sql::Database& Library::getDatabase() const noexcept { return *database; }

    Type& Library::getType(const std::string& name)
    {
        const auto it = types.find(name);
        if (it == types.end()) throw std::runtime_error("Type does not exist");
        return *it->second;
    }

    const Type& Library::getType(const std::string& name) const
    {
        const auto it = types.find(name);
        if (it == types.end()) throw std::runtime_error("Type does not exist");
        return *it->second;
    }

    TypeTable& Library::getTypeTable() noexcept { return typeTable; }

    const TypeTable& Library::getTypeTable() const noexcept { return typeTable; }

    PropertyTable& Library::getPropertyTable() noexcept { return propertyTable; }

    const PropertyTable& Library::getPropertyTable() const noexcept { return propertyTable; }

    MemberTable& Library::getMemberTable() noexcept { return memberTable; }

    const MemberTable& Library::getMemberTable() const noexcept { return memberTable; }

    ////////////////////////////////////////////////////////////////
    // Types.
    ////////////////////////////////////////////////////////////////

    Type& Library::createType(const std::string& name)
    {
        if (types.contains(name)) throw std::runtime_error("A type with this name already exists");
        // Also check for other forbidden names:
        // Types, Properties, Members built-in tables.
        // Table names forbidden by sqlite.
        // Existing generated tables.
        // Type names such as float, double, int32 etc.
        // ...

        // Create type.
        auto type = std::make_unique<Type>(name);
        return *types.emplace(name, std::move(type)).first->second;
    }

    void Library::commitTypes()
    {
        for (auto& [typeName, type] : types)
        {
            // Skip already committed types.
            if (type->isCommitted()) continue;

            commitType(*type);
        }
    }

    void Library::commitType(Type& type) { type.commit(*database, typeTable, propertyTable, memberTable); }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void Library::writeGraph(std::ostream& out) const
    {
        static_cast<void>(out);
        static_cast<void>(this);
#if 0
        dot::Graph graph;

        // Create type nodes.
        std::unordered_map<uint64_t, size_t> typeNodes;
        for (const auto& [name, type] : types)
        {
            auto& node = graph.createNode();
            node.setLabel(name);
            node.setShape("circle"s);

            typeNodes[type->getId()] = node.getIndex();
        }

        // Create property nodes.
        std::unordered_map<uint64_t, size_t> propertyNodes;
        for (const auto& [name, prop] : properties)
        {
            auto& node = graph.createNode();

            auto typeAnnotation = ": "s + toString(prop->getDataType());

            if (prop->isReference() && prop->isArray())
                typeAnnotation += "[]"s;
            else if (prop->isReference())
                typeAnnotation += "&"s;
            else if (prop->isBlob() && prop->isArray())
                typeAnnotation += "*"s;

            node.setLabel(name + typeAnnotation);
            node.setShape("box"s);

            propertyNodes[prop->getId()] = node.getIndex();
        }

        // Create edges from types to properties.
        for (const auto& [tname, type] : types)
        {
            for (const auto& prop : type->getProperties())
            {
                const auto& typeNode = graph.getNodes()[typeNodes[type->getId()]];
                const auto& propNode = graph.getNodes()[propertyNodes[prop->getId()]];
                auto&       edge     = graph.createEdge(typeNode, propNode);
                if (prop->isReference()) edge.attributes["style"] = "dashed";
            }
        }

        // Create edges from properties to types.
        for (const auto& [name, prop] : properties)
        {
            if (prop->getDataType() == DataType::Type)
            {
                const auto& propNode = graph.getNodes()[propertyNodes[prop->getId()]];
                const auto& typeNode = graph.getNodes()[typeNodes[prop->getType()->getId()]];
                graph.createEdge(propNode, typeNode);
            }
        }

        graph.write(out);
#endif
    }

    void Library::readSpecification()
    {
        // TODO: Order by ID ascending.

        // Keep map from ID to Type and Property to resolve members.
        std::unordered_map<sql::row_id, Type*>       typemap;
        std::unordered_map<sql::row_id, PropertyPtr> propsmap;

        // Read types.
        {
            auto select = typeTable.selectAll();
            for (auto row : select)
            {
                // Get values.
                const auto& id   = std::get<0>(row);
                const auto& name = std::get<1>(row);

                // Create type.
                auto type   = std::make_unique<Type>(name);
                typemap[id] = type.get();
                types.emplace(name, std::move(type));
            }
        }

        // Read properties.
        {
            auto select = propertyTable.selectAll();
            for (auto row : select)
            {
                // Get values.
                const auto& id   = std::get<0>(row);
                const auto& name = std::get<1>(row);
                DataType    dataType;
                fromString(std::get<2>(row), dataType);
                Type* type = nullptr;
                if (dataType == DataType::Type) type = typemap[std::get<3>(row)];
                const bool reference = std::get<4>(row) == 1 ? true : false;
                const bool array     = std::get<5>(row) == 1 ? true : false;
                const bool blob      = std::get<6>(row) == 1 ? true : false;

                // Create property.
                auto prop = type ? std::make_unique<Property>(id, name, *type, reference, array, blob) :
                                   std::make_unique<Property>(id, name, dataType, reference, array, blob);
                // TODO: Set default value by parsing string based on dataType.
                propsmap[id] = std::move(prop);
            }
        }

        // Read and resolve type members.
        {
            auto select = memberTable.selectAll<MemberTableTypeIndex, MemberTablePropertyIndex>();
            for (auto row : select)
            {
                // Get columns.
                const auto& typeId = std::get<0>(row);
                const auto& propId = std::get<1>(row);

                // Add property to type.
                typemap[typeId]->addProperty(std::move(propsmap[propId]));
            }
        }

        // Assign IDs to types, marking them as committed.
        for (auto& [id, type] : typemap) type->id = id;

        // Resolve references.
        for (auto& [name, type] : types) type->resolve(*database, database->getTable(type->getName()), "");
    }
}  // namespace alex