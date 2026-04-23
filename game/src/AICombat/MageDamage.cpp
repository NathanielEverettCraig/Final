#include <AICombat/MageDamage.hpp>

#include <AICombat/BrawlerStateMachine.hpp>

#include <Canis/App.hpp>
#include <Canis/Time.hpp>
#include <Canis/Math.hpp>
#include <Canis/Scene.hpp>
#include <Canis/Window.hpp>
#include <Canis/Editor.hpp>
#include <Canis/InputManager.hpp>

#include <Canis/ConfigHelper.hpp>

#include <algorithm>

namespace AICombat
{
    namespace
    {
        ScriptConf MageDamageConf = {};
    }

    void RegisterMageDamageScript(Canis::App& _app)
    {
        REGISTER_PROPERTY(MageDamageConf, AICombat::MageDamage, owner);
        REGISTER_PROPERTY(MageDamageConf, AICombat::MageDamage, sensorSize);
        REGISTER_PROPERTY(MageDamageConf, AICombat::MageDamage, damage);
        REGISTER_PROPERTY(MageDamageConf, AICombat::MageDamage, targetTag);

        DEFAULT_CONFIG_AND_REQUIRED(
            MageDamageConf,
            AICombat::MageDamage,
            Canis::Transform,
            Canis::Rigidbody,
            Canis::BoxCollider);

        MageDamageConf.DEFAULT_DRAW_INSPECTOR(AICombat::MageDamage);

        _app.RegisterScript(MageDamageConf);
    }

    DEFAULT_UNREGISTER_SCRIPT(MageDamageConf, MageDamage)

    void MageDamage::Create()
    {
        entity.GetComponent<Canis::Transform>();

        Canis::Rigidbody& rigidbody = entity.GetComponent<Canis::Rigidbody>();
        rigidbody.motionType = Canis::RigidbodyMotionType::STATIC;
        rigidbody.useGravity = false;
        rigidbody.isSensor = true;
        rigidbody.allowSleeping = false;
        rigidbody.linearVelocity = Canis::Vector3(0.0f);
        rigidbody.angularVelocity = Canis::Vector3(0.0f);

        entity.GetComponent<Canis::BoxCollider>().size = sensorSize;
    }

    void MageDamage::Ready()
    {
        if (owner == nullptr)
            owner = FindOwnerFromHierarchy();

        if (targetTag.empty())
        {
            if (BrawlerStateMachine* ownerStateMachine = GetOwnerStateMachine())
                targetTag = ownerStateMachine->targetTag;
        }
    }

    void MageDamage::Update(float)
    {
        CheckSensorEnter();
    }

    void MageDamage::CheckSensorEnter()
    {
        if (!entity.HasComponents<Canis::BoxCollider, Canis::Rigidbody>())
            return;

        BrawlerStateMachine* ownerStateMachine = GetOwnerStateMachine();
        if (ownerStateMachine == nullptr || !ownerStateMachine->IsAlive())
        {
            m_hitTargetsThisSwing.clear();
            return;
        }

        const bool damageWindowOpen =
            ownerStateMachine->GetCurrentStateName() == HammerTimeState::Name &&
            ownerStateMachine->GetStateTime() >= ownerStateMachine->hammerTimeState.attackDamageTime;

        if (!damageWindowOpen)
        {
            m_hitTargetsThisSwing.clear();
            return;
        }

        for (Canis::Entity* other : entity.GetComponent<Canis::BoxCollider>().entered)
        {
            if (other == nullptr || !other->active || other == owner || HasDamagedThisSwing(*other))
                continue;

            BrawlerStateMachine* targetStateMachine = other->GetScript<BrawlerStateMachine>();
            if (targetStateMachine == nullptr || !targetStateMachine->IsAlive())
                continue;

            if (other->tag != targetTag)
                continue;

            targetStateMachine->TakeDamage(damage);
            m_hitTargetsThisSwing.push_back(other);
        }
    }

    BrawlerStateMachine* MageDamage::GetOwnerStateMachine()
    {
        if (owner == nullptr)
            owner = FindOwnerFromHierarchy();

        if (owner == nullptr || !owner->active)
            return nullptr;

        return owner->GetScript<BrawlerStateMachine>();
    }

    Canis::Entity* MageDamage::FindOwnerFromHierarchy() const
    {
        if (!entity.HasComponent<Canis::Transform>())
            return nullptr;

        Canis::Entity* current = entity.GetComponent<Canis::Transform>().parent;
        while (current != nullptr)
        {
            if (current->HasScript<BrawlerStateMachine>())
                return current;

            if (!current->HasComponent<Canis::Transform>())
                break;

            current = current->GetComponent<Canis::Transform>().parent;
        }

        return nullptr;
    }

    bool MageDamage::HasDamagedThisSwing(Canis::Entity& _target) const
    {
        return std::find(m_hitTargetsThisSwing.begin(), m_hitTargetsThisSwing.end(), &_target)
            != m_hitTargetsThisSwing.end();
    }
}
