#ifndef ECS_H
#define ECS_H

// COMPONENT
// Basic component class
#include <cstddef>
#include <cctype>
#include <typeinfo>

typedef unsigned int ComponentType;
typedef size_t EntityId;

static inline const char* demangle(const char* str)
{
    const char *res = str;
    size_t i = 0;
    while(isdigit(str[i]) && str[i]) {
        res++;
        i++;
    }

    return res;
}

class ComponentCounter {
public:
    static ComponentType getNextType()
    {
        static ComponentType nextComponentType = 0;
        return nextComponentType++;
    }
};

template <class T> class Component
{
public:
    Component(EntityId id = 0) : m_id(id) {}

    static ComponentType type() { return T::m_type; }

    static const char* name() { return demangle(typeid(T).name()); }

    EntityId id() { return m_id; }

    bool isValid() { return m_id > 0; }

private:
    const static ComponentType m_type;
    EntityId m_id;
};

template < class T > const ComponentType Component<T>::m_type = ComponentCounter::getNextType();


// EVENT
// Basic Event classes
#include <unordered_set>
#include <queue>
#include <memory>
#include <map>
#include <mutex>
#include <iostream>
#include <thread>
#include <condition_variable>

typedef unsigned int EventType;

class EventCounter {
public:
    static EventType getNextType()
    {
        static EventType nextEventType = 0;
        return nextEventType++;
    }
};

class BaseEvent
{
public:
    virtual ~BaseEvent() = default;

    virtual EventType getType() = 0;
};

template <class T> class Event : public BaseEvent
{
public:
    static EventType type() { return T::m_type; }

    static const std::string name() { return demangle(typeid(T).name()); }

    EventType getType() { return type(); }

private:
    const static EventType m_type;
};

template < class T > const EventType Event<T>::m_type = EventCounter::getNextType();

typedef std::queue<std::shared_ptr<BaseEvent>> EventQueue;

class EventListener
{
public:
    EventQueue &events() { return m_events; }
private:
    EventQueue m_events;
};

typedef std::map<EventType, std::map<unsigned int, EventListener*>> Subscriptions;

class EventThread
{
public:
    EventThread() : m_running(true), m_thread(&EventThread::run, this) {}
    ~EventThread()
    {
        m_running = false;
        cv.notify_one();
        if(m_thread.joinable())
            m_thread.join();
    }

    void pushEvent(BaseEvent* e)
    {
        {
            std::lock_guard<std::mutex> lck(mtx);
            m_events.push(e);
        }
        cv.notify_one();
    }

    template<class T>
    void addSubscription(unsigned int listenerId, EventListener* listener)
    {
        EventType type(T::type());
        m_subscriptions[type].insert(std::pair<unsigned int, EventListener*>(listenerId, listener));
    }

    void removeSubscription(EventType type, unsigned int listenerId)
    {
       m_subscriptions[type].erase(listenerId);
    }

    void removeAllSubscriptions(unsigned int listenerId)
    {
        for(auto &pair : m_subscriptions)
            pair.second.erase(listenerId);
    }

    void clear()
    {
        m_subscriptions.clear();
    }

private:
    bool m_running;

    std::thread m_thread;
    std::mutex mtx;
    std::condition_variable cv;

    Subscriptions m_subscriptions;
    std::queue<BaseEvent*> m_events;

    void run()
    {

        while(m_running) {
            {
                std::unique_lock<std::mutex> lk(mtx);
                cv.wait(lk, [this] { return !m_events.empty() || !m_running; });
            }

            while(!m_events.empty()) {
                std::lock_guard<std::mutex> lck(mtx);
                BaseEvent *event = m_events.front();
                m_events.pop();

                EventType type(event->getType());
                std::shared_ptr<BaseEvent> ptr(event);

                for(auto &pair : m_subscriptions[type])
                    pair.second->events().push(ptr);
            }
        }
    }
};

template <typename T> struct StaticStorage
{
public:
    static T& get() { return m_static; }

private:
    static T m_static;
};

template <typename T> T StaticStorage<T>::m_static;

class EventDispatcher : public StaticStorage<EventThread> {};

class EventProducer
{
public:
    template<class T>
    static void publishEvent(T* event)
    {
       EventDispatcher::get().pushEvent(event);
    }
};

// SYSTEM
// Basic system classes
typedef unsigned int SystemType;

class SystemCounter {
public:
    static SystemType getNextType()
    {
        static SystemType nextSystemType = 0;
        return nextSystemType++;
    }
};

class BaseSystem : public EventListener, public EventProducer
{
public:
    virtual ~BaseSystem() = default;

    virtual void update(float dt) = 0;

    virtual void processEvents() = 0;
};

template < class T > class System : public BaseSystem
{
public:
    ~System() { EventDispatcher::get().removeAllSubscriptions(type()); }

    static SystemType type() { return T::m_type; }

    static const char* name() { return demangle(typeid(T).name()); }

    virtual void update(float dt) {}

    virtual void handleEvent(BaseEvent*) {}

    void processEvents()
    {
        while(!events().empty()) {
            BaseEvent *event = events().front().get();
            handleEvent(event);
            events().pop();
        }
    }

    template<class... Args> void subscribeTo() { subscribeToEvent<Args...>(); }

private:
    const static SystemType m_type;

    template<class C>
    void subscribeToEvent() { EventDispatcher::get().addSubscription<C>(type(), this); }

#if 0
template<class C, class ... Args> // >=1 template parameters -- ambiguity!
void subscribeToEvent() { EventDispatcher::addSubscription<C>(type(), this); }
#endif

    template<class T1, class T2, class ...Args>
    void subscribeToEvent()
    {
        subscribeToEvent<T1>();
        subscribeToEvent<T2, Args...>();
    }
};

template <class T> const SystemType System<T>::m_type = SystemCounter::getNextType();


// CORE EVENTS
// Events produced in the Container class
template<class T> class ItemCreated : public Event<ItemCreated<T>>
{
public:
    ItemCreated(const T& _item) : item(_item) {}
    T item;
};

template<class T> class ItemDeleted : public Event<ItemDeleted<T>>
{
public:
    ItemDeleted(const T& _item) : item(_item) {}
    T item;
};


// CONTAINER
// Flexible container wrapping std::vector
#include <vector>

class BaseContainer
{
public:
    BaseContainer() = default;
    virtual ~BaseContainer() = default;

    virtual size_t size() = 0;

    virtual void clear() = 0;

    virtual void removeItem(size_t id) = 0;
};

template<class T>
class Container : public BaseContainer, public EventProducer
{
public:
    Container() { clear(); }
    ~Container() { clear(); }

    size_t size() { return m_items.size(); }

    T& operator [](size_t i) const { return m_items[i]; }

    T& operator [](size_t i) { return m_items[i]; }

    T& item(size_t index) { return m_items[index]; }

    std::vector<T>& items() { return m_items; }

    size_t freeIndex() { return m_freeIndex.front(); }

    size_t addItem(const T &item)
    {
        std::lock_guard<std::mutex> lock(m_lock);

        size_t itemIndex = m_freeIndex.front();

        if(itemIndex == m_items.size())
            m_items.push_back(item);
        else
            m_items[itemIndex] = item;

        m_freeIndex.pop();
        if(m_freeIndex.empty())
            m_freeIndex.push(m_items.size());

        publishEvent(new ItemCreated<T>(item));

        return itemIndex;
    }

    void removeItem(size_t index) override
    {
        std::lock_guard<std::mutex> lock(m_lock);

        T item = m_items[index];

        if(index == m_items.size() - 1)
            m_items.pop_back();
        else
            m_items[index] = T();

        m_freeIndex.push(index);

        publishEvent(new ItemDeleted<T>(item));
    }

    void clear()
    {
        m_items.clear();
        m_items.push_back(T());

        std::queue<size_t> empty;
        std::swap( m_freeIndex, empty );
        m_freeIndex.push(1);
    }

private:
    std::vector<T> m_items;
    std::queue<size_t> m_freeIndex;
    std::mutex m_lock;
};


// SIGNATURE TREE
// Store entities ID based on their component signature
#include <boost/container/flat_set.hpp>
#include <set>

typedef std::set<unsigned int> Signature;

struct SignatureNode{
    boost::container::flat_set<size_t> items;
    std::map<unsigned int, SignatureNode> children;
};

class SignatureTree
{
public:
    void addToSignature(size_t id, Signature signature, unsigned int component)
    {
        for(unsigned int i : signature) {
            addRecursive(&(root.children[i]), id, component, signature, i);
            signature.erase(signature.begin());
        }
    }

    void removeFromSignature(size_t id, Signature signature, unsigned int component)
    {
        for(unsigned int i : signature) {
            removeRecursive(&(root.children[i]), id, component, signature, i);
            signature.erase(signature.begin());
        }
    }

    void removeAll(size_t id, Signature signature)
    {
        for(unsigned int i : signature) {
            removeAllRecursive(&(root.children[i]), id, signature, i);
            signature.erase(signature.begin());
        }
    }

    boost::container::flat_set<size_t>& itemsMatchingSignature(const Signature &signature)
    {
        SignatureNode *node = &root;

        for(unsigned int type : signature)
            node = &(node->children[type]);

        return node->items;
    }

private:
    SignatureNode root;

    void addRecursive(SignatureNode *node, size_t id, unsigned int component, Signature signature, unsigned int nodeId)
    {
        if(nodeId >= component)
            node->items.insert(id);

        signature.erase(signature.begin());

        for(unsigned int type : signature) {
            addRecursive(&(node->children[type]), id, component, signature, type);
            signature.erase(signature.begin());
        }
    }

    void removeRecursive(SignatureNode *node, size_t id, unsigned int component, Signature signature, unsigned int nodeId)
    {
        if(nodeId >= component)
            node->items.erase(id);

        signature.erase(signature.begin());

        for(unsigned int type : signature) {
            removeRecursive(&(node->children[type]), id, component, signature, type);
            signature.erase(signature.begin());
        }
    }

    void removeAllRecursive(SignatureNode *node, size_t id, Signature signature, unsigned int nodeId)
    {
        node->items.erase(id);

        signature.erase(signature.begin());

        for(unsigned int type : signature) {
            removeAllRecursive(&(node->children[type]), id, signature, type);
            signature.erase(signature.begin());
        }
    }
};


// STORAGE
// Static storage classes, wrappers for containers
typedef size_t ComponentIndex;

typedef std::vector<ComponentIndex> ComponentList;
typedef std::vector<BaseContainer*> ComponentStorage;
typedef std::vector<BaseSystem*> SystemStorage;

typedef std::pair<ComponentType, BaseContainer*> ComponentStorageItem;
typedef std::pair<SystemType, BaseSystem*> SystemStorageItem;

class StaticComponentStorage : public StaticStorage<ComponentStorage> {};
class StaticEntityStorage : public StaticStorage<Container<ComponentList>> {};
class StaticSignatureTree : public StaticStorage<SignatureTree> {};
class StaticSystemStorage : public StaticStorage<SystemStorage> {};


// ECS
// Main ECS class, manage entity/system/component creation and deletion
typedef boost::container::flat_set<size_t> EntitySet;

class ECS {
public:
    // create a new component and return a temporary handler
    template<class T, typename... Targs> static T* createComponent(EntityId id, Targs... args)
    {
        ComponentType type(T::type());
        T component(id, args...);

        Container<T> *container = componentContainer<T>();
        ComponentIndex index = container->addItem(component);

        // update signature tree
        if(entities()[id].size() <= type)
            entities()[id].resize(type + 1, 0);
        entities()[id][type] = index;

        signatureTree().addToSignature(id, entitySignature(id), type);

        return &(container->item(index));
    }

    template<class T> static void deleteComponent(EntityId id)
    {
        ComponentType type(T::type());
        removeComponent(id, type);
    }

    template<class T> static std::vector<T>* components()
    {
        Container<T> *container = componentContainer<T>();
        return &(container->items());
    }

    template<class T> static T* component(EntityId id)
    {
        ComponentIndex index = componentIndex(id, T::type());
        Container<T> *container = componentContainer<T>();
        return &(container->item(index));
    }

    static EntityId createEntity() { return entities().addItem(ComponentList()); }

    static void deleteEntity(EntityId id)
    {
        signatureTree().removeAll(id, entitySignature(id));

        for (const ComponentType &type : entitySignature(id)) {
            ComponentIndex index = componentIndex(id, type);
            components()[type]->removeItem(index);
        }

        entities().removeItem(id);
    }

    template<class... Args> static EntitySet* entitiesWithComponents()
    {
        Signature signature;
        addToSignature<Args...>(signature);

        return &(signatureTree().itemsMatchingSignature(signature));
    }

    template<class T, typename... Targs> static T* createSystem(Targs... args)
    {
        SystemType type(T::type());

        if(systems().size() <= type)
            systems().resize(type + 1, nullptr);

        if(systems()[type] == nullptr)
            systems()[type] = new T(args...);

        return static_cast<T*>(systems()[type]);
    }

    template<class T> static void deleteSystem()
    {
        SystemType type(T::type());
        delete systems()[type];
        systems()[type] = nullptr;
    }

    static void cleanUp()
    {
        EventDispatcher::get().clear();

        for(size_t i = 0; i < systems().size(); ++i) {
            if(systems()[i])
                delete systems()[i];
        }

        for(size_t i = 0; i < components().size(); ++i) {
            if(components()[i])
                delete components()[i];
        }

        systems().clear();
        components().clear();
        entities().clear();
    }

    static ComponentStorage &components() { return StaticComponentStorage::get(); }
    static Container<ComponentList> &entities() { return StaticEntityStorage::get(); }
    static SignatureTree &signatureTree() { return StaticSignatureTree::get(); }
    static SystemStorage &systems() { return StaticSystemStorage::get(); }

private:
    template<class T> static Container<T> *componentContainer()
    {
        ComponentType type(T::type());

        if(components().size() <= type )
            components().resize(type + 1, nullptr);

        if(components()[type] == nullptr)
            components()[type] = new Container<T>();

        return static_cast<Container<T>*>(components()[type]);
    }

    static ComponentIndex componentIndex(EntityId id, ComponentType type)
    {
        ComponentList *list = &entities()[id];
        if(list->size() <= type)
            list->resize(type + 1, 0);

        return (*list)[type];
    }

    static Signature entitySignature(EntityId id)
    {
        Signature signature;
        for(unsigned int i = 0; i < entities()[id].size(); ++i) {
            if(entities()[id][i] != 0)
                signature.insert(i);
        }
        return signature;
    }

    static void removeComponent(EntityId id, const ComponentType type)
    {
        // update signature tree
        signatureTree().removeFromSignature(id, entitySignature(id), type);
        ComponentIndex index = componentIndex(id, type);
        components()[type]->removeItem(index);
        entities()[id][type] = 0;
    }

    template<class T>
    static void addToSignature(std::set<unsigned int> &signature) { signature.insert(T::type()); }

#if 0
template<class T, class ... Args> // >=1 template parameters -- ambiguity!
static void addToSignature(std::set<unsigned int> &signature) { signature.insert(T::type()); }
#endif

    template<class T1, class T2, class ...Args>
    static void addToSignature(std::set<unsigned int> &signature)
    {
        addToSignature<T1>(signature);
        addToSignature<T2, Args...>(signature);
    }
};

#endif // ECS_H
