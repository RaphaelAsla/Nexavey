#pragma once

namespace nex {
    class Component {
      public:
        Component() = default;
        virtual ~Component() = default;

        virtual void Initialize() {};
        virtual void OnCreate() {};
        virtual void OnDelete() {};
        virtual void OnTick() {};

      private:
        bool m_enabled = false;
    };
}  // namespace nex
