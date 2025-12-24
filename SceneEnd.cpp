#include "SceneEnd.h"
#include "SceneMain.h"
#include "Game.h"
#include <string>

void SceneEnd::init()
{
    // 载入背景音乐
    bgm = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    if (!bgm) {
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Failed to load music: %s", Mix_GetError());
    }
    Mix_PlayMusic(bgm, -1);

    if (!SDL_IsTextInputActive()) {
        SDL_StartTextInput();
    }
    if (!SDL_IsTextInputActive()) {
        SDL_LogError(SDL_LOG_PRIORITY_ERROR, "Failed to start text input: %s", SDL_GetError());
    }
}

void SceneEnd::update(float deltaTime)
{
    blinkTimer -= deltaTime;
    if (blinkTimer <= 0) {
        blinkTimer += 1.0f;
    }
}

void SceneEnd::render()
{
    if (isTyping) {
        renderPhase1();
    }
    else {
        renderPhase2();
    }
}

void SceneEnd::clean()
{
    if (bgm != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
    }
}

void SceneEnd::handleEvent(SDL_Event* event)
{
    if (isTyping) {
        if (event->type == SDL_TEXTINPUT) {
            name += event->text.text;
        }
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.scancode == SDL_SCANCODE_RETURN) {
                isTyping = false;
                SDL_StopTextInput();
                if (name == "") {
                    name = "Anonymous";//无名氏
                }
                game.insertLeaderBoard(game.getFinalScore(), name);
            }
            if (event->key.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
                removeLastUTF8Char(name);
            }
        }
    }
    else {
        if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.scancode == SDL_SCANCODE_J) {
                auto sceneMain = new SceneMain();
                game.changeScene(sceneMain);
            }
        }
    }
}

void SceneEnd::renderPhase1()
{
    auto score = game.getFinalScore();
    std::string scoreText = "Your score is :" + std::to_string(score);//你的得分是
    std::string gameOver = "Game Over";
    std::string instrutionText = "Type your name, then press Enter:";//请输入你的名字，按回车键确认
    
    game.renderTextCentered(scoreText, 0.1, false);
    game.renderTextCentered(gameOver, 0.4, true);
    game.renderTextCentered(instrutionText, 0.6, false);

    if (name != "") {
        SDL_Point p = game.renderTextCentered(name, 0.8, false);
        if (blinkTimer < 0.5) {
            game.renderTextPos("_", p.x, p.y);
        }
    }
    else {
        if (blinkTimer < 0.5) {
            game.renderTextCentered("_", 0.8, false);
        }
    }
}

void SceneEnd::renderPhase2()
{
    game.renderTextCentered("Scoreboard", 0.05, true);//得分榜
    auto posY = 0.2 * game.getWindowHeight();
    auto i = 1;
    for (auto item : game.getLeaderBoard()) {
        std::string name = std::to_string(i) + ". " + item.second;
        std::string score = std::to_string(item.first);
        game.renderTextPos(name, 100, posY);
        game.renderTextPos(score, 100, posY, false);
        posY += 45;
        i++;
    }
    if (blinkTimer < 0.5) {
        game.renderTextCentered("Press J to restart the game", 0.85, false);//按 J 键重新开始游戏
    }

}

void SceneEnd::removeLastUTF8Char(std::string& str)
{
    if (str.empty()) return;

    auto lastchar = str.back();
    if ((lastchar & 0b10000000) == 0b10000000) { // 中文字符的后续字节
        str.pop_back();
        while ((str.back() & 0b11000000) != 0b11000000) { // 判断是否是中文字符的第一个字节
            str.pop_back();
        }
    }
    str.pop_back();
}
