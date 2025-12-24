#ifndef SCENE_BOSS_H
#define SCENE_BOSS_H

#include "Scene.h"
#include <vector>
#include <SDL.h>

class SceneBoss : public Scene {
public:
    virtual void init() override;
    virtual void update(float deltaTime) override;
    virtual void render() override;
    virtual void clean() override;
    virtual void handleEvent(SDL_Event* event) override;

private:
    // Boss属性
    struct Boss {
        float x = 0;
        float y = 0;
        int width = 200;
        int height = 200;
        int health = 500;      // 降低血量（10分进入Boss战）
        int maxHealth = 500;
        float speed = 200.0f;
        float attackTimer = 0.0f;
        float attackInterval = 0.6f;
        bool movingRight = true;
    } boss;

    // Boss子弹
    struct BossBullet {
        float x = 0;
        float y = 0;
        float speedY = 150.0f;
        int width = 20;
        int height = 40;
        bool active = true;
    };

    std::vector<BossBullet> bossBullets;

    // 玩家
    struct Player {
        float x = 0;
        float y = 0;
        int width = 60;
        int height = 60;
        int health = 100;
        int maxHealth = 100;
        float shootCooldown = 0.0f;  // 射击冷却
    } player;

    //玩家移速
    void updatePlayer(float deltaTime); 

    // 玩家子弹
    struct PlayerBullet {
        float x = 0;
        float y = 0;
        float speedY = -500.0f;
        int width = 8;
        int height = 24;
        bool active = true;
    };

    std::vector<PlayerBullet> playerBullets;

    // 游戏状态
    int score = 0;
    bool isGameOver = false;
    float gameOverTimer = 0.0f;

    // 纹理指针
    SDL_Texture* bossTexture = nullptr;
    SDL_Texture* playerTexture = nullptr;
    SDL_Texture* bossBulletTexture = nullptr;
    SDL_Texture* playerBulletTexture = nullptr;
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* bossHealthBarTexture = nullptr;
    SDL_Texture* playerHealthBarTexture = nullptr;

    // 私有方法
    void updateBoss(float deltaTime);
    void updateBullets(float deltaTime);
    void checkCollisions();
    void spawnBossBullets();
    void playerShoot();
};

#endif // SCENE_BOSS_H
