//#include "application.h"
#include "odfaeg/Graphics/renderWindow.h"
using namespace odfaeg::core;
using namespace odfaeg::math;
using namespace odfaeg::physic;
using namespace odfaeg::graphic;
using namespace odfaeg::window;




int main() {


    /*MyAppli app(sf::VideoMode(800, 600), "Test odfaeg");
    return app.exec();*/
    RenderWindow window(sf::VideoMode(800, 600), "test vulkan");
    while (true) {
        VertexArray va;
        va.append(Vertex(sf::Vector3f(0, 0, 0)));
        window.draw(va);
        window.display();
        window.clear();
    }
    return 0;

}




