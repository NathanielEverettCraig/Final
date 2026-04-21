#include <AICombat/BigBrawlerStateMachine.hpp>

#include <Canis/App.hpp>
#include <Canis/AudioManager.hpp>
#include <Canis/ConfigHelper.hpp>
#include <Canis/Debug.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace AICombat
{
    namespace
    {
        ScriptConf BigBrawlerStateMachineConf = {};
    }

    BigIdleState::BigIdleState(SuperPupUtilities::StateMachine& _stateMachine) :
        State(Name, _stateMachine) {}

    void BigIdleState::Enter()
    {
        if (BigBrawlerStateMachine* brawlerStatMachine = dynamic_cast<BigBrawlerStateMachine*>(m_stateMachine))
            brawlerStatMachine->ResetHammerPose();
    }

    void BigIdleState::Update(float)
    {
        if (BigBrawlerStateMachine* brawlerStatMachine = dynamic_cast<BigBrawlerStateMachine*>(m_stateMachine))
        {
            if (brawlerStatMachine->FindClosestTarget() != nullptr)
                brawlerStatMachine->ChangeState(BigChaseState::Name);
        }
    }

    BigChaseState::BigChaseState(SuperPupUtilities::StateMachine& _stateMachine) :
        State(Name, _stateMachine) {}

    void BigChaseState::Enter()
    {
        if (BigBrawlerStateMachine* brawlerStatMachine = dynamic_cast<BigBrawlerStateMachine*>(m_stateMachine))
            brawlerStatMachine->ResetHammerPose();
    }

    void BigChaseState::Update(float _dt)
    {
        BigBrawlerStateMachine* brawlerStatMachine = dynamic_cast<BigBrawlerStateMachine*>(m_stateMachine);
        if (brawlerStatMachine == nullptr)
            return;

        Canis::Entity* target = brawlerStatMachine->FindClosestTarget();

        if (target == nullptr)
        {
            brawlerStatMachine->ChangeState(BigIdleState::Name);
            return;
        }

        brawlerStatMachine->FaceTarget(*target);

        if (brawlerStatMachine->DistanceTo(*target) <= brawlerStatMachine->GetAttackRange())
        {
            brawlerStatMachine->ChangeState(BighammerTimeState::Name);
            return;
        }

        brawlerStatMachine->MoveTowards(*target, moveSpeed, _dt);
    }

    BighammerTimeState::BighammerTimeState(SuperPupUtilities::StateMachine& _stateMachine) :
        State(Name, _stateMachine) {}

    void BighammerTimeState::Enter()
    {
        if (BigBrawlerStateMachine* brawlerStatMachine = dynamic_cast<BigBrawlerStateMachine*>(m_stateMachine))
            brawlerStatMachine->SetHammerSwing(0.0f);
    }

    void BighammerTimeState::Update(float)
    {
        BigBrawlerStateMachine* brawlerStatMachine = dynamic_cast<BigBrawlerStateMachine*>(m_stateMachine);
        if (brawlerStatMachine == nullptr)
            return;

        if (Canis::Entity* target = brawlerStatMachine->FindClosestTarget())
            brawlerStatMachine->FaceTarget(*target);

        const float duration = std::max(attackDuration, 0.001f);
        brawlerStatMachine->SetHammerSwing(brawlerStatMachine->GetStateTime() / duration);

        if (brawlerStatMachine->GetStateTime() < duration)
            return;

        if (brawlerStatMachine->FindClosestTarget() != nullptr)
            brawlerStatMachine->ChangeState(BigChaseState::Name);
        else
            brawlerStatMachine->ChangeState(BigIdleState::Name);
    }

    void BighammerTimeState::Exit()
    {
        if (BigBrawlerStateMachine* brawlerStatMachine = dynamic_cast<BigBrawlerStateMachine*>(m_stateMachine))
            brawlerStatMachine->ResetHammerPose();
    }

    BigBrawlerStateMachine::BigBrawlerStateMachine(Canis::Entity& _entity) :
        SuperPupUtilities::StateMachine(_entity),
        BigIdleState(*this),
        BigChaseState(*this),
        BighammerTimeState(*this) {}

    void RegisterBigBrawlerStateMachineScript(Canis::App& _app)
    {
        REGISTER_PROPERTY(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, targetTag);
        REGISTER_PROPERTY(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, detectionRange);
        REGISTER_PROPERTY(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, bodyColliderSize);
        RegisterAccessorProperty(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, BigChaseState, moveSpeed);
        RegisterAccessorProperty(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, BighammerTimeState, hammerRestDegrees);
        RegisterAccessorProperty(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, BighammerTimeState, hammerSwingDegrees);
        RegisterAccessorProperty(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, BighammerTimeState, attackRange);
        RegisterAccessorProperty(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, BighammerTimeState, attackDuration);
        RegisterAccessorProperty(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, BighammerTimeState, attackDamageTime);
        REGISTER_PROPERTY(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, maxHealth);
        REGISTER_PROPERTY(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, logStateChanges);
        REGISTER_PROPERTY(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, hammerVisual);
        REGISTER_PROPERTY(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, hitSfxPath1);
        REGISTER_PROPERTY(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, hitSfxPath2);
        REGISTER_PROPERTY(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, hitSfxVolume);
        REGISTER_PROPERTY(BigBrawlerStateMachineConf, AICombat::BigBrawlerStateMachine, deathEffectPrefab);

        DEFAULT_CONFIG_AND_REQUIRED(
            BigBrawlerStateMachineConf,
            AICombat::BigBrawlerStateMachine,
            Canis::Transform,
            Canis::Material,
            Canis::Model,
            Canis::Rigidbody,
            Canis::BoxCollider);

        BigBrawlerStateMachineConf.DEFAULT_DRAW_INSPECTOR(AICombat::BigBrawlerStateMachine);

        _app.RegisterScript(BigBrawlerStateMachineConf);
    }

    DEFAULT_UNREGISTER_SCRIPT(BigBrawlerStateMachineConf, BigBrawlerStateMachine)

    void BigBrawlerStateMachine::Create()
    {
        entity.GetComponent<Canis::Transform>();

        Canis::Rigidbody& rigidbody = entity.GetComponent<Canis::Rigidbody>();
        rigidbody.motionType = Canis::RigidbodyMotionType::KINEMATIC;
        rigidbody.useGravity = false;
        rigidbody.allowSleeping = false;
        rigidbody.linearVelocity = Canis::Vector3(0.0f);
        rigidbody.angularVelocity = Canis::Vector3(0.0f);

        entity.GetComponent<Canis::BoxCollider>().size = bodyColliderSize;

        if (entity.HasComponent<Canis::Material>())
        {
            m_baseColor = entity.GetComponent<Canis::Material>().color;
            m_hasBaseColor = true;
        }
    }

    void BigBrawlerStateMachine::Ready()
    {
        if (entity.HasComponent<Canis::Material>())
        {
            m_baseColor = entity.GetComponent<Canis::Material>().color;
            m_hasBaseColor = true;
        }

        m_currentHealth = std::max(maxHealth, 1);
        m_stateTime = 0.0f;
        m_useFirstHitSfx = true;

        ClearStates();
        AddState(BigIdleState);
        AddState(BigChaseState);
        AddState(BighammerTimeState);

        ResetHammerPose();
        ChangeState(BigIdleState::Name);
    }

    void BigBrawlerStateMachine::Destroy()
    {
        hammerVisual = nullptr;
        SuperPupUtilities::StateMachine::Destroy();
    }

    void BigBrawlerStateMachine::Update(float _dt)
    {
        if (!IsAlive())
            return;

        m_stateTime += _dt;
        SuperPupUtilities::StateMachine::Update(_dt);
    }

    Canis::Entity* BigBrawlerStateMachine::FindClosestTarget() const
    {
        if (targetTag.empty() || !entity.HasComponent<Canis::Transform>())
            return nullptr;

        const Canis::Transform& transform = entity.GetComponent<Canis::Transform>();
        const Canis::Vector3 origin = transform.GetGlobalPosition();
        Canis::Entity* closestTarget = nullptr;
        float closestDistance = detectionRange;

        for (Canis::Entity* candidate : entity.scene.GetEntitiesWithTag(targetTag))
        {
            if (candidate == nullptr || candidate == &entity || !candidate->active)
                continue;

            if (!candidate->HasComponent<Canis::Transform>())
                continue;

            if (const BigBrawlerStateMachine* other = candidate->GetScript<BigBrawlerStateMachine>())
            {
                if (!other->IsAlive())
                    continue;
            }

            const Canis::Vector3 candidatePosition = candidate->GetComponent<Canis::Transform>().GetGlobalPosition();
            const float distance = glm::distance(origin, candidatePosition);

            if (distance > detectionRange || distance >= closestDistance)
                continue;

            closestDistance = distance;
            closestTarget = candidate;
        }

        return closestTarget;
    }

    float BigBrawlerStateMachine::DistanceTo(const Canis::Entity& _other) const
    {
        if (!entity.HasComponent<Canis::Transform>() || !_other.HasComponent<Canis::Transform>())
            return std::numeric_limits<float>::max();

        const Canis::Vector3 selfPosition = entity.GetComponent<Canis::Transform>().GetGlobalPosition();
        const Canis::Vector3 targetPosition = _other.GetComponent<Canis::Transform>().GetGlobalPosition();
        return glm::distance(selfPosition, targetPosition);
    }

    void BigBrawlerStateMachine::FaceTarget(const Canis::Entity& _target)
    {
        if (!entity.HasComponent<Canis::Transform>() || !_target.HasComponent<Canis::Transform>())
            return;

        Canis::Transform& transform = entity.GetComponent<Canis::Transform>();
        const Canis::Vector3 selfPosition = transform.GetGlobalPosition();
        Canis::Vector3 direction = _target.GetComponent<Canis::Transform>().GetGlobalPosition() - selfPosition;
        direction.y = 0.0f;

        if (glm::dot(direction, direction) <= 0.0001f)
            return;

        direction = glm::normalize(direction);
        transform.rotation.y = std::atan2(-direction.x, -direction.z);
    }

    void BigBrawlerStateMachine::MoveTowards(const Canis::Entity& _target, float _speed, float _dt)
    {
        if (!entity.HasComponent<Canis::Transform>() || !_target.HasComponent<Canis::Transform>())
            return;

        Canis::Transform& transform = entity.GetComponent<Canis::Transform>();
        const Canis::Vector3 selfPosition = transform.GetGlobalPosition();
        Canis::Vector3 direction = _target.GetComponent<Canis::Transform>().GetGlobalPosition() - selfPosition;
        direction.y = 0.0f;

        if (glm::dot(direction, direction) <= 0.0001f)
            return;

        direction = glm::normalize(direction);
        transform.position += direction * _speed * _dt;
    }

    void BigBrawlerStateMachine::ChangeState(const std::string& _stateName)
    {
        if (SuperPupUtilities::StateMachine::GetCurrentStateName() == _stateName)
            return;

        if (!SuperPupUtilities::StateMachine::ChangeState(_stateName))
            return;

        m_stateTime = 0.0f;

        if (logStateChanges)
            Canis::Debug::Log("%s -> %s", entity.name.c_str(), _stateName.c_str());
    }

    const std::string& BigBrawlerStateMachine::GetCurrentStateName() const
    {
        return SuperPupUtilities::StateMachine::GetCurrentStateName();
    }

    float BigBrawlerStateMachine::GetStateTime() const
    {
        return m_stateTime;
    }

    float BigBrawlerStateMachine::GetAttackRange() const
    {
        return BighammerTimeState.attackRange;
    }

    int BigBrawlerStateMachine::GetCurrentHealth() const
    {
        return m_currentHealth;
    }

    void BigBrawlerStateMachine::ResetHammerPose()
    {
        SetHammerSwing(0.0f);
    }

    void BigBrawlerStateMachine::SetHammerSwing(float _normalized)
    {
        if (hammerVisual == nullptr || !hammerVisual->HasComponent<Canis::Transform>())
            return;

        Canis::Transform& hammerTransform = hammerVisual->GetComponent<Canis::Transform>();
        const float normalized = Clamp01(_normalized);
        const float swingBlend = (normalized <= 0.5f)
            ? normalized * 2.0f
            : (1.0f - normalized) * 2.0f;

        hammerTransform.rotation.x = DEG2RAD *
            (BighammerTimeState.hammerRestDegrees + (BighammerTimeState.hammerSwingDegrees * swingBlend));
    }

    void BigBrawlerStateMachine::TakeDamage(int _damage)
    {
        if (!IsAlive())
            return;

        const int damageToApply = std::max(_damage, 0);
        if (damageToApply <= 0)
            return;

        m_currentHealth = std::max(0, m_currentHealth - damageToApply);
        PlayHitSfx();

        if (m_hasBaseColor && entity.HasComponent<Canis::Material>())
        {
            Canis::Material& material = entity.GetComponent<Canis::Material>();
            const float healthRatio = (maxHealth > 0)
                ? (static_cast<float>(m_currentHealth) / static_cast<float>(maxHealth))
                : 0.0f;

            material.color = Canis::Vector4(
                m_baseColor.x * (0.5f + (0.5f * healthRatio)),
                m_baseColor.y * (0.5f + (0.5f * healthRatio)),
                m_baseColor.z * (0.5f + (0.5f * healthRatio)),
                m_baseColor.w);
        }

        if (m_currentHealth > 0)
            return;

        if (logStateChanges)
            Canis::Debug::Log("%s was defeated.", entity.name.c_str());

        SpawnDeathEffect();
        entity.Destroy();
    }

    void BigBrawlerStateMachine::PlayHitSfx()
    {
        const Canis::AudioAssetHandle& selectedSfx = m_useFirstHitSfx ? hitSfxPath1 : hitSfxPath2;
        m_useFirstHitSfx = !m_useFirstHitSfx;

        if (selectedSfx.Empty())
            return;

        Canis::AudioManager::PlaySFX(selectedSfx, std::clamp(hitSfxVolume, 0.0f, 1.0f));
    }

    void BigBrawlerStateMachine::SpawnDeathEffect()
    {
        if (deathEffectPrefab.Empty() || !entity.HasComponent<Canis::Transform>())
            return;

        const Canis::Transform& sourceTransform = entity.GetComponent<Canis::Transform>();
        const Canis::Vector3 spawnPosition = sourceTransform.GetGlobalPosition();
        const Canis::Vector3 spawnRotation = sourceTransform.GetGlobalRotation();

        for (Canis::Entity* spawnedEntity : entity.scene.Instantiate(deathEffectPrefab))
        {
            if (spawnedEntity == nullptr || !spawnedEntity->HasComponent<Canis::Transform>())
                continue;

            Canis::Transform& spawnedTransform = spawnedEntity->GetComponent<Canis::Transform>();
            spawnedTransform.position = spawnPosition;
            spawnedTransform.rotation = spawnRotation;
        }
    }

    bool BigBrawlerStateMachine::IsAlive() const
    {
        return m_currentHealth > 0;
    }
}
