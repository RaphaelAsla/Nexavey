#pragma once

namespace nex {
    class Entity;

    class Component {
      public:
        Component()          = default;
        virtual ~Component() = default;

        virtual void Initialize() {};
        virtual void OnCreate() {};
        virtual void OnDelete() {};
        virtual void OnTick() {};
        virtual void SetParent([[maybe_unused]] Entity* parent) {};

      protected:
        [[maybe_unused]] Entity* m_parent = nullptr;
    };
}  // namespace nex
