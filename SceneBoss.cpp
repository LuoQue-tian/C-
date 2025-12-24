#include "SceneBoss.h"
#include "SceneMain.h"
#include "SceneEnd.h"
#include "Game.h"
#include <SDL_image.h>
#include <iostream>
#include <algorithm>

void SceneBoss::init() {
    std::cout << "进入Boss战！" << std::endl;

    // 初始化Boss位置
    boss.x = game.getWindowWidth() / 2 - boss.width / 2;
    boss.y = 50;

    // 初始化玩家位置
    player.x = game.getWindowWidth() / 2 - player.width / 2;
    player.y = game.getWindowHeight() - player.height - 50;

    // 加载所有纹理
    bossTexture = IMG_LoadTexture(game.getRenderer(), "assets/image/boss.png");
    playerTexture = IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
    bossBulletTexture = IMG_LoadTexture(game.getRenderer(), "assets/image/bullet.png");//敌人子弹
    playerBulletTexture = IMG_LoadTexture(game.getRenderer(), "assets/image/bullet.png");
    backgroundTexture = IMG_LoadTexture(game.getRenderer(), "assets/image/bg.png");//背景
    bossHealthBarTexture = IMG_LoadTexture(game.getRenderer(), "assets/image/Health UI Black.png");//敌人血量
    playerHealthBarTexture = IMG_LoadTexture(game.getRenderer(), "assets/image/Health UI Black.png");

    // 获取分数
    score = game.getFinalScore();

    // 清空子弹列表
    bossBullets.clear();
    playerBullets.clear();

    // 重置游戏状态
    isGameOver = false;
    gameOverTimer = 0.0f;
    player.shootCooldown = 0.0f;
}


void SceneBoss::update(float deltaTime) {
    // 如果游戏结束，延迟后返回
    if (isGameOver) {
        gameOverTimer += deltaTime;
        if (gameOverTimer > 2.0f) {
            auto sceneEnd = new SceneEnd();
            game.changeScene(sceneEnd);
        }
        return;
    }

    // 更新射击冷却
    if (player.shootCooldown > 0) {
        player.shootCooldown -= deltaTime;
    }

    // 更新玩家移动
    updatePlayer(deltaTime);  

    // 更新Boss
    updateBoss(deltaTime);

    // 更新子弹
    updateBullets(deltaTime);

    // 检查碰撞
    checkCollisions();

    // 检查游戏结束条件
    if (player.health <= 0) {
        isGameOver = true;
        std::cout << "玩家被Boss击败！" << std::endl;
    }

    if (boss.health <= 0) {
        score += 500;  // 击败Boss获得500分
        game.setFinalScore(score);
        isGameOver = true;
        std::cout << "Boss被击败！最终分数：" << score << std::endl;
    }
}






void SceneBoss::updateBoss(float deltaTime) {
    // Boss左右移动
    if (boss.movingRight) {
        boss.x += boss.speed * deltaTime;
        if (boss.x > game.getWindowWidth() - boss.width) {
            boss.movingRight = false;
        }
    }
    else {
        boss.x -= boss.speed * deltaTime;
        if (boss.x < 0) {
            boss.movingRight = true;
        }
    }

    // Boss攻击
    boss.attackTimer += deltaTime;
    if (boss.attackTimer >= boss.attackInterval) {
        boss.attackTimer = 0.0f;
        spawnBossBullets();
    }
}

void SceneBoss::updateBullets(float deltaTime) {
    // 更新Boss子弹
    for (auto& bullet : bossBullets) {
        bullet.y += bullet.speedY * deltaTime;
        if (bullet.y > game.getWindowHeight()) {
            bullet.active = false;
        }
    }

    // 更新玩家子弹
    for (auto& bullet : playerBullets) {
        bullet.y += bullet.speedY * deltaTime;
        if (bullet.y < 0) {
            bullet.active = false;
        }
    }

    // 清理不活跃的子弹
    bossBullets.erase(
        std::remove_if(bossBullets.begin(), bossBullets.end(),
            [](const BossBullet& b) { return !b.active; }),
        bossBullets.end()
    );

    playerBullets.erase(
        std::remove_if(playerBullets.begin(), playerBullets.end(),
            [](const PlayerBullet& b) { return !b.active; }),
        playerBullets.end()
    );
}

void SceneBoss::checkCollisions() {
    // 玩家子弹与Boss碰撞
    for (auto& bullet : playerBullets) {
        if (!bullet.active) continue;

        if (bullet.x + bullet.width > boss.x &&
            bullet.x < boss.x + boss.width &&
            bullet.y + bullet.height > boss.y &&
            bullet.y < boss.y + boss.height) {

            bullet.active = false;
            boss.health -= 25;  // 每次扣25血
        }
    }

    // Boss子弹与玩家碰撞
    for (auto& bullet : bossBullets) {
        if (!bullet.active) continue;

        if (bullet.x + bullet.width > player.x &&
            bullet.x < player.x + player.width &&
            bullet.y + bullet.height > player.y &&
            bullet.y < player.y + player.height) {

            bullet.active = false;
            player.health -= 20;  // 每次扣20血
        }
    }

    // Boss与玩家碰撞
    if (boss.x + boss.width > player.x &&
        boss.x < player.x + player.width &&
        boss.y + boss.height > player.y &&
        boss.y < player.y + player.height) {

        player.health = 0;  // 直接死亡
    }
}

//void SceneBoss::spawnBossBullets() {
//    // Boss发射3发子弹（简单扇形）
//    for (int i = 0; i < 3; i++) {
//        BossBullet bullet;
//        float offsetX = (i - 1) * 30.0f;  // -30, 0, 30
//
//        bullet.x = boss.x + boss.width / 2 - bullet.width / 2 + offsetX;
//        bullet.y = boss.y + boss.height;
//
//        bossBullets.push_back(bullet);
//    }
//}

void SceneBoss::spawnBossBullets() {
    // 随机决定发射模式
    int mode = rand() % 3;  // 0, 1, 2 三种模式

    switch (mode) {
    case 0:  // 扇形3发
        for (int i = 0; i < 3; i++) {
            BossBullet bullet;
            float offsetX = (i - 1) * 40.0f;  // -40, 0, 40

            bullet.x = boss.x + boss.width / 2 - bullet.width / 2 + offsetX;
            bullet.y = boss.y + boss.height;
            bullet.speedY = 300.0f + (rand() % 100);  // 随机速度

            bossBullets.push_back(bullet);
        }
        break;

    case 1:  // 密集5发
        for (int i = 0; i < 5; i++) {
            BossBullet bullet;
            float offsetX = (i - 2) * 25.0f;  // -50, -25, 0, 25, 50

            bullet.x = boss.x + boss.width / 2 - bullet.width / 2 + offsetX;
            bullet.y = boss.y + boss.height;
            bullet.speedY = 250.0f;

            bossBullets.push_back(bullet);
        }
        break;

    case 2:  // 追踪子弹（朝向玩家）
        BossBullet bullet;
        bullet.x = boss.x + boss.width / 2 - bullet.width / 2;
        bullet.y = boss.y + boss.height;

        // 计算朝向玩家的方向
        float dirX = (player.x + player.width / 2) - (boss.x + boss.width / 2);
        float dirY = (player.y + player.height / 2) - (boss.y + boss.height / 2);
        float length = sqrt(dirX * dirX + dirY * dirY);

        if (length > 0) {
            bullet.speedY = (dirY / length) * 250.0f;
        }
        else {
            bullet.speedY = 250.0f;
        }

        bossBullets.push_back(bullet);
        break;
    }
}


void SceneBoss::playerShoot() {
    // 检查冷却时间
    if (player.shootCooldown > 0 || isGameOver) return;

    // 设置冷却时间（0.2秒）
    player.shootCooldown = 0.2f;

    // 创建子弹
    PlayerBullet bullet;
    bullet.x = player.x + player.width / 2 - bullet.width / 2;
    bullet.y = player.y;

    playerBullets.push_back(bullet);
}

void SceneBoss::render() {
    //明确设置清屏颜色
    SDL_SetRenderDrawColor(game.getRenderer(), 0, 0, 0, 255);  // 设置为黑色
    SDL_RenderClear(game.getRenderer());

    // 绘制背景
    if (backgroundTexture) {
        SDL_RenderCopy(game.getRenderer(), backgroundTexture, NULL, NULL);
    }

    // 绘制Boss
    if (bossTexture) {
        SDL_Rect bossRect = { (int)boss.x, (int)boss.y, boss.width, boss.height };
        SDL_RenderCopy(game.getRenderer(), bossTexture, NULL, &bossRect);
    }

    // 绘制玩家
    if (playerTexture) {
        SDL_Rect playerRect = { (int)player.x, (int)player.y, player.width, player.height };
        SDL_RenderCopy(game.getRenderer(), playerTexture, NULL, &playerRect);
    }

    // 绘制Boss子弹
    if (bossBulletTexture) {
        for (const auto& bullet : bossBullets) {
            if (bullet.active) {
                SDL_Rect bulletRect = { (int)bullet.x, (int)bullet.y, bullet.width, bullet.height };
                SDL_RenderCopy(game.getRenderer(), bossBulletTexture, NULL, &bulletRect);
            }
        }
    }

    // 绘制玩家子弹
    if (playerBulletTexture) {
        for (const auto& bullet : playerBullets) {
            if (bullet.active) {
                SDL_Rect bulletRect = { (int)bullet.x, (int)bullet.y, bullet.width, bullet.height };
                SDL_RenderCopy(game.getRenderer(), playerBulletTexture, NULL, &bulletRect);
            }
        }
    }

    // 绘制Boss生命条背景
    SDL_SetRenderDrawColor(game.getRenderer(), 100, 100, 100, 255);
    SDL_Rect bossHealthBg = { 50, 20, 300, 20 };
    SDL_RenderFillRect(game.getRenderer(), &bossHealthBg);

    // 绘制Boss生命条
    SDL_SetRenderDrawColor(game.getRenderer(), 255, 0, 0, 255);
    float bossHealthPercent = static_cast<float>(boss.health) / boss.maxHealth;
    SDL_Rect bossHealth = {
        52, 22,
        static_cast<int>(296 * bossHealthPercent), 16
    };
    SDL_RenderFillRect(game.getRenderer(), &bossHealth);

    // 绘制玩家生命条背景
    SDL_SetRenderDrawColor(game.getRenderer(), 100, 100, 100, 255);
    SDL_Rect playerHealthBg = { 50, 60, 300, 20 };
    SDL_RenderFillRect(game.getRenderer(), &playerHealthBg);

    // 绘制玩家生命条
    SDL_SetRenderDrawColor(game.getRenderer(), 0, 255, 0, 255);
    float playerHealthPercent = static_cast<float>(player.health) / player.maxHealth;
    SDL_Rect playerHealth = {
        52, 62,
        static_cast<int>(296 * playerHealthPercent), 16
    };
    SDL_RenderFillRect(game.getRenderer(), &playerHealth);

    // ============= 修复2：在渲染文本前重置绘制颜色 =============
    SDL_SetRenderDrawColor(game.getRenderer(), 255, 255, 255, 255);  // 文本通常用白色

    // 绘制分数
    std::string scoreText = "Score: " + std::to_string(score);
    game.renderTextPos(scoreText, game.getWindowWidth() - 200, 20);

    // 绘制游戏状态提示
    if (isGameOver) {
        if (boss.health <= 0) {
            game.renderTextCentered("Victory！", 0.4, true);
            game.renderTextCentered("+500 points", 0.5, false);
        }
        else {
            game.renderTextCentered("Game Over！", 0.4, true);
        }
        game.renderTextCentered("Return in 2 second...", 0.6, false);
    }
    else {
        game.renderTextCentered("BOSS Battle！", 0.1, true);
        game.renderTextCentered("Move with WASD", 0.9, false);
    }

    // ============= 修复3：在显示前重置为默认颜色 =============
    SDL_SetRenderDrawColor(game.getRenderer(), 0, 0, 0, 255);  // 重置为黑色

    // 更新显示
    SDL_RenderPresent(game.getRenderer());
}



void SceneBoss::clean() {
    // 清理纹理
    if (bossTexture) {
        SDL_DestroyTexture(bossTexture);
        bossTexture = nullptr;
    }
    if (playerTexture) {
        SDL_DestroyTexture(playerTexture);
        playerTexture = nullptr;
    }
    if (bossBulletTexture) {
        SDL_DestroyTexture(bossBulletTexture);
        bossBulletTexture = nullptr;
    }
    if (playerBulletTexture) {
        SDL_DestroyTexture(playerBulletTexture);
        playerBulletTexture = nullptr;
    }
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    if (bossHealthBarTexture) {
        SDL_DestroyTexture(bossHealthBarTexture);
        bossHealthBarTexture = nullptr;
    }
    if (playerHealthBarTexture) {
        SDL_DestroyTexture(playerHealthBarTexture);
        playerHealthBarTexture = nullptr;
    }

    // 清空子弹列表
    bossBullets.clear();
    playerBullets.clear();
}


void SceneBoss::handleEvent(SDL_Event* event) {
    if (event->type == SDL_KEYDOWN) {
        switch (event->key.keysym.scancode) {
        case SDL_SCANCODE_J:  // 射击
            playerShoot();
            break;
        case SDL_SCANCODE_ESCAPE:  // 返回主场景
        {
            auto sceneMain = new SceneMain();
            game.changeScene(sceneMain);
        }
        break;
        }
    }
}




void SceneBoss::updatePlayer(float deltaTime) {
    // 获取按键状态
    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    float playerSpeed = 300.0f;  // 玩家移动速度（像素/秒）

    // 计算移动
    if (keystate[SDL_SCANCODE_W]) {
        player.y -= playerSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_S]) {
        player.y += playerSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_A]) {
        player.x -= playerSpeed * deltaTime;
    }
    if (keystate[SDL_SCANCODE_D]) {
        player.x += playerSpeed * deltaTime;
    }

    // 边界检查
    if (player.x < 0) player.x = 0;
    if (player.x > game.getWindowWidth() - player.width)
        player.x = game.getWindowWidth() - player.width;
    if (player.y < game.getWindowHeight() / 2)
        player.y = game.getWindowHeight() / 2;
    if (player.y > game.getWindowHeight() - player.height)
        player.y = game.getWindowHeight() - player.height;
}

