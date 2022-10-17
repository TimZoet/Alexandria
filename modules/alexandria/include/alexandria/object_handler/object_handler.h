#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cassert>
#include <memory>
#include <unordered_map>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"
#include "cppql-core/database.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/type.h"
#include "alexandria/member_types/member.h"
#include "alexandria/object_handler/blob_array_handler.h"
#include "alexandria/object_handler/primitive_array_handler.h"
#include "alexandria/object_handler/primitive_handler.h"
#include "alexandria/object_handler/reference_array_handler.h"
#include "alexandria/queries/query.h"

namespace alex
{
    //TODO: Move these cache related types to a separate file.
    enum class CacheMethod
    {
        None,
        Weak,
        Strong
    };

    template<typename T>
    struct CacheElement
    {
        CacheElement() = default;

        CacheElement(std::weak_ptr<T> w, std::shared_ptr<T> s) : weakPtr(w), strongPtr(std::move(s)) {}

        CacheElement(const CacheElement&) = delete;

        CacheElement(CacheElement&&) noexcept = default;

        ~CacheElement() = default;

        CacheElement& operator=(CacheElement&&) noexcept = default;

        CacheElement& operator=(const CacheElement&) noexcept = delete;

        void releaseWeak() { weakPtr.reset(); }

        void releaseStrong() { strongPtr.reset(); }

        void releaseAll()
        {
            releaseWeak();
            releaseStrong();
        }

        void set(const std::shared_ptr<T>& instance, CacheMethod c)
        {
            // TODO: Check if instance is not null for weak and strong cache methods.
            switch (c)
            {
            case CacheMethod::None:
                weakPtr.reset();
                strongPtr.reset();
                break;
            case CacheMethod::Weak:
                weakPtr = instance;
                strongPtr.reset();
                break;
            case CacheMethod::Strong:
                if (!strongPtr)
                {
                    weakPtr.reset();
                    strongPtr = instance;
                }
                break;
            }
        }

        std::weak_ptr<T>   weakPtr;
        std::shared_ptr<T> strongPtr;
    };

    template<typename T>
    using Cache = std::unordered_map<InstanceId, CacheElement<T>>;

    // TODO: Constrain members to be Member types of the same class.
    // requires(is_same_class_member_pointer_v<IdMember, Members...>&& is_instance_id_mp<IdMember>)
    template<typename IdMember, typename... Members>
    class ObjectHandler
    {
    public:
        using object_t             = typename IdMember::class_t;
        using blob_array_handler_t = generate_blob_array_handler_t<IdMember, Members...>;
        using prim_array_handler_t = generate_primitive_array_handler_t<IdMember, Members...>;
        using prim_handler_t       = generate_primitive_handler_t<IdMember, Members...>;
        using ref_array_handler_t  = generate_reference_array_handler_t<IdMember, Members...>;

        static constexpr size_t size = sizeof...(Members) + 1;

        ObjectHandler() = delete;

        ObjectHandler(const Type& t, sql::Database& db);

        ObjectHandler(const ObjectHandler&) = delete;

        ObjectHandler(ObjectHandler&&) = delete;

        ObjectHandler& operator=(const ObjectHandler&) = delete;

        ObjectHandler& operator=(ObjectHandler&&) = delete;

        ~ObjectHandler() noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Create.
        ////////////////////////////////////////////////////////////////

        InstanceId create(object_t* instance);

        [[nodiscard]] std::shared_ptr<object_t> create();

        ////////////////////////////////////////////////////////////////
        // Insert.
        ////////////////////////////////////////////////////////////////

        void insert(object_t& instance);

        void insert(std::shared_ptr<object_t> instance);

        ////////////////////////////////////////////////////////////////
        // Update.
        ////////////////////////////////////////////////////////////////

        void update(const object_t& instance);

        void update(std::shared_ptr<object_t> instance);

        ////////////////////////////////////////////////////////////////
        // Get.
        ////////////////////////////////////////////////////////////////

        void get(InstanceId id, object_t& instance);

        [[nodiscard]] std::shared_ptr<object_t> get(InstanceId id);

        [[nodiscard]] bool exists(InstanceId id);

        [[nodiscard]] std::vector<InstanceId> list();

        ////////////////////////////////////////////////////////////////
        // Delete.
        ////////////////////////////////////////////////////////////////

        void del(InstanceId id);

        ////////////////////////////////////////////////////////////////
        // Caching.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the default cache method.
         * \return Default cache method.
         */
        [[nodiscard]] CacheMethod getDefaultCacheMethod() const noexcept;

        /**
         * \brief Set the default cache method that is used the first time an instance is created, retrieved, modified etc. through any of the managed method.
         * \param c Default cache method.
         */
        void setDefaultCacheMethod(CacheMethod c) noexcept;

        /**
         * \brief Get the cache method of the specified instance.
         * \param id Instance identifier.
         * \return Cache method.
         */
        [[nodiscard]] CacheMethod getCacheMethod(InstanceId id) const;

        /**
         * \brief Set the cache method of the specified instance.
         * \param instance Instance. If CacheMethod is None, pointer is allowed to be null.
         * \param c Cache method.
         */
        void setCacheMethod(const std::shared_ptr<object_t>& instance, CacheMethod c);

        /**
         * \brief Get the entire cache.
         * \return Cache.
         */
        [[nodiscard]] const Cache<object_t>& getCache() const noexcept;

        /**
         * \brief Remove the cache entry for the specified instance. Calling any managed method for this instance will result in the recreation of a cache entry with the default cache method.
         * \param id Instance identifier.
         * \return True if entry was removed, false if no entry existed.
         */
        bool clearCache(InstanceId id);

        /**
         * \brief Reset all non-owning pointers.
         */
        void releaseWeak();

        /**
         * \brief Reset all owning pointers.
         */
        void releaseStrong();

        /**
         * \brief Reset all non-owning and owning pointers. Equivalent of calling both releaseWeak and releaseStrong.
         */
        void releaseAll();

        /**
         * \brief Clear the entire cache. Calling any managed method will result in the (re)creation of cache entries with the default cache method.
         */
        void clearAll();

        ////////////////////////////////////////////////////////////////
        // Find.
        ////////////////////////////////////////////////////////////////

        template<size_t I>
        auto getPrimitiveColumn()
        {
            // TODO: This method should be removed once getCol has been finalized.
            return primHandler.getTable().template col<I>();
        }

        template<typename F>
        auto find(F&& filter)
        {
            return Query(*this, std::forward<F>(filter));
        }

        template<typename M>
        auto getCol()
        {
            // TODO: Try to use constraints to improve compile time error message when member does not exist.
            return primHandler.getTable().template col<prim_handler_t::template getIndex<M>()>();
        }

        prim_handler_t& getPrimitiveHandler() noexcept { return primHandler; }

        const prim_handler_t& getPrimitiveHandler() const noexcept { return primHandler; }

    private:
        void createDefaultCacheElement(const std::shared_ptr<object_t>& instance);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        const Type*          type;
        sql::Database*       database;
        blob_array_handler_t blobArrayHandler;
        prim_array_handler_t primArrayHandler;
        prim_handler_t       primHandler;
        ref_array_handler_t  refArrayHandler;
        Cache<object_t>      objectCache;
        CacheMethod          defaultCacheMethod = CacheMethod::Weak;
    };

    ////////////////////////////////////////////////////////////////
    // Implementation.
    ////////////////////////////////////////////////////////////////

    // Not entirely sure why the requires needs to be added to all implementations. GCC does not complain.
    // See https://godbolt.org/z/8nhYKTPMo

    template<typename IdMember, typename... Members>
    ObjectHandler<IdMember, Members...>::ObjectHandler(const Type& t, sql::Database& db) :
        type(&t),
        database(&db),
        blobArrayHandler(t, db),
        primArrayHandler(t, db),
        primHandler(t, db),
        refArrayHandler(t, db)
    {
    }

    ////////////////////////////////////////////////////////////////
    // Create.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::create(object_t* instance) -> InstanceId
    {
        InstanceId id;
        // Create a new instance with default values. If pointer is not null, immediately retrieve it.
        if (instance)
        {
            if (IdMember::template get(*instance).valid()) throw std::runtime_error("");
            id = primHandler.create();
            get(id, *instance);
        }
        else
            id = primHandler.create();

        return id;
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::create() -> std::shared_ptr<object_t>
    {
        // Create a new instance with default values and immediately retrieve it.
        return get(primHandler.create());
    }

    ////////////////////////////////////////////////////////////////
    // Insert.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::insert(object_t& instance) -> void
    {
        // Cannot insert an object that already has a valid ID.
        if (IdMember::template get(instance).valid()) throw std::runtime_error("Instance was already inserted");

        // Insert main data.
        primHandler.insert(instance);

        // Get row ID.
        IdMember::template get(instance) = database->getLastInsertRowId();

        // Insert blob arrays, primitive arrays, reference arrays.
        blobArrayHandler.insert(instance);
        primArrayHandler.insert(instance);
        refArrayHandler.insert(instance);
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::insert(std::shared_ptr<object_t> instance) -> void
    {
        if (!instance) throw std::runtime_error("");

        // Insert instance.
        insert(*instance);

        // Create CacheElement.
        createDefaultCacheElement(std::move(instance));
    }

    ////////////////////////////////////////////////////////////////
    // Update.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::update(const object_t& instance) -> void
    {
        if (!IdMember::template get(instance).valid()) throw std::runtime_error("");

        // Update.
        primHandler.update(instance);
        blobArrayHandler.update(instance);
        primArrayHandler.update(instance);
        refArrayHandler.update(instance);
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::update(std::shared_ptr<object_t> instance) -> void
    {
        if (!instance) throw std::runtime_error("");

        // Update instance.
        update(*instance);

        // Create CacheElement if it does not exist.
        if (!objectCache.contains(IdMember::template get(*instance))) createDefaultCacheElement(std::move(instance));
    }

    ////////////////////////////////////////////////////////////////
    // Get.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::get(InstanceId id, object_t& instance) -> void
    {
        if (IdMember::template get(instance).valid()) throw std::runtime_error("");

        // Retrieve main data.
        primHandler.get(id, instance);

        // Retrieve blob arrays, primitive arrays, reference arrays.
        blobArrayHandler.get(id, instance);
        primArrayHandler.get(id, instance);
        refArrayHandler.get(id, instance);
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::get(InstanceId id) -> std::shared_ptr<object_t>
    {
        // Look for instance in cache.
        auto it = objectCache.find(id);
        if (it != objectCache.end())
        {
            auto& cacheElement = it->second;
            if (cacheElement.strongPtr) return cacheElement.strongPtr;
            if (auto ptr = cacheElement.weakPtr.lock()) return ptr;
        }

        // Create empty instance.
        auto instance = std::make_shared<object_t>();

        // Retrieve instance.
        get(id, *instance);

        // Create CacheElement.
        if (it == objectCache.end()) createDefaultCacheElement(instance);

        return instance;
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::exists(InstanceId id) -> bool
    {
        // TODO: First use cache? Can cause problems in other methods, e.g. setCache, which tries to find out if the object actually exists on disk.
        // Query primitiveHandler to see if object exists.
        return primHandler.exists(id);
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::list() -> std::vector<InstanceId>
    {
        // Query primitiveHandler for list of IDs.
        return primHandler.list();
    }

    ////////////////////////////////////////////////////////////////
    // Delete.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::del(InstanceId id) -> void
    {
        // Call handlers to remove instance from all tables.
        primHandler.del(id);
        blobArrayHandler.del(id);
        primArrayHandler.del(id);
        refArrayHandler.del(id);

        // Remove all references to this instance in other tables.
        for (const auto* prop : type->getReferences()) { prop->deleteReferences(id); }

        // TODO: Should the ID of the cached instance be reset here?
        // Delete CacheElement if it exists.
        auto it = objectCache.find(id);
        if (it != objectCache.end()) objectCache.erase(it);
    }

    ////////////////////////////////////////////////////////////////
    // Caching.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::getDefaultCacheMethod() const noexcept -> CacheMethod
    {
        return defaultCacheMethod;
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::setDefaultCacheMethod(CacheMethod c) noexcept -> void
    {
        defaultCacheMethod = c;
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::getCacheMethod(InstanceId id) const -> CacheMethod
    {
        const auto it = objectCache.find(id);
        if (it == objectCache.end()) return CacheMethod::None;

        if (it->second.strongPtr) return CacheMethod::Strong;
        if (it->second.weakPtr.lock()) return CacheMethod::Weak;

        return CacheMethod::None;
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::setCacheMethod(const std::shared_ptr<object_t>& instance, CacheMethod c)
      -> void
    {
        if (!IdMember::template get(instance).valid()) throw std::runtime_error("");

        // Look for existing CacheElement.
        const auto it = objectCache.find(IdMember::template get(*instance));

        // No CacheElement yet.
        if (it == objectCache.end())
        {
            // TODO: Check if instance actually exists in database? Also when clearing cache?
            if (!exists(IdMember::template get(*instance))) throw std::runtime_error("");

            // Create new CacheElement.
            CacheElement<object_t> elem;
            elem.set(instance, c);
            objectCache[IdMember::template get(*instance)] = std::move(elem);
        }
        // Update existing CacheElement.
        else
            it->second.set(instance, c);
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::clearCache(InstanceId id) -> bool
    {
        const auto it = objectCache.find(id);
        if (it != objectCache.end())
        {
            objectCache.erase(it);
            return true;
        }
        return false;
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::getCache() const noexcept -> const Cache<object_t>&
    {
        return objectCache;
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::releaseWeak() -> void
    {
        std::for_each(objectCache.begin(), objectCache.end(), [](auto& elem) { elem.second.releaseWeak(); });
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::releaseStrong() -> void
    {
        std::for_each(objectCache.begin(), objectCache.end(), [](auto& elem) { elem.second.releaseStrong(); });
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::releaseAll() -> void
    {
        std::for_each(objectCache.begin(), objectCache.end(), [](auto& elem) { elem.second.releaseAll(); });
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::clearAll() -> void
    {
        objectCache.clear();
    }

    template<typename IdMember, typename... Members>
    auto ObjectHandler<IdMember, Members...>::createDefaultCacheElement(const std::shared_ptr<object_t>& instance)
      -> void
    {
        assert(!objectCache.contains(IdMember::template get(*instance)));

        // Create CacheElement with default cache method.
        CacheElement<object_t> elem;
        elem.set(instance, defaultCacheMethod);
        objectCache[IdMember::template get(*instance)] = std::move(elem);
    }

}  // namespace alex