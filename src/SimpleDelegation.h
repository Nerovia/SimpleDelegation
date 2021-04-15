#pragma once

template<typename R, typename ...TArgs>
class Delegate;

namespace Invokers
{
    template<typename R, typename ...TArgs>
    class StaticInvoker : public Delegate<R, TArgs...>
    {
        protected:
            R (*function)(TArgs...);
        public:
            StaticInvoker(R (*function)(TArgs...)) : function(function) { }
            R invoke(TArgs... args) override { return function(args...); }
    };

    template<typename C, typename R, typename ...TArgs>
    class MemberInvoker : public Delegate<R, TArgs...>
    {
        protected:
            C* sender;
            R (C::*function)(TArgs...);
        public:
            MemberInvoker(C* sender, R (C::*function)(TArgs...)) : sender(sender), function(function) { }
            R invoke(TArgs... args) override { return (sender->*function)(args...); }
    };

    template<typename O, typename R, typename ...TArgs>
    class ObjectInvoker : public Delegate<R, TArgs...>
    {
        protected:
            O object;
        public: 
            ObjectInvoker(const O& object) : object(object) { }
            R invoke(TArgs... args) override { return object(args...); }
    };
}

template<typename R, typename ...TArgs>
class Delegate
{
    public:
        static Delegate<R, TArgs...>* create(R (*function)(TArgs...))
        {
            return new Invokers::StaticInvoker<R, TArgs...>(function);
        }

        template<typename C>
        static Delegate<R, TArgs...>* create(C* sender, R (C::*function)(TArgs...))
        {
            return new Invokers::MemberInvoker<C, R, TArgs...>(sender, function);
        }

        template<typename O>
        static Delegate<R, TArgs...>* create(const O& object)
        {
            return new Invokers::ObjectInvoker<O, R, TArgs...>(object);
        }

        virtual ~Delegate() { };
        virtual R invoke(TArgs...) = 0;
        R operator()(TArgs... args) { return invoke(args...); }
};

template<typename T>
using Getter = Delegate<T>;

template<typename T>
using Setter = Delegate<void, T>;

using Action = Delegate<void>;

template<typename T>
class Property;

namespace Invokers
{
    template<typename T>
    class DelegateGetInvoker : public Property<T>
    {
        public:
            Getter<T>* getter;
        public:
            DelegateGetInvoker(Getter<T>* getter) : getter(getter) { }
            ~DelegateGetInvoker() { delete getter; }
            T get() override { return getter->invoke(); }
    };

    template<typename T>
    class DelegateGetSetInvoker : public DelegateGetInvoker<T>
    {
        public:
            Setter<T>* setter;
        public:
            DelegateGetSetInvoker(Getter<T>* getter, Setter<T>* setter) : DelegateGetInvoker<T>(getter), setter(setter) { }
            ~DelegateGetSetInvoker() { delete setter; }
            void set(T value) override { setter->invoke(value); }
    };

    template<typename C, typename T>
    class MemberGetInvoker : public Property<T>
    {
        protected:
            C* sender;
            T (C::*getter)();
        public:
            MemberGetInvoker(C* sender, T(C::*getter)()) : sender(sender), getter(getter) { }
            T get() override { return (sender->*getter)(); }
    };

    template<typename C, typename T>
    class MemberGetSetInvoker : public MemberGetInvoker<C, T>
    {
        protected:
            void (C::*setter)(T);
        public:
            MemberGetSetInvoker(C* sender, T(C::*getter)(), void(C::*setter)(T)) : MemberGetInvoker<C, T>(sender, getter), setter(setter) { }
            void set(T value) override { (MemberGetInvoker<C, T>::sender->*setter)(value); }
    };
}

template<typename T>
class Property
{
    public:
        static Property<T>* create(Delegate<T>* getter)
        {
            return new Invokers::DelegateGetInvoker<T>(getter);
        }

        static Property<T>* create(Delegate<T>* getter, Delegate<void, T>* setter)
        {
            return new Invokers::DelegateGetSetInvoker<T>(getter, setter);
        }

        template<typename C>
        static Property<T>* create(C* sender, T(C::*getter)())
        {
            return new Invokers::MemberGetInvoker<C, T>(sender, getter);
        }

        template<typename C>
        static Property<T>* create(C* sender, T(C::*getter)(), void(C::*setter)(T))
        {
            return new Invokers::MemberGetSetInvoker<C, T>(sender, getter, setter);
        }
        
        virtual ~Property() {};
        virtual T get() = 0;
        virtual void set(T value) { }
};
