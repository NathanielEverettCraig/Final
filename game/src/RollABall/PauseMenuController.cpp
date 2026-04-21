#include <RollABall/PauseMenuController.hpp>

#include <Canis/App.hpp>
#include <Canis/Time.hpp>
#include <Canis/Debug.hpp>
#include <Canis/InputManager.hpp>
#include <Canis/ConfigHelper.hpp>
#include <RollABall/PlayerController.hpp>

namespace RollABall
{
    ScriptConf pauseMenuConf = {};

    void RegisterPauseMenuControllerScript(App& _app)
    {
        REGISTER_PROPERTY(pauseMenuConf, RollABall::PauseMenuController, pauseMenu);

        DEFAULT_CONFIG_AND_REQUIRED(pauseMenuConf, RollABall::PauseMenuController, RectTransform, Canvas);

        RegisterUIAction<RollABall::PauseMenuController>(pauseMenuConf, "resume", &RollABall::PauseMenuController::ResumeAction);
        RegisterUIAction<RollABall::PauseMenuController>(pauseMenuConf, "quit", &RollABall::PauseMenuController::QuitAction);

        pauseMenuConf.DEFAULT_DRAW_INSPECTOR(RollABall::PauseMenuController);

        _app.RegisterScript(pauseMenuConf);
    }

    DEFAULT_UNREGISTER_SCRIPT(pauseMenuConf, PauseMenuController)

    void PauseMenuController::Create() {}

    void PauseMenuController::Ready() {
        if (pauseMenu)
            pauseMenu->active = false;
    }

    void PauseMenuController::Destroy() {}

    void PauseMenuController::Update(float _dt)
    {
        if (entity.scene.GetInputManager().JustPressedKey(Key::ESCAPE)) {
            float timeScale = Time::GetTimeScale();

            if (timeScale != 0.0f) {
                Time::SetTimeScale(0.0f);

                if (pauseMenu)
                    pauseMenu->active = true;
            } else {
                Time::SetTimeScale(1.0f);

                if (pauseMenu)
                    pauseMenu->active = false;
            }
        }
    }

    void PauseMenuController::ResumeAction(const Canis::UIActionContext& _context) {
        Time::SetTimeScale(1.0f);

        if (pauseMenu)
            pauseMenu->active = false;
    }

    void PauseMenuController::QuitAction(const Canis::UIActionContext& _context) {
        entity.scene.QuitGame();
    }
}
