#pragma once

#include <Canis/Entity.hpp>

namespace Canis
{
    struct UIActionContext;
}

namespace RollABall
{
    class PauseMenuController : public Canis::ScriptableEntity
    {
    public:
        static constexpr const char* ScriptName = "RollABall::PauseMenuController";

        Canis::Entity* pauseMenu = nullptr;

        explicit PauseMenuController(Canis::Entity& _entity) : Canis::ScriptableEntity(_entity) {}

        void Create() override;
        void Ready() override;
        void Destroy() override;
        void Update(float _dt) override;
        
        void ResumeAction(const Canis::UIActionContext& _context);
        void QuitAction(const Canis::UIActionContext& _context);
    };

    void RegisterPauseMenuControllerScript(Canis::App& _app);
    void UnRegisterPauseMenuControllerScript(Canis::App& _app);
}
