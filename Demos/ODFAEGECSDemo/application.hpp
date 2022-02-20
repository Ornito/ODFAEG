#ifndef ECS_APPLICATION_HPP
#define ECS_APPLICATION_HPP
#include "odfaeg/Graphics/ECS/application.hpp"
class ECSApplication {
    ECSApplication(sf::VideoMode vm, std::string title, sf::Uint32 style = sf::Style::Default, window::ContextSettings settings = window::ContextSettings()) : odfaeg::graphic::ecs::Application<ECSApplication>(vm, title, style, settings) {
    }
    void onLoad() {
        TextureManager<> tm;
        tm.fromFileWithAlias("tilesets/eau.png", "WATER");
        cache.addResourceManager(tm, "TextureManager");
    }
    void onInit(SystemArray& sya, SceneArray& sa, RendererArray& ra, EntityComponentArray& eca) {
        TextureManager<> &tm = cache.resourceManager<Texture, std::string>("TextureManager");
        Vec2f pos (getView().getPosition().x - getView().getSize().x * 0.5f, getView().getPosition().y - getView().getSize().y * 0.5f);
        BoundingBox bx (pos.x, pos.y, 0, getView().getSize().x, getView().getSize().y, 0);
        ecs::EntityId
    }
};
#endif
