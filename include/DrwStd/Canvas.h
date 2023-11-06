#pragma once

#include "Engone/Rendering/Buffer.h"
struct App;

struct Pixel{
    char r,g,b,a;
};
struct Color {
    float r,g,b,a;  
};
struct Particle{
    float x,y;
    Color color;
    float s;
};
class Canvas {
public:
    Canvas() = default;
    ~Canvas(){cleanup();}
    void cleanup();
    
    // Canvas needs a reference to the window to know it's size.
    void init(App* app);
    
    
    // mx, my is mouse coordinates in screen pixels
    void drawPixel(float mx, float my);
    void drawRawPixel(float x, float y);
    void drawFromTo(float fromX, float fromY, float toX, float toY);
    
    void erasePixel(float mx, float my);    
    void eraseFromTo(float fromX, float fromY, float toX, float toY);
    
    std::vector<int> countHistory;
    int historyIndex=0;
    bool undo();
    bool redo();
    bool submitUndo();
    
    void render();
    
    u64 getUsedMemory();
    
    float toCanvasX(float x);
    float toCanvasY(float y);
    float fromCanvasX(float x);
    float fromCanvasY(float y);
    
    // Todo: prune algorithm
    // Todo: save/load data, with some compression
    
    bool save(const std::string& path);
    bool savePng(const std::string& path, int pixelWidth, int pixelHeight, float borders[4]);
    bool load(const std::string& path);
    bool loadPng(const std::string& path);
    
    void clear();
    
    float brushSize=3;
    void setBrushSize(float size);
    
    Color brushColor={0.5,1,0,1};
    void setBrushColor(Color color);
    
    // Color backgroundColor={1.,1.,1.,1};
    Color backgroundColor={0,0.03,0.08,1};
    void setBackgroundColor(Color color);
    
    // Will remove overlapping particles. Your history will also be deleted.
    static const char MARK_AVAILABLE=0;
    static const char MARK_DEL=1;
    static const char MARK_EMPTY=2;
    std::vector<char> markedParticles;
    void pruneAlgorithm();
    
    App* mainWindow=0;
    
    float zoomFactor=1.f;
    float offsetX=0;
    float offsetY=0;
    
    float pngMaxWidth = 2000;
    float pngMaxHeight = 2000;
    
    // outBorders is a list with top, right, bottom, left
    void computeBorder(float outBorders[4]);
    
    Particle* particles=0;
    u32 particleCount=0;
    u32 maxParticles=0;
    
    engone::Shader* shader=0;
    bool needsRefresh=false;
    engone::ShaderBuffer shaderBuffer{};
    
    engone::Shader* brushShader=0;
    
};