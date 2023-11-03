#include "DrwStd/App.h"
#include "Engone/Util/Utilities.h"

#include "DrwStd/resource.h"

static const char* basicGLSL = {
#include "DrwStd/Shaders/basic.glsl"
	};
static const char* colorGLSL = {
#include "DrwStd/Shaders/color.glsl"
	};

#include "Engone/Win32Includes.h"


App* App::Create() {
    return new App();
}
void App::Destroy(App* app) {
    delete app;
}

int RenderThread(App* app);
void StartApp() {
    App* app = App::Create();
    
    RenderThread(app);
    
    app->canvas.save("autosave.drw");
    
    App::Destroy(app);
}

int RenderThread(App* app) {
    using namespace engone;
    
    Assert(glfwInit());
 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
 
    GLFWwindow* window = glfwCreateWindow(800, 600, "Delightful particles", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return 0;
    }
 
    glfwMakeContextCurrent(window);
    glewInit();

    // if(gameState->locked_fps) 
        glfwSwapInterval(1); // limit fps to your monitors frequency?
    // else
    //     glfwSwapInterval(0);
        
    app->window = window;

    // SetupRendering(gameState);

    #define HOT_RELOAD_ORIGIN "bin/game.dll"
    #define HOT_RELOAD_IN_USE "bin/hotloaded.dll"
    #define HOT_RELOAD_ORIGIN_PDB "bin/game.pdb"
    #define HOT_RELOAD_IN_USE_PDB "bin/hotloaded.pdb"
    #define HOT_RELOAD_TIME 2

    app->activeUpdateProc = UpdateApp;
    app->activeRenderProc = RenderApp;
    app->inactiveUpdateProc = UpdateApp;
    app->inactiveRenderProc = RenderApp;

    void* prev_hot_reload_dll = nullptr;
    void* hot_reload_dll = nullptr;
    double last_dll_write = 0;
 
    auto lastTime = engone::StartMeasure();

    // glEnable(GL_BLEND);
    // glEnable(GL_CULL_FACE);
    // glEnable(GL_DEPTH_TEST);
    // glCullFace(GL_FRONT);
    
    PNG* consolas_png = PNG::Load(IDB_PNG1);
    PNG* colorMarker_png = PNG::Load(IDB_PNG2);
    
    RawImage* consolas_raw = PNGToRawImage(consolas_png);
    RawImage* colorMarker_raw = PNGToRawImage(colorMarker_png);
    
    int consolasId = app->uiModule.readRawFont((u8*)consolas_raw->data(), consolas_raw->width, consolas_raw->height, consolas_raw->channels);
    Assert(consolasId == 0);
    app->colorMarker_textureId = app->uiModule.readRawTexture((u8*)colorMarker_raw->data(), colorMarker_raw->width, colorMarker_raw->height, colorMarker_raw->channels);
    // app->uiModule.readRawTexture((u8*)colorMarker_raw->data(), colorMarker_raw->width, colorMarker_raw->height, colorMarker_raw->channels);
    app->uiModule.init(nullptr);
    app->inputModule.init(app->window);
    app->uiModule.enableInput(&app->inputModule);
    
    // TODO: MOVE THIS ELSEWHERE!
    // app->assetStorage.setRoot("../DrawingStudio/assets/");
    
    int len = GetCurrentDirectoryA(0,0);
    app->workingDirectory.resize(len);
    GetCurrentDirectoryA(len,(char*)app->workingDirectory.data());
    
    // printf("CWD: %s\n",workingDirectory.c_str());
    
    // Loading twice, very dumb, but engine is dumb so it's fine.
    // app->assetStorage.set<FontAsset>("default",new FontAsset(IDB_PNG1,"4\n35"));
    // app->assetStorage.set<FontAsset>("fonts/consolas42",new FontAsset(IDB_PNG1,"4\n35"));
    
    // app->assetStorage.set<TextureAsset>("colorMarker",new TextureAsset(IDB_PNG2));
    
    // app->assetStorage.set<ShaderAsset>("basic",new ShaderAsset(basic));
    app->basicShader = new Shader(basicGLSL);
    app->hueShader = new Shader(colorGLSL);
    
    app->canvas.init(app);
    app->canvas.load("autosave.drw");
    
    app->initQuad();

    // Loop
    auto gameStartTime = engone::StartMeasure();
    double updateAccumulation = 0;
    double sec_timer = 0;
    double reloadTime = 0;
    while (!glfwWindowShouldClose(window)) {    
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        app->winWidth = width;
        app->winHeight = height;
    
        auto now = engone::StartMeasure();
        double frame_deltaTime = DiffMeasure(now - lastTime);
        lastTime = now;
        app->current_frameTime = frame_deltaTime;
        
        app->avg_frameTime.addSample(frame_deltaTime);

        app->game_runtime = DiffMeasure(now - gameStartTime);

        glViewport(0, 0, width, height);
        glClearColor(0.33,0.2,0.33,1);
        glClear(GL_COLOR_BUFFER_BIT);

        #ifdef USE_HOT_RELOAD
        reloadTime -= frame_deltaTime;
        if(reloadTime <= 0) {
            reloadTime += HOT_RELOAD_TIME;
            // log::out << "Try reload\n";
            double new_dll_write = 0;
            bool yes = engone::FileLastWriteSeconds(HOT_RELOAD_ORIGIN, &new_dll_write);
            if(yes && new_dll_write > last_dll_write) {
                log::out << "Reloaded\n";
                last_dll_write = new_dll_write;
                if(hot_reload_dll) {
                    Assert(!prev_hot_reload_dll);
                    prev_hot_reload_dll = hot_reload_dll;
                    hot_reload_dll = nullptr;
                }
                // TODO: Is 256 enough?
                char dll_path[256]{0};
                char pdb_path[256]{0};
                snprintf(dll_path,sizeof(dll_path),"bin/hotloaded-%d.dll", (int)rand());
                snprintf(pdb_path,sizeof(pdb_path),"bin/hotloaded-%d.pdb", (int)rand());
                engone::FileCopy(HOT_RELOAD_ORIGIN, dll_path);
                engone::FileCopy(HOT_RELOAD_ORIGIN_PDB, pdb_path);
                // engone::FileCopy(HOT_RELOAD_ORIGIN, HOT_RELOAD_IN_USE);
                // engone::FileCopy(HOT_RELOAD_ORIGIN_PDB, HOT_RELOAD_IN_USE_PDB);
                hot_reload_dll = engone::LoadDynamicLibrary(dll_path);
                // hot_reload_dll = engone::LoadDynamicLibrary(HOT_RELOAD_IN_USE);

                gameState->inactiveUpdateProc = (GameProcedure)engone::GetFunctionPointer(hot_reload_dll, "UpdateGame");
                gameState->inactiveRenderProc = (GameProcedure)engone::GetFunctionPointer(hot_reload_dll, "RenderGame");
                gameState->activeRenderProc = gameState->inactiveRenderProc;
            }
        }
        // TODO: Mutex on game proc
        if(gameState->activeUpdateProc == gameState->inactiveUpdateProc && prev_hot_reload_dll) {
            engone::UnloadDynamicLibrary(prev_hot_reload_dll);
            prev_hot_reload_dll = nullptr;
        }
        #endif
        if(app->activeUpdateProc != app->inactiveUpdateProc) {
            app->activeUpdateProc = app->inactiveUpdateProc;
        }

        // for(auto& proc : app->assetStorage.getIOProcessors()) {
        //     proc->process();
        // }
        // for(auto& proc : app->assetStorage.getDataProcessors()) {
        //     proc->process();
        // }

        updateAccumulation += frame_deltaTime;
        if(app->activeUpdateProc){
            while(updateAccumulation>app->update_deltaTime){
                updateAccumulation-=app->update_deltaTime;
                app->activeUpdateProc(app);
            }
        }

        // for(auto& proc : app->assetStorage.getGraphicProcessors()) {
        //     proc->process();
        // }
        if(app->activeRenderProc) {
            app->activeRenderProc(app);
        }

        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        app->uiModule.render(app->winWidth, app->winHeight);

        app->inputModule.resetEvents(true);
        app->inputModule.resetPollChar();
        app->inputModule.m_lastMouseX = app->inputModule.m_mouseX;
        app->inputModule.m_lastMouseY = app->inputModule.m_mouseY;
 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    app->isRunning = false;
 
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;   
}
void UpdateApp(App* app) {
    float mx = app->inputModule.getMouseX(), my = app->inputModule.getMouseY();
    if(app->doDraw){
        if(app->promptType==0){
            if(app->drawType==0)
                app->canvas.drawFromTo(app->lastmx,app->lastmy,mx,my);
            else
                app->canvas.eraseFromTo(app->lastmx,app->lastmy,mx,my);
        }
        app->lastmx = mx;
        app->lastmy = my;
    }
    if (app->move) {
        app->canvas.offsetX += (mx - app->lastmx) / app->canvas.zoomFactor;
        app->canvas.offsetY -= (my - app->lastmy) / app->canvas.zoomFactor;
        app->lastmx = mx;
        app->lastmy = my;
    }
    float scrollY = app->inputModule.getScrollY();
    if(scrollY != 0) {
        if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL)
        // ||mainWindow->isKeyDown(GLFW_MOUSE_BUTTON_2)
        ){
            
            if ((int)scrollY  == scrollY) {
                float lastZoom = app->canvas.zoomFactor;
                app->canvas.zoomFactor *= (scrollY > 0 ? pow(1.1, abs(scrollY)) : pow(0.9, abs(scrollY)));
                
                if(1/app->canvas.zoomFactor<app->minimumZoom)
                    app->canvas.zoomFactor=1/app->minimumZoom;
                if(1/app->canvas.zoomFactor>app->maximumZoom)
                    app->canvas.zoomFactor=1/app->maximumZoom;
                    
                app->canvas.offsetX += ((mx - app->winWidth / 2) / app->canvas.zoomFactor - (mx - app->winWidth / 2) / lastZoom);
                app->canvas.offsetY -= ((my - app->winHeight / 2) / app->canvas.zoomFactor - (my - app->winHeight / 2) / lastZoom);
            } else {
                // What is this?
                // app->canvas.offsetX += scrollX*18/app->canvas.zoomFactor;
                app->canvas.offsetY -= scrollY*18/app->canvas.zoomFactor;
            }
        }else{
            float size = app->canvas.brushSize;
            float power = 0.05;
            if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_SHIFT)){
                power *= 3.1;
            }
            if(size>10){
                size *= (scrollY > 0 ? pow(1+power, abs(scrollY)) : pow(1-power, abs(scrollY)));
            }else{
                size = size + scrollY;
            }
            if(size<1) size = 1;
            app->canvas.setBrushSize(size);
        }
    }
}
void RenderApp(App* app) {
    using namespace engone;
    app->undoRedoTime-= app->current_frameTime;
    if(app->undoRedoTime<0){
        app->undoRedoTime = 0;   
    }
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyDown(GLFW_KEY_Z)){
        if(app->undoRedoTime==0){
            app->canvas.undo();
            app->undoRedoTime = app->undoRedoDelay;
        }
    }
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyDown(GLFW_KEY_R)){
        if(app->undoRedoTime==0){
            app->canvas.redo();
            app->undoRedoTime = app->undoRedoDelay;
        }
    }
    if(!app->inputModule.isKeyDown(GLFW_KEY_R)&&!app->inputModule.isKeyDown(GLFW_KEY_Z))
        app->undoRedoTime=0;   
    
    // Note: prune doesn't work yet
    // if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyPressed(GLFW_KEY_P)){
    //     canvas.pruneAlgorithm();
    // }
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL)&&app->inputModule.isKeyDown(GLFW_KEY_LEFT_SHIFT) && app->inputModule.isKeyPressed(GLFW_KEY_N)){
        app->canvas.clear();
    }
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_ALT)&&app->inputModule.isKeyPressed(GLFW_KEY_F4)){
        app->isRunning = false;
    }
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_ALT)&&app->inputModule.isKeyPressed(GLFW_KEY_ENTER)){
        if(glfwGetWindowAttrib(app->window, GLFW_MAXIMIZED)) {
            glfwRestoreWindow(app->window);
        } else {
            glfwMaximizeWindow(app->window);
        }
        // mainWindow->maximize(!(mainWindow->getX() == 0 && mainWindow->getY() >= 0 && mainWindow->getY() < 100));
        //if(mainWindow->getX()==0&&mainWindow->getY()>=0&&mainWindow->getY()<100){
        //    mainWindow->setPosition(winCoords[0],winCoords[1]);
        //    mainWindow->setSize(winCoords[2],winCoords[3]);
        //}else{
        //    winCoords[0] = mainWindow->getX();
        //    winCoords[1] = mainWindow->getY();
        //    winCoords[2] = mainWindow->getWidth();
        //    winCoords[3] = mainWindow->getHeight();
        //    mainWindow->maximize();
        //}
    }
    
    // glClearColor(app->canvas.backgroundColor.r,app->canvas.backgroundColor.g,app->canvas.backgroundColor.b,app->canvas.backgroundColor.a);
    glClearColor(0.3,0.3,0.3,1.f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // FontAsset* consolas = info.window->getStorage()->get<FontAsset>("default");
    
    UIModule* ui = &app->uiModule;

    char tempstr[200];
    int templen=0;
    UIText* text = nullptr;
    UILayout layout = ui->makeLayout();
    layout.x = 5;
    layout.y = 5;
    layout.textSize = 20;
    layout.textColor = app->textColor;
    
    
    templen = snprintf(tempstr, sizeof(tempstr),"Inverse Zoom: %.1f",1/app->canvas.zoomFactor);
    text = layout.makeText(tempstr, templen);
    
    templen = snprintf(tempstr, sizeof(tempstr),"Size: %d",(int)app->canvas.brushSize);
    text = layout.makeText(tempstr, templen);
    
    templen = FormatBytes(tempstr, sizeof(tempstr), app->canvas.getUsedMemory());
    text = layout.makeText(tempstr, templen);
    
    templen = snprintf(tempstr, sizeof(tempstr),"History: %d",(int)app->canvas.countHistory.size());
    text = layout.makeText(tempstr, templen);
    
    text = ui->makeText();
    text->color = app->textColor;
    text->h = 20;
    templen = snprintf(tempstr, sizeof(tempstr),"Pixels: %d/%d", (int)app->canvas.particleCount, (int)app->canvas.maxParticles);
    ui->setString(text, tempstr, templen);
    text->x = app->winWidth/2.f - ui->getWidthOfText(text)/2.f;
    text->y = 5;
    
    // text = ui->makeText(242);
    // if(text->h != 20) {
    //     text->x = 300;
    //     text->y = 300;
    //     text->h = 20;
    //     text->color = {1,1,1,1};
    // }
    // ui->edit(text, true);
    
    // ui::TextBox zoomText={zoomT,0,0,20,consolas,textColor};
    // zoomText.y=5;
    // zoomText.x=5;
    // ui::Draw(zoomText);

    
    // ui::TextBox toolType={"Size: "+std::to_string((int)canvas.brushSize),0,0,20,consolas,textColor};
    // toolType.y=zoomText.y+zoomText.h;
    // toolType.x=zoomText.x;
    // ui::Draw(toolType);
    
    // ui::TextBox partText={"Pixels: "+std::to_string(canvas.particleCount)+"/"+std::to_string(canvas.maxParticles),0,0,20,consolas,textColor};
    // partText.x=info.window->getWidth()/2-partText.getWidth()/2;
    // partText.y=5;
    // ui::Draw(partText);
    
    // char mem[10];
    // FormatBytes(mem,10,canvas.getUsedMemory());
    // ui::TextBox memText={mem,0,0,20,consolas,textColor};
    // memText.x=toolType.x;
    // memText.y=toolType.y+toolType.h;
    // ui::Draw(memText);
    
    // ui::TextBox historyText={"History: "+std::to_string(canvas.countHistory.size()),0,0,20,consolas,textColor};
    // historyText.x=memText.x;
    // historyText.y=memText.y+memText.h;
    // ui::Draw(historyText);
    // #ifdef gone
    float sw = app->winWidth;
    float sh = app->winHeight;
    
    UILayout msgLayout = ui->makeLayout();
    msgLayout.textSize = 20;
    msgLayout.x = sw;
    msgLayout.y = sw;
    msgLayout.alignment = UILayout::ALIGN_RIGHT;
    msgLayout.flow = UILayout::FLOW_UP;
    for(int i=0;i<app->messages.size();i++) {
        auto msg = &app->messages[i];
        
        msg->time -= app->current_frameTime;
        if(msg->time <= 0.f) {
            app->messages.removeAt(i);
            i--;
            continue;   
        }
        
        text = msgLayout.makeText(msg->text.c_str(), msg->text.length());
        text->color = msg->color;
        if(msg->time < 0.5f)
            text->color.a = msg->time / 0.5f;
    }
    // if(notifyTime==notifyDelay) // saving png can take 2 seconds. if so timeStep will be 2s and notification will disappear rigth away.
    //     notifyTime-=0.001;
    // else
    //     notifyTime-=info.timeStep;
    
    // if(!app->saveLoadPath) {
    //     app->saveLoadPath = ui->makeText(app->saveLoadPath_id);
    //     app->maximumSizeText = ui->makeText(app->maximumSizeText_id);
    // }
    
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyDown(GLFW_KEY_LEFT_SHIFT) &&app->inputModule.isKeyPressed(GLFW_KEY_S)){
        app->switchState(App::PROMPT_SAVE);
    } else if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyPressed(GLFW_KEY_S)){
        // std::string oldtxt = saveLoadPath.text;
        std::string pathText = app->saveLoadPath->string;
        pathText += ".drw";
        // saveLoadPath.text = saveLoadPath.text + ".drw";
        if(FileExist(pathText)){
            app->switchState(0); // stop prompt if it is open?
            bool yes = app->canvas.save(pathText); // Todo: saving can take time. Frame will drop.
            if(yes)
                app->notify("Saved '"+pathText+"'",app->successColor);
            else
                app->notify("Could not saved '"+pathText+"'",app->failColor);
        }else{
            app->switchState(App::PROMPT_SAVE);
        }
        // saveLoadPath.text = oldtxt;
    }
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyPressed(GLFW_KEY_L)){
        app->switchState(App::PROMPT_LOAD);
    }
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyPressed(GLFW_KEY_E)){
        app->switchState(App::PROMPT_DEPICT);
        app->canvas.computeBorder(app->depictionBorders);
    }
    
    if(app->inputModule.isKeyPressed(GLFW_KEY_ESCAPE)){
        app->switchState(0);
    }
    
    float promptHeight = app->winHeight*0.05f;
    
    if(app->promptType==App::PROMPT_DEPICT_SAVE){
        UILayout promptLayout = ui->makeLayout();
        promptLayout.x = sw/2.f;
        promptLayout.y = sh/3.f;
        promptLayout.alignment = UILayout::ALIGN_CENTER;
        promptLayout.textSize = promptHeight;
        promptLayout.textColor = app->promptColor;
        
        templen = snprintf(tempstr, sizeof(tempstr),"Export path?");
        text = promptLayout.makeText(tempstr, templen);
        
        app->saveLoadPath = promptLayout.makeText(tempstr, templen, 930);
        app->saveLoadPath->h = promptHeight-3;
        app->saveLoadPath->color = {1,0.9,1,1};
        
        templen = snprintf(tempstr, sizeof(tempstr),"Maximum size (ex, 123x456 or 200)");
        text = promptLayout.makeText(tempstr, templen);
        
        app->maximumSizeText = promptLayout.makeText(tempstr, templen, 932);
        app->maximumSizeText->h = promptHeight-3;
        app->maximumSizeText->color = {1,0.9,1,1};
        
        if(app->maximumSizeText->length == 0 && !app->maximumSizeText->editing) {
            
            templen = snprintf(tempstr, sizeof(tempstr),"%dx%d",(int)app->canvas.pngMaxWidth,(int)app->canvas.pngMaxHeight);
            ui->setString(app->maximumSizeText, tempstr, templen);
        }
        
        if(app->saveLoadPath && app->saveLoadPath->length >= 4) {
            app->saveLoadPath->length -= 4;
        }
        if(app->saveLoadPath->editing) {
            ui->edit(app->saveLoadPath, true);
            app->maximumSizeText->editing = false;
        }
        if(!app->saveLoadPath) {
            ui->setString(app->saveLoadPath, ".png",4);
        } else {
            templen = snprintf(tempstr, sizeof(tempstr),"%s%s",app->saveLoadPath->string, ".png");
            ui->setString(app->saveLoadPath, tempstr, templen);
        }
        
        if(app->maximumSizeText->editing) {
            ui->edit(app->maximumSizeText, true);
        }
        
        if(app->inputModule.isKeyPressed(GLFW_KEY_TAB)){
            if(app->saveLoadPath->editing){
                app->maximumSizeText->editing=true;
                app->saveLoadPath->editing=false;
            }else{
                app->maximumSizeText->editing=false;
                app->saveLoadPath->editing=true;
            }
        }
    
        std::string path = app->saveLoadPath->string;
        
        if(app->inputModule.isKeyPressed(GLFW_KEY_ENTER)){
            app->maximumSizeText->editing=false;
            app->switchState(0);
            
            auto list = SplitString(app->maximumSizeText->string,"x");
            bool good=true;
            try {
                if(list.size()==2){
                    app->canvas.pngMaxWidth = std::stoi(list[0]);    
                    app->canvas.pngMaxHeight = std::stoi(list[1]);    
                }else if(list.size()==1){
                    app->canvas.pngMaxWidth = std::stoi(list[0]);    
                    app->canvas.pngMaxHeight = app->canvas.pngMaxWidth;
                }else{
                    good=false;   
                }
            } catch (std::exception e){
                good=false;
                log::out << log::RED << "Bad export size: "<<e.what()<<"\n";
            }
            if(good){
                bool yes = app->canvas.savePng(path,0,0,app->depictionBorders); // Todo: saving can take time. Program will freeze.
                if(yes)
                    app->notify("Exported '"+path+"'",app->successColor);
                else
                    app->notify("Could not export '"+path+"'",app->failColor);
            }else {
                app->notify(std::string(app->maximumSizeText->string)+" has bad format",app->failColor);
            }
        }
        
        if(ui->clicked(app->maximumSizeText)==1){
            app->maximumSizeText->editing=true;
            app->saveLoadPath->editing=false;   
        }
        if(ui->clicked(app->saveLoadPath)==1){
            app->saveLoadPath->editing=true;   
            app->maximumSizeText->editing=false;
        }
    }
    
    if(app->promptType==App::PROMPT_DEPICT){
        float x=app->canvas.fromCanvasX(app->depictionX),y=app->canvas.fromCanvasY(app->depictionY),xw=app->canvas.fromCanvasX(app->depictionXW),yh=app->canvas.fromCanvasY(app->depictionYH);
        UIColor lineColor = {1,0,0,1};
        ui->drawLine(x,yh,xw,yh,lineColor); // top
        ui->drawLine(xw,yh,xw,y,lineColor); // right
        ui->drawLine(x,y,xw,y,lineColor); // bottom
        ui->drawLine(x,y,x,yh,lineColor); // left
        if(app->inputModule.isKeyPressed(GLFW_KEY_ENTER)){
            app->switchState(App::PROMPT_DEPICT_SAVE);
        }
        
        float range = 20/app->canvas.zoomFactor;
        float mx = app->canvas.toCanvasX(app->inputModule.getMouseX());
        float my = app->canvas.toCanvasY(app->inputModule.getMouseY());
        if(app->inputModule.isKeyPressed(GLFW_MOUSE_BUTTON_1)){
            if(abs(mx-app->depictionXW)<range&& my>app->depictionY-range&&my<app->depictionYH+range){
                app->selectedEdgeX = 1;
                app->selectedOffsetX = app->depictionXW-mx;
            }else if(abs(mx-app->depictionX)<range&& my>app->depictionY-range&&my<app->depictionYH+range){
                app->selectedEdgeX = - 1;
                app->selectedOffsetX = app->depictionX-mx;
            }
            if(abs(my-app->depictionYH)<range&& mx>app->depictionX-range&&mx<app->depictionXW+range){
                app->selectedEdgeY = 1;
                app->selectedOffsetY = app->depictionYH-my;
            }else if(abs(my-app->depictionY)<range&& mx>app->depictionX-range&&mx<app->depictionXW+range){
                app->selectedEdgeY = - 1;
                app->selectedOffsetY = app->depictionY-my;
            }
        }
        if(app->inputModule.isKeyReleased(GLFW_MOUSE_BUTTON_1)){
            app->selectedEdgeX=0;
            app->selectedEdgeY=0;
        }
        if(app->selectedEdgeX==1){
            app->depictionXW = mx+app->selectedOffsetX;
            if(app->depictionXW<app->depictionX){
                app->selectedEdgeX=-1;
                float tmp = app->depictionXW;
                app->depictionXW = app->depictionX;
                app->depictionX = tmp;   
            }
        }else if(app->selectedEdgeX==-1){
            app->depictionX = mx+app->selectedOffsetX;
            if(app->depictionX>app->depictionXW){
                app->selectedEdgeX=1;
                float tmp = app->depictionXW;
                app->depictionXW = app->depictionX;
                app->depictionX = tmp;
            }
        }
        if(app->selectedEdgeY==1){
            app->depictionYH = app->canvas.toCanvasY(my)+app->selectedOffsetY;
            if(app->depictionYH<app->depictionY){
                app->selectedEdgeY=-1;
                float tmp = app->depictionYH;
                app->depictionYH = app->depictionY;
                app->depictionY = tmp;   
            }
        }else if(app->selectedEdgeY==-1){
            app->depictionY = app->canvas.toCanvasY(my)+app->selectedOffsetY;
            if(app->depictionY>app->depictionYH){
                app->selectedEdgeY=1;
                float tmp = app->depictionYH;
                app->depictionYH = app->depictionY;
                app->depictionY = tmp;
            }
        }
    }
    
    if(app->promptType==App::PROMPT_SAVE){
        UILayout promptLayout = ui->makeLayout();
        promptLayout.x = sw/2.f;
        promptLayout.y = sh/3.f;
        promptLayout.alignment = UILayout::ALIGN_CENTER;
        promptLayout.textSize = promptHeight;
        promptLayout.textColor = app->promptColor;
        
        templen = snprintf(tempstr, sizeof(tempstr),"Save path?");
        text = promptLayout.makeText(tempstr, templen);
        
        app->saveLoadPath = promptLayout.makeText(tempstr, templen, 930);
        app->saveLoadPath->h = promptHeight-3;
        app->saveLoadPath->color = {1,0.9,1,1};
        
        if(app->saveLoadPath && app->saveLoadPath->length >= 4) {
            app->saveLoadPath->length -= 4;
        }
        ui->edit(app->saveLoadPath, true);
        if(!app->saveLoadPath) {
            ui->setString(app->saveLoadPath, ".drw",4);
        } else {
            templen = snprintf(tempstr, sizeof(tempstr),"%s%s",app->saveLoadPath->string, ".drw");
            ui->setString(app->saveLoadPath, tempstr, templen);
        }
        
        std::string path = app->saveLoadPath->string;
        if(!app->saveLoadPath->editing) {
            app->switchState(0);
            bool yes = app->canvas.save(path); // Todo: saving can take time. Frame will drop.
            if(yes)
                app->notify("Saved '"+path+"'",app->successColor);
            else
                app->notify("Could not save '"+path+"'",app->failColor);
        }
    }
    if(app->promptType==App::PROMPT_LOAD){
        UILayout promptLayout = ui->makeLayout();
        promptLayout.x = sw/2.f;
        promptLayout.y = sh/3.f;
        promptLayout.alignment = UILayout::ALIGN_CENTER;
        promptLayout.textSize = promptHeight;
        promptLayout.textColor = app->promptColor;
        
        templen = snprintf(tempstr, sizeof(tempstr),"Load path?");
        text = promptLayout.makeText(tempstr, templen);
        
        app->saveLoadPath = promptLayout.makeText(tempstr, templen, 930);
        app->saveLoadPath->h = promptHeight-3;
        app->saveLoadPath->color = {1,0.9,1,1};
        
        if(app->saveLoadPath && app->saveLoadPath->length >= 4) {
            app->saveLoadPath->length -= 4;
        }
        ui->edit(app->saveLoadPath, true);
        if(!app->saveLoadPath) {
            ui->setString(app->saveLoadPath, ".drw",4);
        } else {
            templen = snprintf(tempstr, sizeof(tempstr),"%s%s",app->saveLoadPath->string, ".drw");
            ui->setString(app->saveLoadPath, tempstr, templen);
        }
        
        std::string path = app->saveLoadPath->string;
        if(!app->saveLoadPath->editing) {
            app->switchState(0);
            bool yes = app->canvas.load(path); // Todo: saving can take time. Frame will drop.
            if(yes)
                app->notify("Loaded '"+path+"'",app->successColor);
            else
                app->notify("Could not load '"+path+"'",app->failColor);
        }
    }
    
    // if(notifyTime>0){
    //     notification.h = 20;
    //     notification.x = sw-notification.getWidth()-5;
    //     notification.y = sh-notification.h-5;
    //     if(notifyTime<0.5)
    //         notification.rgba.a = notifyTime/0.5;
    //     notification.font = consolas;
    //     ui::Draw(notification);
    // }
    // printf("ehm %.2f\n",info.timeStep);
    // if(notifyTime==notifyDelay) // saving png can take 2 seconds. if so 
    //     notifyTime-=0.001;
    // else
    //     notifyTime-=info.timeStep;
    // }
    // if(notifyTime<0)
    //     notifyTime=0;
    
    
    // Render images

    float colorPickerHeight=app->winHeight*.3f;
    float colorPickerWidth=app->winWidth*.3f;
    if(colorPickerHeight>200)
        colorPickerHeight=200;
    if(colorPickerWidth>160)
        colorPickerWidth=160;
    if(colorPickerHeight<100)
        colorPickerHeight=100;
    if(colorPickerWidth<80)
        colorPickerWidth=80;
    int hueY = sh-colorPickerHeight;
    
    UIColor boxColor = {0.05,0.05,0.3,0.7};
    UIColor hoverBoxColor = {0.05,0.15,0.4,0.7};
    UIColor lightBoxColor = {0.15,0.3,0.5,0.7};
    
    layout = ui->makeLayout();
    layout.textSize = 25;
    layout.textColor = app->textColor;
    
    layout.x = 0;
    layout.y = sh/2.f;
    
    UIBox* box = nullptr;
    
    UIBox* brushBox = layout.makeBox(0,0);
    UIText* brushText = layout.makeText("Brush");
    box->w = ui->getWidthOfText(text) + 6;
    box->h = text->h + 6;
    
    UIBox* backBox = layout.makeBox(0,0);
    UIText* backText = layout.makeText("Background");
    box->w = ui->getWidthOfText(text) + 6;
    box->h = text->h + 6;
    
    backBox->x = 0;
    brushBox->x = 0;
    if(app->pickColorType==App::PICK_COLOR_BACKGROUND){
        backBox->y = hueY-backText->h-6;
        brushBox->y = backBox->y - backBox->h;
    }else {
        brushBox->y = hueY-brushText->h-6;
        backBox->y = brushBox->y - brushBox->h;
    }
    
    backText->x = backBox->x + 3;
    brushText->x = brushBox->x + 3;
    
    brushBox->color = boxColor;
    backBox->color = boxColor;
    if(app->pickColorType==App::PICK_COLOR_BRUSH){
        brushText->color = app->highTextColor;  
        brushBox->color = lightBoxColor;
    } else if(ui->hover(brushBox)){
        brushBox->color = hoverBoxColor;
    }
    if(app->pickColorType==App::PICK_COLOR_BACKGROUND){
        backText->color = app->highTextColor;  
        backBox->color = lightBoxColor;
    } else if(ui->hover(backBox)){
        backBox->color = hoverBoxColor;
    }
    
    if(ui->clicked(brushBox)==1){
        app->pickColorType = App::PICK_COLOR_BRUSH;
        if(app->promptType==0) // click from user was handled -> disable drawing
            app->promptType=-1;
    } else if(ui->clicked(backBox)==1){
        app->pickColorType = App::PICK_COLOR_BACKGROUND;
        if(app->promptType==0) // click from user was handled -> disable drawing
            app->promptType=-1;
    }
    
    app->basicShader->bind();
    app->basicShader->setVec2("uOffset", {app->canvas.offsetX, app->canvas.offsetY});
    app->basicShader->setVec2("uTransform", {app->canvas.zoomFactor / app->winWidth, app->canvas.zoomFactor / app->winHeight});
    app->canvas.render();
    
    app->renderHue(0,hueY,colorPickerWidth,colorPickerHeight);
    
    if(app->promptType==0){
        const UIColor markColor={1,0,0,1};
        float mx = app->inputModule.getMouseX(),my=app->inputModule.getMouseY();
        // UIRenderer* ui = info.window->getUIRenderer();
        float d = app->canvas.brushSize*app->canvas.zoomFactor/2;
        ui->drawLine(mx-d,my-d,mx+d,my-d,markColor);
        ui->drawLine(mx+d,my-d,mx+d,my+d,markColor);
        ui->drawLine(mx+d,my+d,mx-d,my+d,markColor);
        ui->drawLine(mx-d,my+d,mx-d,my-d,markColor);
    }
    if(app->inputModule.isKeyPressed(GLFW_MOUSE_BUTTON_1)){
        app->lastmx = app->inputModule.getMouseX();
        app->lastmy = app->inputModule.getMouseY();
        if(app->promptType==0){
            if(!app->doDraw){
                app->doDraw = true;
                app->drawType = 0;
                app->canvas.drawPixel(app->lastmx,app->lastmy);
            }
        }
    }else if(app->inputModule.isKeyPressed(GLFW_MOUSE_BUTTON_2)){
        app->lastmx = app->inputModule.getMouseX();
        app->lastmy = app->inputModule.getMouseY();
        if(app->promptType==0){
            if(!app->doDraw){
                app->doDraw = true;
                app->drawType = 1;
                app->canvas.brushSize *= app->extraEraseSize;
                app->canvas.erasePixel(app->lastmx,app->lastmy);
            }
        }
    }else if(app->inputModule.isKeyPressed(GLFW_MOUSE_BUTTON_3)){
        app->lastmx = app->inputModule.getMouseX();
        app->lastmy = app->inputModule.getMouseY();
        app->move = true;
    }
    if(app->inputModule.isKeyReleased(GLFW_MOUSE_BUTTON_1)){
        if(app->drawType==0){
            if(app->doDraw)
                app->canvas.submitUndo();
            app->doDraw = false;
        }   
    }
    if(app->inputModule.isKeyReleased(GLFW_MOUSE_BUTTON_2)){
        if(app->drawType==1){
            app->canvas.brushSize /= app->extraEraseSize;
            if(app->doDraw)
                app->canvas.submitUndo();
            app->doDraw = false;
        }  
    }
    if(app->inputModule.isKeyReleased(GLFW_MOUSE_BUTTON_3)){
        app->move=false;   
    }
    if(app->promptType==-1)
        app->promptType=0;
        
    // #endif
}
void App::notify(const std::string& text, engone::UIColor color){
    messages.add({});
    auto& msg = messages.last();
    msg.text = text;
    msg.color = color;
    msg.time = 2.f;
}
void App::switchState(int state){
    promptType = state;
    saveLoadPath->editing=true;
    selectedEdgeX=0;
    selectedEdgeY=0;
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
void App::renderHue(float x, float y, float w, float h){
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
    hueShader->setVec2("uWindow", { winWidth, winHeight });
    float pureHue[]{hsvColor[0],1,1};
    HSVToRGB(pureHue,rgbColor);
    hueShader->setVec3("uColor", { rgbColor[0],rgbColor[1],rgbColor[2]});
    
    hueShader->setVec2("uPos", {fadeX,fadeY});
    hueShader->setVec2("uSize", { fadeW,fadeH});
    hueShader->setInt("uShaderType", 0);
    
    quadVA.drawTriangleArray(6);
    
    hueShader->setVec2("uPos", { hueX,hueY});
    hueShader->setVec2("uSize", { hueW, hueH });
    hueShader->setInt("uShaderType", 1);
    quadVA.drawTriangleArray(6);

    hueShader->setVec2("uPos", { alphaX,alphaY});
    hueShader->setVec2("uSize", { alphaW,alphaH});
    hueShader->setInt("uShaderType", 2);
    quadVA.drawTriangleArray(6);

    if(inputModule.isKeyReleased(GLFW_MOUSE_BUTTON_1)){
        if(promptType==SELECT_FADE||promptType==SELECT_HUE||promptType==SELECT_ALPHA){
            switchState(0);   
        }
    }
    float mx = inputModule.getMouseX();
    float my = inputModule.getMouseY();
    if(inputModule.isKeyPressed(GLFW_MOUSE_BUTTON_1)){
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
        const float floatMin=0.001; // 1.0 is converted to 0.0. The markers will jump to some default position
        saturation = 1-(mx - fadeX) / (fadeW);
        value = 1-(my - fadeY) / (fadeH);
        if (saturation < floatMin)
            saturation = floatMin;
        if (saturation > 1) {
            saturation = 1;
        }
        if (value < floatMin)
            value = floatMin;
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
    
    
    UIModule* ui = &uiModule;
    // float tw = mark0.texture->getWidth();
    // float th = mark0.texture->getHeight();
    float tw = 10;
    float th = 10;
    {
        UIBox* box = ui->makeBox();
        box->x = fadeX+fadeW*(1-saturation)-tw/2;
        box->y = fadeY+fadeH*(1-value)-tw/2;
        box->w = tw+1;
        box->h = th+1;
        box->color = {1,1,1,1};
        box->textureId = colorMarker_textureId;
    }
    {
        UIBox* box = ui->makeBox();
        box->x = hueX+ hueW * hue-tw/2;
        box->y = hueY + hueH/2-th/2;
        box->w = tw+1;
        box->h = th+1;
        box->color = {1,1,1,1};
        box->textureId = colorMarker_textureId;
    }
    {
        UIBox* box = ui->makeBox();
        box->x = alphaX + alphaW * alpha - tw / 2;
        box->y = alphaY + alphaH / 2 - th / 2;
        box->w = tw+1;
        box->h = th+1;
        box->color = {1,1,1,1};
        box->textureId = colorMarker_textureId;
    }
    
    // ui::TexturedBox mark0 = {};
    // mark0.texture = &info.window->getStorage()->get<TextureAsset>("colorMarker")->texture;
    // mark0.x = fadeX+fadeW*(1-saturation)-tw/2;
    // mark0.y = fadeY+fadeH*(1-value)-tw/2;
    // mark0.w = tw+1;
    // mark0.h = th+1;
    // mark0.rgba = {1,1,1,1};
    // ui::Draw(mark0);
    
    // mark0.x = hueX+ hueW * hue-tw/2;
    // mark0.y = hueY + hueH/2-th/2;
    // ui::Draw(mark0);
    
    // mark0.x = alphaX + alphaW * alpha - tw / 2;
    // mark0.y = alphaY + alphaH / 2 - th / 2;
    // ui::Draw(mark0);
}
#ifdef gone
void App::render(engone::LoopInfo& info) {
    using namespace engone;
    
    glClearColor(canvas.backgroundColor.r,canvas.backgroundColor.g,canvas.backgroundColor.b,canvas.backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    FontAsset* consolas = info.window->getStorage()->get<FontAsset>("default");
    
    char zoomT[30];
    sprintf(zoomT,"Inverse Zoom: %.1f",1/canvas.zoomFactor);
    ui::TextBox zoomText={zoomT,0,0,20,consolas,textColor};
    zoomText.y=5;
    zoomText.x=5;
    ui::Draw(zoomText);
    
    ui::TextBox toolType={"Size: "+std::to_string((int)canvas.brushSize),0,0,20,consolas,textColor};
    toolType.y=zoomText.y+zoomText.h;
    toolType.x=zoomText.x;
    ui::Draw(toolType);
    
    ui::TextBox partText={"Pixels: "+std::to_string(canvas.particleCount)+"/"+std::to_string(canvas.maxParticles),0,0,20,consolas,textColor};
    partText.x=info.window->getWidth()/2-partText.getWidth()/2;
    partText.y=5;
    ui::Draw(partText);
    
    char mem[10];
    FormatBytes(mem,10,canvas.getUsedMemory());
    ui::TextBox memText={mem,0,0,20,consolas,textColor};
    memText.x=toolType.x;
    memText.y=toolType.y+toolType.h;
    ui::Draw(memText);
    
    ui::TextBox historyText={"History: "+std::to_string(canvas.countHistory.size()),0,0,20,consolas,textColor};
    historyText.x=memText.x;
    historyText.y=memText.y+memText.h;
    ui::Draw(historyText);
    
    float sw = info.window->getWidth();
    float sh = info.window->getHeight();
    
    if(notifyTime>0){
        notification.h = 20;
        notification.x = sw-notification.getWidth()-5;
        notification.y = sh-notification.h-5;
        if(notifyTime<0.5)
            notification.rgba.a = notifyTime/0.5;
        notification.font = consolas;
        ui::Draw(notification);
    }
    if(notifyTime==notifyDelay) // saving png can take 2 seconds. if so timeStep will be 2s and notification will disappear rigth away.
        notifyTime-=0.001;
    else
        notifyTime-=info.timeStep;
    
    if(notifyTime<0)
        notifyTime=0;
    
    
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyDown(GLFW_KEY_LEFT_SHIFT) &&app->inputModule.isKeyPressed(GLFW_KEY_S)){
        switchState(PROMPT_SAVE);
    } else if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyPressed(GLFW_KEY_S)){
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
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyPressed(GLFW_KEY_L)){
        switchState(PROMPT_LOAD);
    }
    if(app->inputModule.isKeyDown(GLFW_KEY_LEFT_CONTROL) && app->inputModule.isKeyPressed(GLFW_KEY_E)){
        switchState(PROMPT_DEPICT);
        canvas.computeBorder(depictionBorders);
    }
    
    if(app->inputModule.isKeyPressed(GLFW_KEY_ESCAPE)){
        switchState(0);
    }
    
    float promptHeight = info.window->getHeight()*0.05f;
    
    if(promptType==PROMPT_DEPICT_SAVE){
        ui::TextBox saveText = {"Export path?",0,0,promptHeight,consolas,promptColor};
        ui::TextBox sizeText = {"Maximum size (ex, 123x456 or 200)",0,0,promptHeight,consolas,promptColor};
        saveText.x = sw/2-saveText.getWidth()/2;
        saveText.y = sh/3;
        
        if(maximumSizeText.text=="" && !maximumSizeText.editing){
            maximumSizeText.text = std::to_string((int)canvas.pngMaxWidth)+"x"+std::to_string((int)canvas.pngMaxHeight);
        }
        
        if(saveLoadPath.editing){
            ui::Edit(&saveLoadPath,true);
            maximumSizeText.editing=false;
        }
        std::string oldtxt = saveLoadPath.text;
        saveLoadPath.text = saveLoadPath.text + ".png";
        if(maximumSizeText.editing){
            ui::Edit(&maximumSizeText,true);
        }
        
        if(app->inputModule.isKeyPressed(GLFW_KEY_TAB)){
            if(saveLoadPath.editing){
                maximumSizeText.editing=true;
                saveLoadPath.editing=false;
            }else{
                maximumSizeText.editing=false;
                saveLoadPath.editing=true;
            }
        }
        
        if(app->inputModule.isKeyPressed(GLFW_KEY_ENTER)){
            maximumSizeText.editing=false;
            switchState(0);
            
            auto list = SplitString(maximumSizeText.text,"x");
            bool good=true;
            try {
                if(list.size()==2){
                    canvas.pngMaxWidth = std::stoi(list[0]);    
                    canvas.pngMaxHeight = std::stoi(list[1]);    
                }else if(list.size()==1){
                    canvas.pngMaxWidth = std::stoi(list[0]);    
                    canvas.pngMaxHeight = canvas.pngMaxWidth;    
                }else{
                    good=false;   
                }
            } catch (std::exception e){
                good=false;
                log::out << log::RED << "Bad export size: "<<e.what()<<"\n";
            }
            if(good){
                bool yes = canvas.savePng(saveLoadPath.text,0,0,depictionBorders); // Todo: saving can take time. Program will freeze.
                if(yes)
                    notify("Exported '"+saveLoadPath.text+"'",successColor);
                else
                    notify("Could not export '"+saveLoadPath.text+"'",failColor);
            }else {
                notify(maximumSizeText.text+" has bad format",failColor);
            }
        }
        
        saveLoadPath.h = saveText.h-3;
        saveLoadPath.x = sw/2-saveLoadPath.getWidth()/2;
        saveLoadPath.y = saveText.y+saveLoadPath.h;
        saveLoadPath.font = consolas;
        saveLoadPath.rgba = {1,0.9,1,1};
        
        sizeText.x = sw/2-sizeText.getWidth()/2;
        sizeText.y = saveLoadPath.y +saveLoadPath.h;
        
        maximumSizeText.h = saveLoadPath.h;
        maximumSizeText.x = sw/2-maximumSizeText.getWidth()/2;
        maximumSizeText.y = sizeText.y +sizeText.h;
        maximumSizeText.font = consolas;
        maximumSizeText.rgba = {1,0.9,1,1};
        
        if(ui::Clicked(maximumSizeText)==1){
            maximumSizeText.editing=true;
            saveLoadPath.editing=false;   
        }
        if(ui::Clicked(saveLoadPath)==1){
            saveLoadPath.editing=true;   
            maximumSizeText.editing=false;
        }
        
        ui::Draw(saveLoadPath);
        ui::Draw(maximumSizeText);
        ui::Draw(sizeText);
        ui::Draw(saveText);
        saveLoadPath.text = oldtxt;
    }
    
    if(promptType==PROMPT_DEPICT){
        float x=canvas.fromCanvasX(depictionX),y=canvas.fromCanvasY(depictionY),xw=canvas.fromCanvasX(depictionXW),yh=canvas.fromCanvasY(depictionYH);
        ui::Color lineColor = {1,0,0,1};
        info.window->getUIRenderer()->drawLine(x,yh,xw,yh,lineColor); // top
        info.window->getUIRenderer()->drawLine(xw,yh,xw,y,lineColor); // right
        info.window->getUIRenderer()->drawLine(x,y,xw,y,lineColor); // bottom
        info.window->getUIRenderer()->drawLine(x,y,x,yh,lineColor); // left
        if(app->inputModule.isKeyPressed(GLFW_KEY_ENTER)){
            switchState(PROMPT_DEPICT_SAVE);
        }
        
        float range = 20/canvas.zoomFactor;
        float mx = canvas.toCanvasX(info.window->getMouseX());
        float my = canvas.toCanvasY(info.window->getMouseY());
        if(app->inputModule.isKeyPressed(GLFW_MOUSE_BUTTON_1)){
            if(abs(mx-depictionXW)<range&& my>depictionY-range&&my<depictionYH+range){
                selectedEdgeX = 1;
                selectedOffsetX = depictionXW-mx;
            }else if(abs(mx-depictionX)<range&& my>depictionY-range&&my<depictionYH+range){
                selectedEdgeX = - 1;
                selectedOffsetX = depictionX-mx;
            }
            if(abs(my-depictionYH)<range&& mx>depictionX-range&&mx<depictionXW+range){
                selectedEdgeY = 1;
                selectedOffsetY = depictionYH-my;
            }else if(abs(my-depictionY)<range&& mx>depictionX-range&&mx<depictionXW+range){
                selectedEdgeY = - 1;
                selectedOffsetY = depictionY-my;
            }
        }
        if(app->inputModule.isKeyReleased(GLFW_MOUSE_BUTTON_1)){
            selectedEdgeX=0;
            selectedEdgeY=0;
        }
        if(selectedEdgeX==1){
            depictionXW = mx+selectedOffsetX;
            if(depictionXW<depictionX){
                selectedEdgeX=-1;
                float tmp = depictionXW;
                depictionXW = depictionX;
                depictionX = tmp;   
            }
        }else if(selectedEdgeX==-1){
            depictionX = mx+selectedOffsetX;
            if(depictionX>depictionXW){
                selectedEdgeX=1;
                float tmp = depictionXW;
                depictionXW = depictionX;
                depictionX = tmp;
            }
        }
        if(selectedEdgeY==1){
            depictionYH = canvas.toCanvasY(info.window->getMouseY())+selectedOffsetY;
            if(depictionYH<depictionY){
                selectedEdgeY=-1;
                float tmp = depictionYH;
                depictionYH = depictionY;
                depictionY = tmp;   
            }
        }else if(selectedEdgeY==-1){
            depictionY = canvas.toCanvasY(info.window->getMouseY())+selectedOffsetY;
            if(depictionY>depictionYH){
                selectedEdgeY=1;
                float tmp = depictionYH;
                depictionYH = depictionY;
                depictionY = tmp;
            }
        }
    }
    
    if(promptType==PROMPT_SAVE){
        ui::TextBox saveText = {"Save path?",0,0,promptHeight,consolas,promptColor};
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
        ui::TextBox loadText = {"Load path?",0,0,promptHeight,consolas,promptColor};
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
    
    // if(notifyTime>0){
    //     notification.h = 20;
    //     notification.x = sw-notification.getWidth()-5;
    //     notification.y = sh-notification.h-5;
    //     if(notifyTime<0.5)
    //         notification.rgba.a = notifyTime/0.5;
    //     notification.font = consolas;
    //     ui::Draw(notification);
    // }
    // printf("ehm %.2f\n",info.timeStep);
    // if(notifyTime==notifyDelay) // saving png can take 2 seconds. if so 
    //     notifyTime-=0.001;
    // else
    //     notifyTime-=info.timeStep;
    // }
    // if(notifyTime<0)
    //     notifyTime=0;
    
    
    // Render images
    ShaderAsset* shader = info.window->getStorage()->get<ShaderAsset>("basic");
    shader->bind();
    shader->setVec2("uOffset", {canvas.offsetX, canvas.offsetY});
    shader->setVec2("uTransform", {canvas.zoomFactor / info.window->getWidth(), canvas.zoomFactor /info.window->getHeight()});

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
    
    ui::Color boxColor = {0.05,0.05,0.3,0.7};
    ui::Color hoverBoxColor = {0.05,0.15,0.4,0.7};
    ui::Color lightBoxColor = {0.15,0.3,0.5,0.7};
    
    ui::Box brushBox = {0,0,0,0};
    ui::TextBox brushText={"Brush ",0,0,25,consolas,textColor};
    
    ui::Box backBox = {};
    ui::TextBox backText={"Background",0,0,25,consolas,textColor};
    
    brushBox.w = brushText.getWidth()+6;
    brushBox.h = brushText.getHeight()+6;
    backBox.w = backText.getWidth()+6;
    backBox.h = backText.getHeight()+6;
    
    backBox.x = 0;
    brushBox.x = 0;
    if(pickColorType==PICK_COLOR_BACKGROUND){
        backBox.y = hueY-backText.h-6;
        brushBox.y = backBox.y - backBox.h;
    }else {
        brushBox.y = hueY-brushText.h-6;
        backBox.y = brushBox.y - brushBox.h;
    }
    
    backText.x = backBox.x + 3;
    backText.y = backBox.y + 3;
    brushText.x = brushBox.x+3;
    brushText.y = brushBox.y+3;
    
    brushBox.rgba = boxColor;
    backBox.rgba = boxColor;
    if(pickColorType==PICK_COLOR_BRUSH){
        brushText.rgba = highTextColor;  
        brushBox.rgba = lightBoxColor;
    } else if(ui::Hover(brushBox)){
        brushBox.rgba = hoverBoxColor;
    }
    if(pickColorType==PICK_COLOR_BACKGROUND){
        backText.rgba = highTextColor;  
        backBox.rgba = lightBoxColor;
    } else if(ui::Hover(backBox)){
        backBox.rgba = hoverBoxColor;
    }
    
    ui::Draw(brushBox);
    ui::Draw(brushText);
    ui::Draw(backBox);
    ui::Draw(backText);
    
    if(ui::Clicked(brushBox)==1){
        pickColorType = PICK_COLOR_BRUSH;
        if(promptType==0) // click from user was handled -> disable drawing
            promptType=-1;
    } else if(ui::Clicked(backBox)==1){
        pickColorType = PICK_COLOR_BACKGROUND;
        if(promptType==0) // click from user was handled -> disable drawing
            promptType=-1;
    }
    
    canvas.render(info);
    renderHue(info,0,hueY,colorPickerWidth,colorPickerHeight);
    
    if(promptType==0){
        const ui::Color markColor={1,0,0,1};
        float mx = info.window->getMouseX(),my=info.window->getMouseY();
        UIRenderer* ui = info.window->getUIRenderer();
        float d = canvas.brushSize*canvas.zoomFactor/2;
        ui->drawLine(mx-d,my-d,mx+d,my-d,markColor);
        ui->drawLine(mx+d,my-d,mx+d,my+d,markColor);
        ui->drawLine(mx+d,my+d,mx-d,my+d,markColor);
        ui->drawLine(mx-d,my+d,mx-d,my-d,markColor);
    }
    if(app->inputModule.isKeyPressed(GLFW_MOUSE_BUTTON_1)){
        lastmx = info.window->getMouseX();
        lastmy = info.window->getMouseY();
        if(promptType==0){
            if(!doDraw){
                doDraw = true;
                drawType = 0;
                canvas.drawPixel(lastmx,lastmy);
            }
        }
    }else if(app->inputModule.isKeyPressed(GLFW_MOUSE_BUTTON_2)){
        lastmx = info.window->getMouseX();
        lastmy = info.window->getMouseY();
        if(promptType==0){
            if(!doDraw){
                doDraw = true;
                drawType = 1;
                canvas.brushSize *= extraEraseSize;
                canvas.erasePixel(lastmx,lastmy);
            }
        }
    }else if(app->inputModule.isKeyPressed(GLFW_MOUSE_BUTTON_3)){
        lastmx = info.window->getMouseX();
        lastmy = info.window->getMouseY();
        move = true;
    }
    if(app->inputModule.isKeyReleased(GLFW_MOUSE_BUTTON_1)){
        if(drawType==0){
            if(doDraw)
                canvas.submitUndo();
            doDraw = false;
        }   
    }
    if(app->inputModule.isKeyReleased(GLFW_MOUSE_BUTTON_2)){
        if(drawType==1){
            canvas.brushSize /= extraEraseSize;
            if(doDraw)
                canvas.submitUndo();
            doDraw = false;
        }  
    }
    if(app->inputModule.isKeyReleased(GLFW_MOUSE_BUTTON_3)){
        move=false;   
    }
    if(promptType==-1)
        promptType=0;
}
#endif