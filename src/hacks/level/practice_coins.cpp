// #include <Geode/Geode.hpp>
// #include <Geode/modify/GJBaseGameLayer.hpp>
// #include "../../core/gui.hpp"

// GUI_HACK_CREATE("Level", "Coins In Practice", "The ability to collect coins in practice", false);

// class $modify(CoinsInPracticeGJBaseGameLayer, GJBaseGameLayer) {
//     static void onModify(auto& self) {
//         auto& gui = GDH::Gui::get();
//         auto& hack = gui.getWindow("Level").findHackByName("Coins In Practice");        
        
//         hack.addHookPtr(self.getHook("GJBaseGameLayer::collisionCheckObjects").unwrap());
//     }

//     void collisionCheckObjects(PlayerObject* object, gd::vector<GameObject*>* objects, int objectCount, float dt) {
//         GJBaseGameLayer::collisionCheckObjects(object, objects, objectCount, dt);
//     }
// };