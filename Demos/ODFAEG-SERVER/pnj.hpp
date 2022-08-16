#ifndef SORROK_PNJ
#define SORROK_PNJ
#include "caracter.h"
#include <vector>
#include "quest.hpp"
#include "odfaeg/Graphics/entity.h"
namespace sorrok {
    class Pnj : public Caracter {
        public :
            Pnj(odfaeg::graphic::EntityFactory& entity);
            void addQuest(Quest quest);
            std::vector<Quest> getQuests();
            bool isMovingFromKeyboard();
            bool isMonster();
            template <typename Archive>
            void vtserialize(Archive &ar) {
                Caracter::vtserialize(ar);
                ar(quests);
            }
        private :
            std::vector<Quest> quests;
    };
}
#endif
