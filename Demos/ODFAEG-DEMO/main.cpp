#include "application.h"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::window;
using namespace odfaeg::audio;
using namespace sorrok;

    class SkillAction {
        public :
        void launchLastHeal() {
        }
    };
int main() {


    SkillAction sa;
    FastDelegate<void> df(&SkillAction::launchLastHeal, sa);
    return 0;

    MyAppli app(sf::VideoMode(800, 600), "Test odfaeg");
    return app.exec();

}




