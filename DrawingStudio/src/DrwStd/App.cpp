#include "DrwStd/App.h"
#include "Engone/Utilities/Utilities.h"

static const char* basic = {
#include "DrwStd/Shaders/basic.glsl"
	};
	static const char* colorGLSL = {
#include "DrwStd/Shaders/color.glsl"
	};

#include "Engone/Tests/BasicRendering.h"

App::App(engone::Engone* engone) : Application(engone) {
    using namespace engone;
    mainWindow = createWindow({engone::WindowMode::ModeWindowed,800,600});
    mainWindow->setTitle("For coders, by coders");
    AssetStorage* as = mainWindow->getStorage();
    as->setRoot("../DrawingStudio/assets/");
    as->load<FontAsset>("consolas");
    
    as->set<ShaderAsset>("basic",new ShaderAsset(basic));
    
    as->load<TextureAsset>("colorMarker");
    
    hueShader = new Shader(colorGLSL);
    
    canvas.init(mainWindow);
    
    canvas.load("autosave.drw");
    
    mainWindow->attachListener(new Listener(EventClick | EventMove | EventScroll,-10, [this](Event& e) {
        //std::cout << e.mx << " " << e.my << " " << e.action << " " << e.button << "\n";
        // if (e.eventType == EventClick) {
        //     if (e.button == GLFW_MOUSE_BUTTON_1) {
        //         doDraw = e.action == 1;
            
        //         if (e.action==1) {
        //             if(promptType==0){
        //                 canvas.submitUndo();
        //                 canvas.drawPixel(e.mx,e.my);
        //             }
        //             lastmx = e.mx;
        //             lastmy = e.my;
        //         }
        //     }
        //     else if (e.button == GLFW_MOUSE_BUTTON_2) {
        //         //move = e.action == 1;
        //     }
        //     else if (e.button == GLFW_MOUSE_BUTTON_3) {
            // if(promptType==0)
        //         move = e.action == 1;
        //         lastmx = e.mx;
        //         lastmy = e.my;
        //     }
        // }
        // else 
        if (e.eventType == EventMove) {
            if(doDraw){
                if(promptType==0){
                    canvas.drawFromTo(lastmx,lastmy,e.mx,e.my);
                }
                lastmx = e.mx;
                lastmy = e.my;
            }
            if (move) {
                canvas.offsetX += (e.mx - lastmx) / canvas.zoomFactor;
                canvas.offsetY -= (e.my - lastmy) / canvas.zoomFactor;
                lastmx = e.mx;
                lastmy = e.my;
            }
        }
        else if (e.eventType == EventScroll) {
            if(mainWindow->isKeyDown(GLFW_KEY_LEFT_CONTROL)){
                if ((int)e.scrollY  == e.scrollY) {
                    float lastZoom = canvas.zoomFactor;
                    canvas.zoomFactor *= (e.scrollY > 0 ? pow(1.1, abs(e.scrollY)) : pow(0.9, abs(e.scrollY)));
                    
                    // Any more zoom will allow you to see the space between particles.
                    // if(canvas.zoomFactor>1)
                    //     canvas.zoomFactor=1;
                        
                    canvas.offsetX += ((e.mx - mainWindow->getWidth() / 2) / canvas.zoomFactor - (e.mx - mainWindow->getWidth() / 2) / lastZoom);
                    canvas.offsetY -= ((e.my - mainWindow->getHeight() / 2) / canvas.zoomFactor - (e.my - mainWindow->getHeight() / 2) / lastZoom);
                } else {
                    // What is this?
                    canvas.offsetX += e.scrollX*18/canvas.zoomFactor;
                    canvas.offsetY -= e.scrollY*18/canvas.zoomFactor;
                }
            }else{
                float size = canvas.brushSize;
                size = size + e.scrollY;
                if(size<1) size = 1;
                canvas.setBrushSize(size);
            }
        }
        return EventNone;
    }));
}
void App::cleanup(){
    canvas.save("autosave.drw");
}
void App::Input(engone::LoopInfo& info){
    if(info.window->isKeyDown(GLFW_KEY_LEFT_CONTROL) && info.window->isKeyPressed(GLFW_KEY_Z)){
        canvas.undo();
    }
    if(info.window->isKeyDown(GLFW_KEY_LEFT_CONTROL) && info.window->isKeyPressed(GLFW_KEY_R)){
        canvas.redo();
    }
    if(info.window->isKeyDown(GLFW_KEY_LEFT_CONTROL) && info.window->isKeyPressed(GLFW_KEY_P)){
        canvas.pruneAlgorithm();
    }
}

void App::update(engone::LoopInfo& info) {
    Input(info);
}
void App::notify(const std::string& text, engone::ui::Color color){
    notification.text = text;
    notification.rgba = color;
    notifyTime = 2;
}
void HSVToRGB(float hsv[3], float rgb[3]) {
    if (hsv[0]*6 <= 1) {
        rgb[0] = 1;
        rgb[1] = hsv[0]*6;
        rgb[2] = 0;
    } else if (hsv[0]*6 <= 2) {
        rgb[0] = 2-hsv[0]*6;
        rgb[1] = 1;
        rgb[2] = 0;
    } else if (hsv[0]*6 <= 3) {
        rgb[0] = 0;
        rgb[1] = 1;
        rgb[2] = hsv[0]*6-2;
    } else if (hsv[0]*6 <= 4) {
        rgb[0] = 0;
        rgb[1] = 4-hsv[0]*6;
        rgb[2] = 1;
    } else if (hsv[0]*6 <= 5) {
        rgb[0] = hsv[0]*6-4;
        rgb[1] = 0;
        rgb[2] = 1;
    } else if (hsv[0]*6 <= 6) {
        rgb[0] = 1;
        rgb[1] = 0;
        rgb[2] = 6-hsv[0]*6;
    }
    rgb[0] = (1 + (rgb[0]-1) * hsv[1]) * hsv[2];
    rgb[1] = (1 + (rgb[1]-1) * hsv[1]) * hsv[2];
    rgb[2] = (1 + (rgb[2]-1) * hsv[1]) * hsv[2];
    
    for(int i=0;i<3;i++){
        if(rgb[i]>1) rgb[i] = 1;
        if(rgb[i]<0) rgb[i] = 0;
    }
}
void RGBToHSV(float rgb[3],float hsv[3]) {
    float max = 0;
    float min = 1;
    for(int i=0;i<3;i++){
        if(rgb[i]>max) max = rgb[i];
        if(rgb[i]<min) min = rgb[i];
    }
    if(min<0) min = 0;
    if(max>1) max = 1;
    if(max!=0) hsv[1] = (max-min)/max;
    else hsv[1] = 0;
    hsv[2] = max;
    
    if(max==min){
        hsv[0] = 0;
    }else{
        float red = rgb[0]/(max-min), green = rgb[1]/(max-min), blue=rgb[2]/(max-min);
        if(rgb[0]==max){
             hsv[0] = (fmodf(6+green-blue,6))/6;
        }
        if(rgb[1]==max) hsv[0] = (blue-red+2)/6;
        if(rgb[2]==max){
            hsv[0] = (red-green+4)/6;
        }
        // engone::log::out << "rgb : "<<red << ", "<<green<<", "<<blue<<"\n";
        // engone::log::out << "min, max, hue : "<<min << ", "<<max<<", "<<hsv[0] << " " << hsv[1]<<" " <<hsv[2]<<"\n";
    }
}
void App::renderHue(engone::LoopInfo& info, float x, float y, float w, float h){
    using namespace engone;
    float fadeX = x;
    float fadeY = y;
    float fadeW = w;
    float fadeH = h*0.8;
    float hueX = fadeX;
    float hueY = fadeY + fadeH;
    float hueW = fadeW;
    float hueH = (h-fadeH)/2;
    float alphaX = fadeX;
    float alphaY = hueY + hueH;
    float alphaW = fadeW;
    float alphaH = (h-fadeH)/2;

    float rgbColor[3];
    float alpha;
    if(pickColorType==PICK_COLOR_BRUSH){
        rgbColor[0] = canvas.brushColor.r;
        rgbColor[1] = canvas.brushColor.g;
        rgbColor[2] = canvas.brushColor.b;
        alpha = canvas.brushColor.a;
    }else {
        rgbColor[0] = canvas.backgroundColor.r;
        rgbColor[1] = canvas.backgroundColor.g;
        rgbColor[2] = canvas.backgroundColor.b;
        alpha = canvas.backgroundColor.a;
    }
    float hsvColor[3];
    float& hue=hsvColor[0];
    float& saturation=hsvColor[1];
    float& value=hsvColor[2];
    RGBToHSV(rgbColor,hsvColor);
    
    hueShader->bind();
    hueShader->setVec2("uWindow", { info.window->getWidth(),info.window->getHeight() });
    float pureHue[]{hsvColor[0],1,1};
    HSVToRGB(pureHue,rgbColor);
    hueShader->setVec3("uColor", { rgbColor[0],rgbColor[1],rgbColor[2]});
    
    hueShader->setVec2("uPos", {fadeX,fadeY});
    hueShader->setVec2("uSize", { fadeW,fadeH});
    hueShader->setInt("uShaderType", 0);
    
    info.window->getCommonRenderer()->drawQuad(info);
    
    hueShader->setVec2("uPos", { hueX,hueY});
    hueShader->setVec2("uSize", { hueW, hueH });
    hueShader->setInt("uShaderType", 1);
    info.window->getCommonRenderer()->drawQuad(info);

    hueShader->setVec2("uPos", { alphaX,alphaY});
    hueShader->setVec2("uSize", { alphaW,alphaH});
    hueShader->setInt("uShaderType", 2);
    info.window->getCommonRenderer()->drawQuad(info);

    if(info.window->isKeyReleased(GLFW_MOUSE_BUTTON_1)){
        if(promptType==SELECT_FADE||promptType==SELECT_HUE||promptType==SELECT_ALPHA){
            switchState(0);   
        }
    }
    float mx = info.window->getMouseX();
    float my = info.window->getMouseY();
    if(info.window->isKeyPressed(GLFW_MOUSE_BUTTON_1)){
        if (fadeX<mx&&mx<fadeX+fadeW) {
            if (fadeY < my && my<fadeY+fadeH) {
                switchState(SELECT_FADE);
            } else if (hueY < my && my<hueY+hueH) {
                switchState(SELECT_HUE);
            } else if (alphaY< my && my < alphaY+alphaH) {
                switchState(SELECT_ALPHA);
            }
        }
    }
    
    if (promptType==SELECT_FADE) {
        saturation = 1-(mx - fadeX) / (fadeW);
        value = 1-(my - fadeY) / (fadeH);
        if (saturation < 0)
            saturation = 0;
        if (saturation > 1) {
            saturation = 1;
        }
        if (value < 0)
            value = 0;
        if (value > 1) {
            value = 1;
        }
    }
    if (promptType==SELECT_HUE) {
        hue = (mx - hueX) / (hueW);
        if (hue < 0)
            hue = 0;
        const float hueMax=0.999; // 1.0 is converted to 0.0. The marker will jump to the left side.
        if (hue > hueMax) {
            hue = hueMax;
        }
    }
    if (promptType==SELECT_ALPHA) {
        alpha = (mx - alphaX) / (alphaW);
        if (alpha < 0)
            alpha = 0;
        if (alpha > 1) {
            alpha = 1;
        }
    }
    HSVToRGB(hsvColor, rgbColor);
    
    if(pickColorType==PICK_COLOR_BRUSH){
        canvas.setBrushColor({rgbColor[0], rgbColor[1], rgbColor[2], alpha});
    }else if(pickColorType==PICK_COLOR_BACKGROUND){
        canvas.setBackgroundColor({rgbColor[0], rgbColor[1], rgbColor[2], alpha});
    }
    
    ui::TexturedBox mark0 = {};
    mark0.texture = &info.window->getStorage()->get<TextureAsset>("colorMarker")->texture;
    float tw = mark0.texture->getWidth();
    float th = mark0.texture->getHeight();
    mark0.x = fadeX+fadeW*(1-saturation)-tw/2;
    mark0.y = fadeY+fadeH*(1-value)-tw/2;
    mark0.w = tw;
    mark0.h = th;
    mark0.rgba = {1,1,1,1};
    ui::Draw(mark0);
    
    mark0.x = hueX+ hueW * hue-tw/2;
    mark0.y = hueY + hueH/2-th/2;
    ui::Draw(mark0);
    
    mark0.x = alphaX + alphaW * alpha - tw / 2;
    mark0.y = alphaY + alphaH / 2 - th / 2;
    ui::Draw(mark0);
}
void App::switchState(int state){
    promptType = state;
    saveLoadPath.editing=true;
}
void App::render(engone::LoopInfo& info) {
    using namespace engone;
    
    glClearColor(canvas.backgroundColor.r,canvas.backgroundColor.g,canvas.backgroundColor.b,canvas.backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // if(info.window->isKeyPressed(GLFW_MOUSE_BUTTON_1)){
    
    //     lastmx = info.window->getMouseX();
    //     lastmy = info.window->getMouseY();
    //     if(promptType==0){
    //         doDraw = true;
    //         canvas.submitUndo();
    //         canvas.drawPixel(lastmx,lastmy);
    //     }
    // }else if(info.window->isKeyPressed(GLFW_MOUSE_BUTTON_3)){
    //     lastmx = info.window->getMouseX();
    //     lastmy = info.window->getMouseY();
    //     if(promptType==0){
    //         move = true;
    //     }
    // }
    // if(info.window->isKeyReleased(GLFW_MOUSE_BUTTON_1)){
    //     doDraw = false;   
    // }
    // if(info.window->isKeyReleased(GLFW_MOUSE_BUTTON_3)){
    //     move=false;   
    // }
    
    FontAsset* consolas = info.window->getStorage()->get<FontAsset>("consolas");
    
    ui::TextBox toolType={"Size: "+std::to_string((int)canvas.brushSize),0,0,20,consolas,textColor};
    toolType.y=5;
    toolType.x=5;
    ui::Draw(toolType);
    
    char zoomT[30];
    sprintf(zoomT,"Zoom: %.1f",1/canvas.zoomFactor);
    ui::TextBox zoomText={zoomT,0,0,20,consolas,textColor};
    zoomText.y=toolType.h+toolType.y;
    zoomText.x=5;
    ui::Draw(zoomText);
    
    ui::TextBox partText={"Pixels: "+std::to_string(canvas.particleCount)+"/"+std::to_string(canvas.maxParticles),0,0,20,consolas,textColor};
    partText.x=300;
    partText.y=5;
    ui::Draw(partText);
    
    char mem[10];
    FormatBytes(mem,10,canvas.getUsedMemory());
    ui::TextBox memText={mem,0,0,20,consolas,textColor};
    memText.x=info.window->getWidth()-memText.getWidth()-5;
    memText.y=5;
    ui::Draw(memText);
    
    ui::TextBox historyText={"Undo: "+std::to_string(canvas.countHistory.size()),0,0,20,consolas,textColor};
    historyText.x=info.window->getWidth()-historyText.getWidth()-5;
    historyText.y=memText.y+memText.h;
    ui::Draw(historyText);
    
    if(info.window->isKeyDown(GLFW_KEY_LEFT_CONTROL) && info.window->isKeyDown(GLFW_KEY_LEFT_SHIFT) &&info.window->isKeyPressed(GLFW_KEY_S)){
        switchState(PROMPT_SAVE);
    } else if(info.window->isKeyDown(GLFW_KEY_LEFT_CONTROL) && info.window->isKeyPressed(GLFW_KEY_S)){
        std::string oldtxt = saveLoadPath.text;
        saveLoadPath.text = saveLoadPath.text + ".drw";
        if(FileExist(saveLoadPath.text)){
            switchState(0); // stop prompt if it is open?
            bool yes = canvas.save(saveLoadPath.text); // Todo: saving can take time. Frame will drop.
            if(yes)
                notify("Saved '"+saveLoadPath.text+"'",successColor);
            else
                notify("Could not saved '"+saveLoadPath.text+"'",failColor);
        }else{
            switchState(PROMPT_SAVE);
        }
        saveLoadPath.text = oldtxt;
    }
    if(info.window->isKeyDown(GLFW_KEY_LEFT_CONTROL) && info.window->isKeyPressed(GLFW_KEY_L)){
        switchState(PROMPT_LOAD);
    }
    if(info.window->isKeyDown(GLFW_KEY_LEFT_CONTROL) && info.window->isKeyPressed(GLFW_KEY_E)){
        switchState(PROMPT_DEPICT);
        canvas.computeBorder(depictionBorders);
    }
    
    if(info.window->isKeyPressed(GLFW_KEY_ESCAPE)){
        switchState(0);
    }
    
    float sw = info.window->getWidth();
    float sh = info.window->getHeight();
    
    if(promptType==PROMPT_DEPICT_SAVE){
        ui::TextBox saveText = {"Save path?",0,0,30,consolas,promptColor};
        saveText.x = sw/2-saveText.getWidth()/2;
        saveText.y = sh/3;
        
        ui::Edit(&saveLoadPath,true);
        std::string oldtxt = saveLoadPath.text;
        saveLoadPath.text = saveLoadPath.text + ".png";
        if(!saveLoadPath.editing){
            switchState(0);
            bool yes = canvas.savePng(saveLoadPath.text,0,0,depictionBorders); // Todo: saving can take time. Frame will drop.
            if(yes)
                notify("Saved '"+saveLoadPath.text+"'",successColor);
            else
                notify("Could not save '"+saveLoadPath.text+"'",failColor);
        }
        
        saveLoadPath.h = saveText.h-3;
        saveLoadPath.x = sw/2-saveLoadPath.getWidth()/2;
        saveLoadPath.y = saveText.y+saveLoadPath.h;
        saveLoadPath.font = consolas;
        saveLoadPath.rgba = {1,0.9,1,1};
        
        ui::Draw(saveLoadPath);
        ui::Draw(saveText);
        saveLoadPath.text = oldtxt;
    }
    if(promptType==PROMPT_DEPICT){
        float x=canvas.fromCanvasX(depictionX),y=canvas.fromCanvasY(depictionY),xw=canvas.fromCanvasX(depictionXW),yh=canvas.fromCanvasY(depictionYH);
        info.window->getUIRenderer()->drawLine(x,yh,xw,yh,{1,0,0,1});
        info.window->getUIRenderer()->drawLine(xw,yh,xw,y,{1,0,0,1});
        info.window->getUIRenderer()->drawLine(x,y,xw,y,{1,0,0,1});
        info.window->getUIRenderer()->drawLine(x,y,x,yh,{1,0,0,1});
        if(info.window->isKeyPressed(GLFW_KEY_ENTER)){
            switchState(PROMPT_DEPICT_SAVE);
        }
    }
    
    if(promptType==PROMPT_SAVE){
        ui::TextBox saveText = {"Save path?",0,0,30,consolas,promptColor};
        saveText.x = sw/2-saveText.getWidth()/2;
        saveText.y = sh/3;
        
        ui::Edit(&saveLoadPath,true);
        std::string oldtxt = saveLoadPath.text;
        saveLoadPath.text = saveLoadPath.text + ".drw";
        if(!saveLoadPath.editing){
            switchState(0);
            bool yes = canvas.save(saveLoadPath.text); // Todo: saving can take time. Frame will drop.
            if(yes)
                notify("Saved '"+saveLoadPath.text+"'",successColor);
            else
                notify("Could not save '"+saveLoadPath.text+"'",failColor);
        }
        
        saveLoadPath.h = saveText.h-3;
        saveLoadPath.x = sw/2-saveLoadPath.getWidth()/2;
        saveLoadPath.y = saveText.y+saveLoadPath.h;
        saveLoadPath.font = consolas;
        saveLoadPath.rgba = {1,0.9,1,1};
        
        ui::Draw(saveLoadPath);
        ui::Draw(saveText);
        saveLoadPath.text = oldtxt;
    }
    if(promptType==PROMPT_LOAD){
        ui::TextBox loadText = {"Load path?",0,0,30,consolas,promptColor};
        loadText.x = sw/2-loadText.getWidth()/2;
        loadText.y = sh/3;
        
        ui::Edit(&saveLoadPath,true);
        std::string oldtxt = saveLoadPath.text;
        saveLoadPath.text = saveLoadPath.text + ".drw";
        if(!saveLoadPath.editing){
            switchState(0);
            bool yes = canvas.load(saveLoadPath.text); // Todo: saving can take time. Frame will drop.
            if(yes)
                notify("Loaded '"+saveLoadPath.text+"'",successColor);
            else
                notify("Could not load '"+saveLoadPath.text+"'",failColor);
        }
        saveLoadPath.h = loadText.h-3;
        saveLoadPath.x = sw/2-saveLoadPath.getWidth()/2;
        saveLoadPath.y = loadText.y+saveLoadPath.h;
        saveLoadPath.font = consolas;
        saveLoadPath.rgba = {1,0.85,1,1};
        
        ui::Draw(saveLoadPath);
        ui::Draw(loadText);
        saveLoadPath.text = oldtxt;
    }
    
    if(notifyTime>0){
        notification.h = 20;
        notification.x = 5;
        notification.y = sh-notification.h;
        if(notifyTime<0.5)
            notification.rgba.a = notifyTime/0.5;
        notification.font = consolas;
        ui::Draw(notification);
    }
    notifyTime-=info.timeStep;
    if(notifyTime<0)notifyTime=0;
    
    
    // Render images
    ShaderAsset* shader = info.window->getStorage()->get<ShaderAsset>("basic");
    shader->bind();
    shader->setVec2("uOffset", {canvas.offsetX, canvas.offsetY});
    shader->setVec2("uTransform", {canvas.zoomFactor / info.window->getWidth(), canvas.zoomFactor /info.window->getHeight()});

    // for (int i = 0; i < images.size();i++) {
    //     images[i]->Render(info);
    // }
    
    float colorPickerHeight=info.window->getHeight()*.3;
    float colorPickerWidth=info.window->getWidth()*.3;
    if(colorPickerHeight>200)
        colorPickerHeight=200;
    if(colorPickerWidth>160)
        colorPickerWidth=160;
    if(colorPickerHeight<100)
        colorPickerHeight=100;
    if(colorPickerWidth<80)
        colorPickerWidth=80;
    int hueY = sh-colorPickerHeight;
    
    ui::TextBox brushText={"Brush ",0,0,25,consolas,textColor};
    brushText.x = 0;
    brushText.y = hueY-brushText.h;
    if(pickColorType==PICK_COLOR_BRUSH){
        brushText.rgba = highTextColor;  
    }
    ui::Draw(brushText);
    
    ui::TextBox backText={"Background",0,0,25,consolas,textColor};
    backText.x = brushText.x+brushText.getWidth();
    backText.y = brushText.y;
     if(pickColorType==PICK_COLOR_BACKGROUND){
        backText.rgba = highTextColor;  
    }
    ui::Draw(backText);
    
    if(ui::Clicked(brushText)==1){
        pickColorType = PICK_COLOR_BRUSH;   
    } else if(ui::Clicked(backText)==1){
        pickColorType = PICK_COLOR_BACKGROUND;   
    }
    canvas.render(info);
    renderHue(info,0,hueY,colorPickerWidth,colorPickerHeight);
    
    if(info.window->isKeyPressed(GLFW_MOUSE_BUTTON_1)){
    
        lastmx = info.window->getMouseX();
        lastmy = info.window->getMouseY();
        if(promptType==0){
            doDraw = true;
            canvas.submitUndo();
            canvas.drawPixel(lastmx,lastmy);
        }
    }else if(info.window->isKeyPressed(GLFW_MOUSE_BUTTON_3)){
        lastmx = info.window->getMouseX();
        lastmy = info.window->getMouseY();
        if(promptType==0){
            move = true;
        }
    }
    if(info.window->isKeyReleased(GLFW_MOUSE_BUTTON_1)){
        doDraw = false;   
    }
    if(info.window->isKeyReleased(GLFW_MOUSE_BUTTON_3)){
        move=false;   
    }
}

void App::onClose(engone::Window* window){
    stop();   
}