#include "ecsapplication.hpp"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::window;
using namespace sorrok;
int main() {
    DynamicTuple systemArray, componentArray, sceneArray, rendererArray;
    sorrok::MyECSAppli app(sf::VideoMode(800, 600), "Test odfaeg");
    return app.exec(systemArray, sceneArray, rendererArray, componentArray);
}
