#include "alexandria/core/library.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <regex>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "sqlite3.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "dot/graph.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/namespace.h"

namespace
{
    void enableForeignKeyConstraints(sql::Database& db)
    {
        int32_t enabled = 0;
        sqlite3_db_config(db.get(), SQLITE_DBCONFIG_ENABLE_FKEY, 1, &enabled);
        if (!enabled) throw std::runtime_error("Failed to enable foreign key constraints.");

        if (const auto stmt0 = db.createStatement("PRAGMA foreign_keys=ON;", true); !stmt0.step())
            throw std::runtime_error("Failed to enable foreign key constraints.");

        const auto stmt1 = db.createStatement("PRAGMA foreign_keys;", true);
        if (!stmt1.step()) throw std::runtime_error("Failed to enable foreign key constraints.");
        int32_t res = 0;
        stmt1.column(0, res);
        if (res != 1) throw std::runtime_error("Failed to enable foreign key constraints.");
    }
}  // namespace

namespace alex
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Library::Library(sql::DatabasePtr db) :
        database(std::move(db)),
        namespaceTable(database->getTable("namespaces")),
        typeTable(database->getTable("types")),
        propertyTable(database->getTable("properties")),
        genTablesTable(database->getTable("tables")),
        namespaceInsert(namespaceTable.insert().compile()),
        typeInsert(typeTable.insert().compile()),
        genTablesInsert(genTablesTable.insert().compile())
    {
    }

    Library::~Library() noexcept = default;

    ////////////////////////////////////////////////////////////////
    // Static open/create methods.
    ////////////////////////////////////////////////////////////////

    LibraryPtr Library::create(const std::filesystem::path& file)
    {
        if (exists(file)) throw std::runtime_error("Library file already exists");

        auto db = sql::Database::create(file);
        enableForeignKeyConstraints(*db);

        // Create table holding namespace definitions.
        auto& nsTable      = db->createTable("namespaces");
        auto& namespaceCol = nsTable.createColumn("id", sql::Column::Type::Int).primaryKey(true).notNull();
        nsTable.createColumn("name", sql::Column::Type::Text).notNull();
        nsTable.commit();

        // Create table holding type definitions.
        auto& typesTable = db->createTable("types");
        auto& typesCol   = typesTable.createColumn("id", sql::Column::Type::Int).primaryKey(true).notNull();
        typesTable.createColumn("namespace", sql::Column::Type::Int)
          .foreignKey(namespaceCol, sql::ForeignKeyAction::Restrict)
          .notNull();
        typesTable.createColumn("name", sql::Column::Type::Text).notNull().unique();
        typesTable.createColumn("is_instance", sql::Column::Type::Int).notNull();
        typesTable.commit();

        // Create table holding property definitions.
        auto& propsTable = db->createTable("properties");
        propsTable.createColumn("id", sql::Column::Type::Int).primaryKey(true).notNull();
        propsTable.createColumn("type", sql::Column::Type::Int)
          .foreignKey(typesCol, sql::ForeignKeyAction::Restrict)
          .notNull();
        propsTable.createColumn("name", sql::Column::Type::Text);
        propsTable.createColumn("datatype", sql::Column::Type::Text);
        propsTable.createColumn("reference_type", sql::Column::Type::Int).foreignKey(typesCol);
        propsTable.createColumn("is_array", sql::Column::Type::Int);
        propsTable.createColumn("is_blob", sql::Column::Type::Int);
        propsTable.commit();

        // Create table holding generated table names.
        auto& namesTable = db->createTable("tables");
        namesTable.createColumn("id", sql::Column::Type::Int).primaryKey(true).notNull();
        namesTable.createColumn("type", sql::Column::Type::Int)
          .foreignKey(typesCol, sql::ForeignKeyAction::Restrict)
          .notNull();
        namesTable.createColumn("name", sql::Column::Type::Text);
        namesTable.createColumn("kind", sql::Column::Type::Text);
        namesTable.commit();

        return std::make_unique<Library>(std::move(db));
    }

    LibraryPtr Library::open(const std::filesystem::path& file)
    {
        if (!exists(file)) throw std::runtime_error("Library file does not exist");

        auto db = sql::Database::open(file);
        enableForeignKeyConstraints(*db);
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

    NamespaceTable& Library::getNamespaceTable() noexcept { return namespaceTable; }

    const NamespaceTable& Library::getNamespaceTable() const noexcept { return namespaceTable; }

    TypeTable& Library::getTypeTable() noexcept { return typeTable; }

    const TypeTable& Library::getTypeTable() const noexcept { return typeTable; }

    PropertyTable& Library::getPropertyTable() noexcept { return propertyTable; }

    const PropertyTable& Library::getPropertyTable() const noexcept { return propertyTable; }

    TypeTableInsert& Library::getTypeTableInsert() noexcept { return typeInsert; }

    GeneratedTablesTable& Library::getGeneratedTablesTable() noexcept { return genTablesTable; }

    const GeneratedTablesTable& Library::getGeneratedTablesTable() const noexcept { return genTablesTable; }

    GeneratedTablesInsert& Library::getGeneratedTablesInsert() noexcept { return genTablesInsert; }

    ////////////////////////////////////////////////////////////////
    // Namespaces.
    ////////////////////////////////////////////////////////////////

    Namespace& Library::createNamespace(const std::string& name)
    {
        if (namespaces.contains(name))
            throw std::runtime_error(std::format(R"(A namespace with name "{}" already exists.)", name));

        if (const std::regex regex("^[a-z][a-z0-9]*$"); !std::regex_match(name, regex))
            throw std::runtime_error(std::format(
              R"(Cannot create name with name "{}". It does not match the regex "^[a-z][a-z0-9_]*$".)", name));

        try
        {
            auto transaction = database->beginTransaction(sql::Transaction::Type::Deferred);

            // Insert new namespace.
            namespaceInsert(nullptr, sql::toStaticText(name));

            transaction.commit();

            // Create namespace.
            auto space = std::make_unique<Namespace>(*this, database->getLastInsertRowId(), name);
            return *namespaces.emplace(name, std::move(space)).first->second;
        }
        catch (...)
        {
            throw;
        }
    }

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
        std::unordered_map<sql::row_id, Namespace*> namespacemap;
        std::unordered_map<sql::row_id, Type*>      typemap;

        // Read namespaces.
        for (auto select =
               namespaceTable.selectAs<NamespaceRow>().orderBy(ascending(namespaceTable.col<0>())).compile();
             NamespaceRow row : select)
        {
            auto& ns = *namespaces.emplace(row.name, std::make_unique<Namespace>(*this, row.id, std::move(row.name)))
                          .first->second;
            namespacemap.insert({ns.id, &ns});
        }

        // Read types.
        for (auto    select = typeTable.selectAs<TypeRow>().orderBy(ascending(typeTable.col<0>())).compile();
             TypeRow row : select)
        {
            auto& ns = *namespacemap.find(row.nameSpace)->second;
            auto& type =
              *ns.types
                 .emplace(row.name,
                          std::make_unique<Type>(row.id, ns, std::move(row.name), row.isInstance > 0 ? true : false))
                 .first->second;
            typemap.insert({type.id, &type});
        }

        // Read properties.
        for (auto select = propertyTable.selectAs<PropertyRow>().orderBy(ascending(propertyTable.col<0>())).compile();
             PropertyRow row : select)
        {
            DataType dataType;
            fromString(row.dataType, dataType);

            auto& type = *typemap.at(row.type);
            if (dataType == DataType::Reference)
            {
                auto refType = typemap.at(row.referenceType);
                type.addProperty(std::make_unique<Property>(
                  type, row.id, std::move(row.name), dataType, refType, row.isArray, row.isBlob));
            }
            else
            {
                type.addProperty(std::make_unique<Property>(
                  type, row.id, std::move(row.name), dataType, nullptr, row.isArray, row.isBlob));
            }
        }

        // Read generated table names.
        for (auto select = genTablesTable.selectAs<TableRow>().orderBy(ascending(genTablesTable.col<0>())).compile();
             TableRow row : select)
        {
            auto& type = *typemap.find(row.type)->second;
            if (row.kind == "instance")
                type.instanceTable = &database->getTable(row.name);
            else if (row.kind == "blob_array")
                type.blobArrayTables.emplace_back(&database->getTable(row.name));
            else if (row.kind == "primitive_array")
                type.primitiveArrayTables.emplace_back(&database->getTable(row.name));
            else if (row.kind == "reference_array")
                type.referenceArrayTables.emplace_back(&database->getTable(row.name));
        }
    }
}  // namespace alex
