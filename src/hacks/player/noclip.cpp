#include <Geode/Geode.hpp>
#include <Geode/Modify/PlayLayer.hpp>
#include "../../core/gui.hpp"

GUI_HACK_CREATE("Level", "level.noclip", "Noclip", "The player will be invincible to obstacles", true);

// ==========================================
// CATEGORY: PLAYER (ОСНОВНОЕ)
// ==========================================
GUI_HACK_CREATE("Player Main", "p.god", "Godmode", "Invincibility to all obstacles", true);
GUI_HACK_CREATE("Player Main", "p.noclip", "Noclip", "Pass through solid objects", true);
GUI_HACK_CREATE("Player Main", "p.hitbox", "Custom Hitbox", "Modify player collision box", true);
GUI_HACK_CREATE("Player Main", "p.size", "Scale Hack", "Change player character size", true);

GUI_HACK_CREATE("Player Physics", "p.grav", "Anti-Gravity", "Disable world gravity", true);
GUI_HACK_CREATE("Player Physics", "p.jump", "Infinite Jump", "Jump in mid-air", true);
GUI_HACK_CREATE("Player Physics", "p.accel", "Instant Accel", "Remove acceleration delay", true);
GUI_HACK_CREATE("Player Physics", "p.friction", "No Friction", "Sliding movement mode", true);

// ==========================================
// CATEGORY: MOVEMENT (ДВИЖЕНИЕ)
// ==========================================
GUI_HACK_CREATE("Speed Control", "m.speed", "Speedhack", "Adjust game simulation speed", true);
GUI_HACK_CREATE("Speed Control", "m.tps", "TPS Modifier", "Change ticks per second", true);
GUI_HACK_CREATE("Speed Control", "m.dash", "Instant Dash", "Instant dash reaction", true);

GUI_HACK_CREATE("Flight", "m.fly", "Fly Mode", "Full 3D flight movement", true);
GUI_HACK_CREATE("Flight", "m.noclip2", "Noclip V2", "Advanced smooth noclip", true);
GUI_HACK_CREATE("Flight", "m.hover", "Hover Mode", "Stay at current height", true);

// ==========================================
// CATEGORY: VISUALS (ВИЗУАЛИЗАЦИЯ)
// ==========================================
GUI_HACK_CREATE("Visuals ESP", "v.esp", "Object ESP", "Highlight objects through walls", true);
GUI_HACK_CREATE("Visuals ESP", "v.tracers", "Line Tracers", "Draw lines to portals", true);
GUI_HACK_CREATE("Visuals ESP", "v.hitbox", "Show Hitboxes", "Draw collision boxes", true);

GUI_HACK_CREATE("Visuals HUD", "v.fps", "Show FPS", "Display frame rate counter", true);
GUI_HACK_CREATE("Visuals HUD", "v.cps", "Show CPS", "Display clicks per second", true);
GUI_HACK_CREATE("Visuals HUD", "v.hide", "Hide HUD", "Remove all game interface", true);
GUI_HACK_CREATE("Visuals HUD", "v.percent", "Precise %", "Show progress with decimals", true);

GUI_HACK_CREATE("Visuals Effects", "v.rgb", "RGB Icons", "Animated rainbow player", true);
GUI_HACK_CREATE("Visuals Effects", "v.trail", "Long Trail", "Infinite movement trail", true);
GUI_HACK_CREATE("Visuals Effects", "v.particles", "No Particles", "Disable death effects", true);
GUI_HACK_CREATE("Visuals Effects", "v.shake", "No Shake", "Disable screen shaking", true);

// ==========================================
// CATEGORY: BYPASS (ОБХОДЫ)
// ==========================================
GUI_HACK_CREATE("Bypass Main", "b.anticheat", "AC Bypass", "Disable internal security", true);
GUI_HACK_CREATE("Bypass Main", "b.copy", "Copy Hack", "Copy any level instantly", true);
GUI_HACK_CREATE("Bypass Main", "b.verify", "Instant Verify", "Verify level without playing", true);

GUI_HACK_CREATE("Unlocks", "u.icons", "Unlock Icons", "Unlock all shop items", true);
GUI_HACK_CREATE("Unlocks", "u.colors", "Unlock Colors", "Unlock all color palettes", true);
GUI_HACK_CREATE("Unlocks", "u.tiers", "Unlock Tiers", "Unlock all difficulty tiers", true);

// ==========================================
// CATEGORY: EDITOR (РЕДАКТОР)
// ==========================================
GUI_HACK_CREATE("Editor Core", "e.limit", "No Object Limit", "Remove object count cap", true);
GUI_HACK_CREATE("Editor Core", "e.zoom", "Zoom Hack", "Unlimited editor zoom", true);
GUI_HACK_CREATE("Editor Core", "e.layers", "Extra Layers", "Enable more editor layers", true);

GUI_HACK_CREATE("Editor Tools", "e.snap", "Fine Snapping", "Pixel-perfect object snapping", true);
GUI_HACK_CREATE("Editor Tools", "e.rotate", "Free Rotate", "Rotate objects by 1 degree", true);
GUI_HACK_CREATE("Editor Tools", "e.scale", "Free Scale", "Resize any object freely", true);

// ==========================================
// CATEGORY: WORLD (ОКРУЖЕНИЕ)
// ==========================================
GUI_HACK_CREATE("World Environment", "w.spikes", "No Spikes", "Disable spike collision", true);
GUI_HACK_CREATE("World Environment", "w.solid", "No Solids", "Disable block collision", true);
GUI_HACK_CREATE("World Environment", "w.deco", "No Deco", "Hide decorative objects", true);

GUI_HACK_CREATE("World Render", "w.bg", "Static BG", "Prevent background changes", true);
GUI_HACK_CREATE("World Render", "w.pulse", "No Pulse", "Disable object pulsing", true);
GUI_HACK_CREATE("World Render", "w.rot", "No Rotation", "Disable camera rotation", true);

// ==========================================
// CATEGORY: AUTOMATION (БОТЫ)
// ==========================================
GUI_HACK_CREATE("Auto Play", "a.bot", "Macro Player", "Execute recorded movement", true);
GUI_HACK_CREATE("Auto Play", "a.click", "Auto Clicker", "Automatic input handling", true);
GUI_HACK_CREATE("Auto Play", "a.respawn", "Fast Respawn", "Zero delay after death", true);

// ==========================================
// CATEGORY: COSMETIC (ВНЕШНОСТЬ)
// ==========================================
GUI_HACK_CREATE("Cosmetics 1", "c.glow", "Full Glow", "Force glow on all icons", true);
GUI_HACK_CREATE("Cosmetics 1", "c.trail", "Rainbow Trail", "Cycling trail colors", true);
GUI_HACK_CREATE("Cosmetics 2", "c.death", "Custom Death", "Modify explosion effect", true);
GUI_HACK_CREATE("Cosmetics 2", "c.ship", "Small Ship", "Always use mini-ship icon", true);

// ==========================================
// CATEGORY: NETWORK & SOCIAL (СЕТЬ)
// ==========================================
GUI_HACK_CREATE("Network", "n.ping", "Ping Spoof", "Fake connection latency", true);
GUI_HACK_CREATE("Network", "n.nick", "Nick Spoof", "Change display name", true);
GUI_HACK_CREATE("Network", "n.chat", "Spam Filter", "Block annoying messages", true);

// ==========================================
// CATEGORY: MISC (РАЗНОЕ / ТЕХНИЧЕСКОЕ)
// ==========================================
GUI_HACK_CREATE("Misc Utilities", "z.timer", "Level Timer", "Show exact time in level", true);
GUI_HACK_CREATE("Misc Utilities", "z.logs", "Log Console", "Output debug information", true);
GUI_HACK_CREATE("Misc Utilities", "z.discord", "Discord RPC", "Rich Presence integration", true);

GUI_HACK_CREATE("Advanced 1", "z.adv1", "Mem Scan", "Enable memory scanning", true);
GUI_HACK_CREATE("Advanced 1", "z.adv2", "Hook Engine", "Inject custom hooks", true);
GUI_HACK_CREATE("Advanced 2", "z.adv3", "Force Sync", "Sync audio and video", true);
GUI_HACK_CREATE("Advanced 2", "z.adv4", "Low Latency", "Experimental input mode", true);

// ==========================================
// CATEGORY: DEBUG (ДЛЯ РАЗРАБОТКИ)
// ==========================================
GUI_HACK_CREATE("Debug Info", "d.pos", "Show Pos", "Player X/Y coordinates", true);
GUI_HACK_CREATE("Debug Info", "d.vel", "Show Velocity", "Player movement speed vector", true);
GUI_HACK_CREATE("Debug Info", "d.id", "Object ID", "Show ID of hovered object", true);

GUI_HACK_CREATE("Experimental", "x.beta1", "Beta Physics", "Test new physics engine", true);
GUI_HACK_CREATE("Experimental", "x.beta2", "Multi-Thread", "Experimental threading", true);
GUI_HACK_CREATE("Experimental", "x.beta3", "GPU Accel", "Force GPU rendering", true);



class $modify(NoclipPlayLayer, PlayLayer) {
    static void onModify(auto& self) {
        auto& gui = GDH::Gui::get();
        auto* hack = gui.getWindow("Level").findHack("level.noclip");        
        
        hack->addHookPtr(self.getHook("PlayLayer::destroyPlayer").unwrap());
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        if (m_anticheatSpike && obj == m_anticheatSpike)
            return PlayLayer::destroyPlayer(player, obj);
    }
};