#pragma once

#include "DrwStd/Canvas.h"

#include "Engone/InputModule.h"
#include "Engone/UIModule.h"
#include "Engone/Assets/AssetModule.h"
#include "Engone/Util/Perf.h"
#include "Engone/Rendering/Buffer.h"
#include "Engone/Asserts.h"
#include "Engone/Util/Array.h"

typedef void (*AppProcedure)(App* app);
struct App {
    App() {}

    static App* Create();
    static void Destroy(App* app);

    GLFWwindow* window = nullptr;
    float winWidth, winHeight;
    engone::InputModule inputModule;
    engone::UIModule uiModule;
    // engone::AssetStorage assetStorage;
    engone::LinearAllocator stringAllocator;
    
    bool isRunning = false;
    float update_deltaTime = 1.0f/60.0f; // fixed
    float current_frameTime = 1.0f/60.0f;
    double game_runtime = 0;
    AvgValue<double> avg_frameTime{30};
    AvgValue<float> avg_updateTime{30};
    
    AppProcedure activeUpdateProc = nullptr;
    AppProcedure activeRenderProc = nullptr;
    AppProcedure inactiveUpdateProc = nullptr;
    AppProcedure inactiveRenderProc = nullptr;

    Canvas canvas;
    
    std::string workingDirectory;
    
    int drawType=0;
    bool doDraw=false, move=false;
	int lastmx=0, lastmy=0;
    float last=0;
    float lastDrawX = -1, lastDrawY = -1;
    
    float extraEraseSize = 3.5;
    
    float undoRedoTime = 0;
    float undoRedoDelay = 0.12;
    
    float maximumZoom=500;
    float minimumZoom=0.04;
    
    // float winCoords[4];
    
    engone::Shader* hueShader=0;
    engone::Shader* basicShader=0;
    int colorMarker_textureId = 0;
    
    static const int PICK_COLOR_BRUSH=0;
    static const int PICK_COLOR_BACKGROUND=1;
    int pickColorType=PICK_COLOR_BRUSH;

    bool showHelp = false;
    
    union {
        float depictionBorders[4]{0};
        struct {
            float depictionYH;
            float depictionXW;
            float depictionY;
            float depictionX;
        };
    };
    
    enum PromptState : int {
        PROMPT_SAVE=1,
        PROMPT_LOAD=2,
        PROMPT_DEPICT=3,
        PROMPT_DEPICT_SAVE=4,
        PROMPT_LOAD_PNG=5,
        
        SELECT_HUE=10,
        SELECT_FADE=11,
        SELECT_ALPHA=12,
    };
    void switchState(int state);
    int promptType=0;
    
    int selectedEdgeX=0;
    int selectedEdgeY=0;
    
    float selectedOffsetX=0;
    float selectedOffsetY=0;
    
    void renderHue(float x, float y, float w, float h);
    // engone::ui::TextBox saveLoadPath{};
    // engone::ui::TextBox maximumSizeText{};
    
    static const int saveLoadPath_id = 2354, maximumSizeText_id = 2315;
    engone::UIText* saveLoadPath = nullptr;
    engone::UIText* maximumSizeText = nullptr;
    
    engone::UIColor successColor = {0.01,1,0.04,1}; 
    engone::UIColor failColor = {1,0.04,0.01,1};
    engone::UIColor highTextColor = {1,0.72,0.08,1};
    engone::UIColor textColor = {1,0.7,0.01,1};
    engone::UIColor promptColor = {1,0.5,0.1,1};
    
    void notify(const std::string& text, engone::UIColor color);
    // float notifyTime=0;
    // float notifyDelay=2;
    
    struct Message {
        std::string text;
        engone::UIColor color;
        float time;
    };
    DynamicArray<Message> messages;
    
    // EXTRA
    engone::VertexBuffer quadVB{};
    engone::VertexArray quadVA{};
    // engone::IndexBuffer quadIB{};
    void initQuad() {
        float simpleQuad[]{
			0,1,0,1,
			1,1,1,1,
			0,0,0,0,
			1,0,1,0,
			0,0,0,0,
			1,1,1,1,

			//0,0,0,1,
			//0,1,0,0,
			//1,1,1,0,
			//1,0,1,1,
		};
		//uint32_t simpleInd[]{
		//	0, 1, 2, 2, 3, 0,
		//};
		quadVB.setData(sizeof(simpleQuad), simpleQuad);
		//quadIB.setData(sizeof(uint32_t)*6, simpleInd);
		quadVA.addAttribute(4, &quadVB);
    }
    
    
};

void StartApp();
void UpdateApp(App* app);
void RenderApp(App* app);