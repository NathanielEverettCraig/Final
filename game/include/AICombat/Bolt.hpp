#pragma once

#include <Canis/Entity.hpp>

namespace AICombat
{
    class Bolt : public Canis::ScriptableEntity
    {
    public:
        static constexpr const char* ScriptName = "AICombat::Bolt";

        float speed = 10.0f;
        float lifeTime = 25.0f;

        Bolt(Canis::Entity &_entity) : Canis::ScriptableEntity(_entity) {}

        void Create();
        void Ready();
        void Destroy();
        void Update(float _dt);
    };

    extern void RegisterBoltScript(Canis::App& _app);
    extern void UnRegisterBoltScript(Canis::App& _app);
}
