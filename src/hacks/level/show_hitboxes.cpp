#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../../core/gui.hpp"
#include "../../core/config.hpp"

GUI_HACK_CREATE("Level", "Show Hitboxes", "Visualises level hitboxes", true);

class $modify(PlayLayer) {
    struct Fields {
        cocos2d::CCDrawNode* m_drawNode;
    };

    void createDrawNode() {
        auto* parent = m_debugDrawNode->getParent();
        if (!parent) return;

        auto* drawNode = cocos2d::CCDrawNode::create();
        drawNode->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        drawNode->setZOrder(9999);
        drawNode->setID("hitboxes"_spr);
        drawNode->m_bUseArea = false;
        parent->addChild(drawNode);

        m_fields->m_drawNode = drawNode;
    }

    void createObjectsFromSetupFinished() {
        PlayLayer::createObjectsFromSetupFinished();
        createDrawNode();
    }

    void updateHitboxNode() {
        auto* drawNode = m_fields->m_drawNode;
        if (!drawNode) return;
        drawNode->clear();

        auto& config = Config::get();
        if (!config.get<bool>("level.show_hitboxes", false)) return;

        auto drawRect = [](cocos2d::CCDrawNode* node, cocos2d::CCRect const& rect,
            cocos2d::ccColor4F const& color, float borderWidth, cocos2d::ccColor4F const& borderColor)
        {
            cocos2d::CCPoint vertices[4] = {
                {rect.getMinX(), rect.getMinY()},
                {rect.getMinX(), rect.getMaxY()},
                {rect.getMaxX(), rect.getMaxY()},
                {rect.getMaxX(), rect.getMinY()}
            };
            node->drawPolygon(vertices, 4, color, borderWidth, borderColor);
        };

        auto drawOrientedOrRect = [&](GameObject* obj,
            cocos2d::ccColor4F const& fill, float bw, cocos2d::ccColor4F const& border)
        {
            if (auto* ob = m_isEditor ? obj->getOrientedBox() : obj->m_orientedBox) {
                drawNode->drawPolygon(ob->m_corners.data(), 4, fill, bw, border);
            } else {
                bool dirty = obj->m_isObjectRectDirty;
                bool boxOff = obj->m_boxOffsetCalculated;
                drawRect(drawNode, obj->getObjectRect(), fill, bw, border);
                obj->m_isObjectRectDirty = dirty;
                obj->m_boxOffsetCalculated = boxOff;
            }
        };

        if (!m_sections.empty()) {
            const int rightBound = std::min(m_rightSectionIndex, static_cast<int>(m_sections.size()) - 1);
            for (int i = m_leftSectionIndex; i <= rightBound; ++i) {
                auto* leftSection = m_sections[i];
                if (!leftSection) continue;
                const int topBound = std::min(m_topSectionIndex, static_cast<int>(leftSection->size()) - 1);
                auto* sizeRow = m_sectionSizes[i];
                for (int j = m_bottomSectionIndex; j <= topBound; ++j) {
                    auto* section = leftSection->at(j);
                    if (!section) continue;
                    const int sectionSize = sizeRow->at(j);
                    auto* data = section->data();
                    for (int k = 0; k < sectionSize; ++k) {
                        auto* obj = data[k];
                        if (!obj) continue;

                        if (obj->m_objectType == GameObjectType::Decoration || !obj->m_isActivated || obj->m_isGroupDisabled)
                            continue;

                        switch (obj->m_objectType) {
                            default: {
                                if (obj == m_player1 || obj == m_player2) break;
                                bool const isSpeedPortal = obj->m_objectID == 200 || obj->m_objectID == 201 || obj->m_objectID == 202 || obj->m_objectID == 203 || obj->m_objectID == 1334;
                                if (obj->m_objectType == GameObjectType::Modifier && !isSpeedPortal) {
                                    if (static_cast<EffectGameObject*>(obj)->m_isTouchTriggered)
                                        drawOrientedOrRect(obj, {0,0,0,0}, 0.5f, {1.f,0.f,0.9f,1.f});
                                    break;
                                }
                                drawOrientedOrRect(obj, {0,0,0,0}, 0.5f, {0.f,1.f,0.f,1.f});
                                break;
                            }
                            case GameObjectType::Solid: {
                                bool const p = obj->m_isPassable;
                                drawRect(drawNode, obj->getObjectRect(),
                                    {0,0,0,0},
                                    0.5f,
                                    p ? cocos2d::ccColor4F{0.f, 1.f, 1.f, 1.f} : cocos2d::ccColor4F{0.f, 0.247f, 1.f, 1.f});
                                break;
                            }
                            case GameObjectType::Slope: {
                                auto const rect = obj->getObjectRect();
                                cocos2d::CCPoint verts[3] = {
                                    {rect.getMinX(), rect.getMinY()},
                                    {rect.getMinX(), rect.getMaxY()},
                                    {rect.getMaxX(), rect.getMinY()},
                                };
                                cocos2d::CCPoint const tr{rect.getMaxX(), rect.getMaxY()};
                                switch (obj->m_slopeDirection) {
                                    case 0: case 7: verts[1] = tr; break;
                                    case 1: case 5: verts[0] = tr; break;
                                    case 3: case 6: verts[2] = tr; break;
                                    default: break;
                                }
                                bool const p = obj->m_isPassable;
                                drawNode->drawPolygon(verts, 3, {0, 0, 0, 0}, 0.5f, p ? cocos2d::ccColor4F{0.f, 1.f, 1.f, 1.f} :
                                                                                        cocos2d::ccColor4F{0.f, 0.247f, 1.f, 1.f});
                                break;
                            }
                            case GameObjectType::AnimatedHazard:
                            case GameObjectType::Hazard: {
                                if (obj == m_anticheatSpike) break;
                                float const radius = std::max(obj->m_scaleX, obj->m_scaleY) * obj->m_objectRadius;

                                if (radius > 0)
                                    drawNode->drawCircle(obj->getPosition(), radius, {0, 0, 0, 0}, 0.5f, {1.f, 0.f, 0.f, 1.f}, 16);
                                else
                                    drawOrientedOrRect(obj, {0, 0, 0, 0}, 0.5f, {1.f, 0.f, 0.f, 1.f});

                                break;
                            }
                            case GameObjectType::CollisionObject: break;
                        }
                    }
                }
            }
        }

        auto drawPlayer = [&](PlayerObject* player) {
            if (!player) return;

            if (auto* ob = player->m_orientedBox)
                drawNode->drawPolygon(ob->m_corners.data(), 4, {0, 0, 0, 0}, 0.5f, {0.5f, 0.f, 0.f, 1.f});

            drawRect(drawNode, player->getObjectRect(), {0, 0, 0, 0}, 0.5f, {1.f, 0.f, 0.f, 1.f});
            drawRect(drawNode, player->getObjectRect(0.3f, 0.3f), {0, 0, 0, 0}, 0.5f, {0.f, 0.f, 1.f, 1.f});
        };

        drawPlayer(m_player1);
        if (m_gameState.m_isDualMode) drawPlayer(m_player2);
    }

    void updateVisibility(float dt) override {
        PlayLayer::updateVisibility(dt);
        updateHitboxNode();
    }
};