#include "skillAction.hpp"
#include "application.h"
using namespace odfaeg::core;
using namespace odfaeg::network;
namespace sorrok {
    void SkillAction::launchLastHeal(Hero* hero, Skill skill) {
        if (hero->isAlive()) {
            static_cast<MyAppli*>(MyAppli::app)->launchSkillAnim(skill.getName());
            std::string request = "ADDLIFE*"+conversionIntString(hero->getId())+"*"+conversionFloatString(10);
            SymEncPacket packet;
            packet<<request;
            Network::sendTcpPacket(packet);
            request = "RETRACTMANA*"+conversionIntString(hero->getId())+"*"+conversionFloatString(skill.getManaCost());
            packet.clear();
            packet<<request;
            Network::sendTcpPacket(packet);
        }
    }
}
/*/usr/local/include/odfaeg/Core/fastDelegate.h|362|error: no matching function for call to ‘std::function<void(sorrok::SkillAction&, sorrok::Hero*&, sorrok::Skill&)>::function(odfaeg::core::DynamicWrapper<void, sorrok::SkillAction, sorrok::Hero*, sorrok::Skill>)’|
/usr/include/c++/11/bits/std_function.h|414|note: candidate: ‘template<class _Functor, class, class> std::function<_Res(_ArgTypes ...)>::function(_Functor) [with _Functor = _Functor; <template-parameter-2-2> = <template-parameter-1-2>; <template-parameter-2-3> = <template-parameter-1-3>; _Res = void; _ArgTypes = {sorrok::SkillAction&, sorrok::Hero*&, sorrok::Skill&}]’|*/
