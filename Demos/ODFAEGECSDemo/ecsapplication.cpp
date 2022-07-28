#include "ecsapplication.hpp"
using namespace odfaeg::graphic;
using namespace odfaeg::core;
namespace sorrok {
    MyECSAppli::MyECSAppli(sf::VideoMode vm, std::string title) : Application<MyECSAppli>(vm, title, sf::Style::Default, odfaeg::window::ContextSettings(0, 0, 4, 4, 6)) {
        fpsCounter = 0;
    }
    void MyECSAppli::onLoad() {
        TextureManager<> tm;
        tm.fromFileWithAlias("tilesets/herbe.png", "GRASS");
        cache.addResourceManager(tm, "TextureManager");
    }
}
