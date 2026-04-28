#include <AICombat/Bolt.hpp>

#include <Canis/App.hpp>
#include <Canis/Time.hpp>
#include <Canis/Math.hpp>
#include <Canis/Scene.hpp>
#include <Canis/Window.hpp>
#include <Canis/Editor.hpp>
#include <Canis/InputManager.hpp>

#include <Canis/ConfigHelper.hpp>

using namespace Canis;

namespace AICombat
{
 
    ScriptConf conf = {};

    void RegisterBoltScript(Canis::App &_app)
    {
        REGISTER_PROPERTY(conf, AICombat::Bolt, speed);
        REGISTER_PROPERTY(conf, AICombat::Bolt, lifeTime);

        DEFAULT_CONFIG_AND_REQUIRED(conf, AICombat::Bolt, Canis::RectTransform, Canis::Sprite2D);

        conf.DrawInspector = [](Editor &_editor, Entity &_entity, const ScriptConf &_conf) -> void
        {
            AICombat::Bolt* Bolt = _entity.GetScript<AICombat::Bolt>();
            if (Bolt != nullptr)
            {
                ImGui::InputFloat(("speed##" + _conf.name).c_str(), &Bolt->speed);
                ImGui::InputFloat(("lifeTime##" + _conf.name).c_str(), &Bolt->lifeTime);
            }
        };

        _app.RegisterScript(conf);
    }

    DEFAULT_UNREGISTER_SCRIPT(conf, Bolt)

    void Bolt::Create() {}

    void Bolt::Ready() {}

    void Bolt::Destroy() {}

    void Bolt::Update(float _dt) {
        RectTransform& transform = entity.GetComponent<RectTransform>();

        transform.Move(transform.GetUp() * speed * _dt);

        lifeTime -= _dt;

        if (lifeTime <= 0.0f) {
            entity.Destroy();
        }
        
    }
}
