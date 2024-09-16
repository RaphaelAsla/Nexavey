#pragma once

namespace nex {
    class Entity;

    class Component {
      public:
        Component() = default;
        virtual ~Component() = default;

        virtual void Initialize() {};
        virtual void OnCreate() {};
        virtual void OnDelete() {};
        virtual void OnTick() {};
        virtual void SetParent([[maybe_unused]] Entity* parent) {};
    };
}  // namespace nex
