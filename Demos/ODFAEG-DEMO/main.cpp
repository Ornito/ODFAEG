#include "application.h"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::window;
using namespace odfaeg::audio;
using namespace sorrok;


int main() {


    /*MyAppli app(sf::VideoMode(800, 600), "Test odfaeg");
    return app.exec();*/
    RenderWindow rw(sf::VideoMode(800, 600), "test vulkan");
    VertexArray va;
    std::cout<<rw.getView().getProjMatrix().getMatrix()<<std::endl;
    va.append(Vertex(sf::Vector3f(0, 100, 0), sf::Color::Red));
    va.append(Vertex(sf::Vector3f(-100, 0, 0), sf::Color::Blue));
    va.append(Vertex(sf::Vector3f(100, 0, 0), sf::Color::Green));
    while (true) {
        rw.draw(va);
        rw.display();
    }

}




