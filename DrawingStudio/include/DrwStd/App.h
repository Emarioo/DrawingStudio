#pragma once

#include "Engone/Application.h"
#include "DrwStd/Canvas.h"

class App : public engone::Application {
public:
    App(engone::Engone* engone);
    ~App() override {cleanup();}
    void cleanup();
    
    void update(engone::LoopInfo& info) override;
    void render(engone::LoopInfo& info) override;
    void onClose(engone::Window* window) override;
    void Input(engone::LoopInfo& info);
    
    engone::Window* mainWindow=nullptr;
    
    Canvas canvas;
        
    bool doDraw=false, move=false;
	int lastmx=0, lastmy=0;
    float last=0;
    float lastDrawX = -1, lastDrawY = -1;
    
    engone::Shader* hueShader=0;
    
    static const int PICK_COLOR_BRUSH=0;
    static const int PICK_COLOR_BACKGROUND=1;
    int pickColorType=PICK_COLOR_BRUSH;

    void renderHue(engone::LoopInfo& info, float x, float y, float w, float h);
    
    union {
        float depictionBorders[4]{0};
        struct {
            float depictionYH;
            float depictionXW;
            float depictionY;
            float depictionX;
        };
    };
    
    engone::ui::TextBox saveLoadPath{};
    static const int PROMPT_SAVE=1;
    static const int PROMPT_LOAD=2;
    static const int PROMPT_DEPICT=3;
    static const int PROMPT_DEPICT_SAVE=4;
    static const int SELECT_HUE=5;
    static const int SELECT_FADE=6;
    static const int SELECT_ALPHA=7;
    void switchState(int state);
    int promptType=0;
    
    engone::ui::Color successColor = {0.01,1,0.04,1}; 
    engone::ui::Color failColor = {1,0.04,0.01,1};
    
    engone::ui::Color highTextColor = {1,0.72,0.08,1};
    engone::ui::Color textColor = {1,0.7,0.01,1};
    engone::ui::Color promptColor = {1,0.5,0.1,1};
    
    void notify(const std::string& text, engone::ui::Color color);
    float notifyTime=0;
    engone::ui::TextBox notification{};
};