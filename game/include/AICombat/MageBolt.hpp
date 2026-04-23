#pragma once

#include <Canis/Entity.hpp>

#include <vector>

namespace AICombat
{
    class BrawlerStateMachine;

    /*class MageDamage : public Canis::ScriptableEntity
    {
    public:
        static constexpr const char* ScriptName = "AICombat::MageDamage";

        Canis::Entity* owner = nullptr;
        Canis::Vector3 sensorSize = Canis::Vector3(1.0f);
        int damage = 20;
        std::string targetTag = "";

        explicit MageDamage(Canis::Entity& _entity) : Canis::ScriptableEntity(_entity) {}

        void Create() override;
        void Ready() override;
        void Update(float _dt) override;

        void CheckSensorEnter();

    private:
        BrawlerStateMachine* GetOwnerStateMachine();
        Canis::Entity* FindOwnerFromHierarchy() const;
        bool HasDamagedThisSwing(Canis::Entity& _target) const;

        std::vector<Canis::Entity*> m_hitTargetsThisSwing = {};

    };

    void RegisterMageDamageScript(Canis::App& _app);
    void UnRegisterMageDamageScript(Canis::App& _app);*/

    class MageBolt : public Canis::ScriptableEntity
    {
    public:
        static constexpr const char* ScriptName = "AiCombat::MageBolt";

        float speed = 10.0f;
        float lifeTime = 25.0f;

        MageBolt(Canis::Entity &_entity) : Canis::ScriptableEntity(_entity) {}

        void Create();
        void Ready();
        void Destroy();
        void Update(float _dt);
    };

    extern void RegisterMageBoltScript(Canis::App& _app);
    extern void UnRegisterMageBoltScript(Canis::App& _app);
}
