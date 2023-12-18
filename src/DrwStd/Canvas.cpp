#include "DrwStd/Canvas.h"

#include "Engone/PlatformLayer.h"

static const char* particleGLSL = {
#include "DrwStd/Shaders/particle.glsl" 
};
static const char* markerGLSL = {
#include "DrwStd/Shaders/marker.glsl" 
};

#include "stb/stb_image_write.h"
#include "stb/stb_image.h"

#include "DrwStd/App.h"

void Canvas::init(App* app){
    mainWindow = app;
    // if(!particles){
    //     maxParticles = 100000;
    //     particles = (Particle*) engone::Allocate(maxParticles*sizeof(Particle));
    //     if(!particles)
    //         maxParticles = 0;
    //     particleCount=0;
    // }
    if(!shader){
        shader = new engone::Shader(particleGLSL);
    }
    countHistory.push_back(0);
    
    if(!brushShader)
        brushShader = new engone::Shader(markerGLSL);
    
    // for(int i=0;i<1000;i++){
    //     float x = i%100;
    //     float y = i/100;
    //     drawPixel(x,y,{1.f,1.f,1.f,1.f});
    // }
}
void Canvas::cleanup(){
    if(particles){
        engone::Free(particles,maxParticles*sizeof(Particle));
        particles = 0;
        maxParticles = 0;
        particleCount = 0;
    }
    // Todo: Clean more things, shaderBuffer, history 
}
Pixel ColorToPixel(Color color){
    return {(char)(color.r*255),(char)(color.g*255),(char)(color.b*255),(char)(color.a*255)};   
}
void Canvas::drawPixel(float mx, float my){
    if(particleCount==maxParticles){
        int newSize = maxParticles*2;
        if(newSize==0)
            newSize = 10000;
        Particle* data = (Particle*)engone::Reallocate(particles,maxParticles*sizeof(Particle),newSize*sizeof(Particle));
        if(!data) return; // failed
        maxParticles = newSize;
        particles = data;
    }
    // Todo: replace with toCanvasX/Y
    float tx = (mx - mainWindow->winWidth / 2) / zoomFactor - offsetX;
    float ty = (mainWindow->winHeight / 2 - my) / zoomFactor - offsetY;
    
    particles[particleCount].x=tx;
    particles[particleCount].y=ty;
    particles[particleCount].color=brushColor;
    particles[particleCount].s=brushSize;
    particleCount++;
    needsRefresh=true;
}
void Canvas::erasePixel(float mx , float my){
    brushSize = -brushSize;
    drawPixel(mx,my);
    brushSize = -brushSize;
}
void Canvas::computeBorder(float outBorders[4]){
    float t=-INFINITY,r=-INFINITY,b=INFINITY,l=INFINITY;
    for(int i=0;i<particleCount;i++){
        float x = particles[i].x,y=particles[i].y;
        float s = particles[i].s/2;
        if(s<0) continue; // erase particles are not counted
        if(x+s>r) r=x+s;
        if(x-s<l) l=x-s;
        if(y+s>t) t=y+s;
        if(y-s<b) b=y-s;
    }
    outBorders[0] = t;
    outBorders[1] = r;
    outBorders[2] = b;
    outBorders[3] = l;
}
u64 Canvas::getUsedMemory(){
    return sizeof(Particle)*maxParticles;
}
void Canvas::drawRawPixel(float x, float y){
    if(particleCount==maxParticles){
        int newSize = maxParticles*2;
        if(newSize==0)
            newSize = 10000;
        Particle* data = (Particle*)engone::Reallocate(particles,maxParticles*sizeof(Particle),newSize*sizeof(Particle));
        if(!data) return; // failed
        maxParticles = newSize;
        particles = data;
    }
    particles[particleCount].x=x;
    particles[particleCount].y=y;
    particles[particleCount].color=brushColor;
    particles[particleCount].s=brushSize;
    particleCount++;
}
float Canvas::toCanvasX(float x){
    return (x - mainWindow->winWidth / 2) / zoomFactor - offsetX;
}
float Canvas::toCanvasY(float y){
    return (mainWindow->winHeight / 2 - y) / zoomFactor - offsetY;
}
float Canvas::fromCanvasX(float x){
    return (x+offsetX)*zoomFactor + mainWindow->winWidth/2;
}
float Canvas::fromCanvasY(float y){
    return -((y+offsetY)*zoomFactor - mainWindow->winHeight/2);
    // return (mainWindow->getHeight() / 2 - y) / zoomFactor - offsetY;
}
void Canvas::drawFromTo(float fromX, float fromY, float toX, float toY){
    needsRefresh=true;
    
    float factor = abs(brushSize)/3;
    if(factor<1) factor=1;
    
    fromX = toCanvasX(fromX);
    fromY = toCanvasY(fromY);
    toX = toCanvasX(toX);
    toY = toCanvasY(toY);
    float dx = (toX-fromX)/factor;
    float dy = (toY-fromY)/factor;
    if (abs(dx)>abs(dy)) {
        float k = dy / dx;
        if (dx>0) {
            for (int i = 0; i <= dx; i++) {
                // drawBrush(fromX+i*factor, fromY+i*k*factor);
                drawRawPixel(fromX+i*factor, fromY+i*k*factor);
            }
        }  else {
            for (int i = 0; i <= abs(dx); i++) {
                // drawBrush(fromX-i*factor, fromY - i * k*factor);
                drawRawPixel(fromX-i*factor, fromY - i * k*factor);
            }
        }
    } else {
        float k = dx / dy;
        if (dy > 0) {
            for (int i = 0; i <= dy; i++) {
                // drawBrush(fromX+i*k*factor, fromY+i*factor);
                drawRawPixel(fromX+i*k*factor, fromY+i*factor);
            }
        } else {
            for (int i = 0; i <= abs(dy); i++) {
                // drawBrush(fromX - i * k*factor, fromY-i*factor);
                drawRawPixel(fromX - i * k*factor, fromY-i*factor);
            }
        }
    }
}
void Canvas::eraseFromTo(float fromX, float fromY, float toX, float toY){
    brushSize = -brushSize;
    drawFromTo(fromX,fromY,toX,toY);
    brushSize = -brushSize;
}
bool Canvas::save(const std::string& path){
    using namespace engone;
    auto file = FileOpen(path,0,engone::FILE_ALWAYS_CREATE);
    if(file){
        bool yes = FileWrite(file,&backgroundColor,sizeof(backgroundColor));
        yes = FileWrite(file,particles,particleCount*sizeof(Particle));
        FileClose(file);
        return true;
    }else{
        return false;   
    }
}
bool Canvas::savePng(const std::string& path, int pixelWidth, int pixelHeight, float borders[4]){
    using namespace engone;
    
    float bx = borders[3];
    float by = borders[0];
    float w = borders[1]-borders[3];
    float h = borders[0]-borders[2];
    
    pixelWidth = w;
    pixelHeight = h;
    if(pixelWidth>pngMaxWidth){    
        pixelWidth = pngMaxWidth;
        pixelHeight = pixelWidth*h/w;
    }
    if(pixelHeight>pngMaxHeight){
        pixelHeight = pngMaxHeight;
        pixelWidth = pixelHeight*w/h;   
    }
    int pixelCount = pixelWidth*pixelHeight;
    
    u8* rawdata = (u8*)Allocate(4*pixelCount);
    if(!rawdata) return false;
    
    // Todo: Create frame buffer and image once (possibly resizing) instead of recreating them each call?
    //      Probably doesn't affects performance to much.
    
    // ####   GPU
    
     if(!shader) return false;
    
    unsigned int fbo;
    glGenFramebuffers(1,&fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,fbo);
    
    unsigned int texture;
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixelWidth, pixelHeight,0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        printf("Buffer not complete\n");  
    }
    
    glViewport(0,0,pixelWidth, pixelHeight);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glEnable(GL_PROGRAM_POINT_SIZE);
   
    glClearColor(backgroundColor.r,backgroundColor.g,backgroundColor.b,backgroundColor.a);
    glClear(GL_COLOR_BUFFER_BIT);
 
    float zoom2 = pixelWidth/w; // or pixelHeight/h
 
    shader->bind();
    shader->setFloat("uZoom",zoom2);
    shader->setVec2("uOffset",{(-bx-w/2),(-by+h/2)});
    shader->setVec2("uWindow",{w*zoom2,h*zoom2});
    shader->setVec4("uBackgroundColor",*(glm::vec4*)&backgroundColor);
   
    if(needsRefresh)
        shaderBuffer.setData(particleCount*sizeof(Particle),particles);
    needsRefresh = false;
    
    if(shaderBuffer.initialized())
        shaderBuffer.drawPoints(particleCount,0);
    
    glReadPixels(0,0,pixelWidth,pixelHeight,GL_RGBA,GL_UNSIGNED_BYTE,rawdata);
    GL_CHECK();
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glDeleteFramebuffers(1,&fbo);
    
    glViewport(0,0,mainWindow->winWidth, mainWindow->winHeight);
    
    // #####   CPU
    #ifdef CPU_PROCESSING
    Pixel pback = ColorToPixel(backgroundColor);
    for(int i=0;i<pixelCount;i++){
        *(rawdata+i*4+0) = pback.r;
        *(rawdata+i*4+1) = pback.g;
        *(rawdata+i*4+2) = pback.b;
        *(rawdata+i*4+3) = pback.a;
    }
    // Todo: multithreading? won't they overlap each other with big sizes? altough with only one color that is fine.
    //      Whether the image is 100% accurate probably doesn't matter.
    // Todo: Another option for optimization is rendering to a framebuffer and utilizing the GPU by doing so.
    
    for(int i=0;i<particleCount;i++){
        Particle& particle = particles[i];
        
        int s = particle.s;
        Color col = particle.color;
        if(s<0){
            col = backgroundColor;
            s = -s;
        }
        if(i%2000==0){
            // Print based on time instead of i?
            printf("Progress %.1f\n",100.f*i/particleCount);
        }
        for(int j=0;j<s*s;j++){
            int x = (int)(pixelWidth*(particle.x-bx+j % s - s/2)/w);
            int y = (int)(pixelHeight*(by-particle.y+ j / s - s/2)/h);
            
            if(x<0||y<0||x>=pixelWidth||y>=pixelHeight) continue;
            int index = x+y*pixelWidth;
            
            *(rawdata+index*4+0) = particle.color.r*255;
            *(rawdata+index*4+1) = particle.color.g*255;
            *(rawdata+index*4+2) = particle.color.b*255;
            *(rawdata+index*4+3) = particle.color.a*255;
        }
    }
    #endif
    
    stbi_flip_vertically_on_write(true);
    int res = stbi_write_png(path.c_str(),pixelWidth,pixelHeight,4,rawdata,pixelWidth*4); // comp = 4 (RGBA), stride_in_bytes = row of pixels
    
    Free(rawdata,4*pixelCount);
        
    return res;
}
bool Canvas::load(const std::string& path){
    using namespace engone;
    u64 fileSize;
    auto file = FileOpen(path,&fileSize,engone::FILE_ONLY_READ);
    
    if(file){
        particleCount = (fileSize-sizeof(backgroundColor))/sizeof(Particle);
        u64 newParticles = particleCount + 10000;// plus some extra to delay the next reallocation
        if(maxParticles<newParticles){
            Particle* data = (Particle*)engone::Reallocate(particles,maxParticles*sizeof(Particle),newParticles*sizeof(Particle));
            if(!data){
                FileClose(file);
                return false;
            }
            particles = data;
            maxParticles = newParticles;
        }
        needsRefresh=true;
        countHistory.clear();
        countHistory.push_back(particleCount);
        bool yes = FileRead(file,&backgroundColor,sizeof(backgroundColor));
        yes = FileRead(file,particles,fileSize-sizeof(backgroundColor));
        FileClose(file);
        return true;
    }else{
        return false;   
    }
}
bool Canvas::loadPng(const std::string& path) {   
    using namespace engone;
    struct RGBA {
        u8 r,g,b,a;
    };
    
    int w,h,channels;
    stbi_set_flip_vertically_on_load(true);
    u8* data = stbi_load(path.c_str(), &w, &h, &channels, 4);
    stbi_set_flip_vertically_on_load(false);
    if(!data)
        return false;
    
    // background color = most common color
    int mostCommon_color = 0;
    int mostCommon_freq = 0;
    std::unordered_map<int,int> colorFrequency;
    for(int i=0;i<w*h;i++) {
        int color = *((int*)data + i);
        auto& pair = colorFrequency.find(color);
        if(pair != colorFrequency.end()) {
            pair->second++;
        } else {
            colorFrequency[color] = 1;
        }
    }
    for(auto& pair : colorFrequency) {
        if(mostCommon_freq < pair.second) {
            mostCommon_color = pair.first;
            mostCommon_freq = pair.second;   
        }
    }
    
    RGBA* color = (RGBA*)&mostCommon_color;
    backgroundColor = {color->r / 255.f, color->g / 255.f, color->b / 255.f, color->a / 255.f};
    
    // get particles from the data
    u64 newParticles = w*h+100;// plus some extra to delay the next reallocation
    if(maxParticles<newParticles){
        Particle* data = (Particle*)engone::Reallocate(particles,maxParticles*sizeof(Particle),newParticles*sizeof(Particle));
        if(!data){
            stbi_image_free(data);
            return false;
        }
        particles = data;
        maxParticles = newParticles;
    }
    
    // TODO: Each pixel (except for backround color) generates a particle. We can improve this by combining close particles into a larger one.
    //  How would the algorithm work?
    particleCount = 0;
    for(int i=0;i<w*h;i++) {
        int color_int = *((int*)data + i);
        if(color_int == mostCommon_color)
            continue;
        RGBA* color = ((RGBA*)data + i);
        int x = i % w, y = i / w;
        
        Particle* p = &particles[particleCount++];
        p->color = {color->r / 255.f, color->g / 255.f, color->b / 255.f, color->a / 255.f};
        p->s = 1.04f; // .04 because of random gaps between particles
        p->x = x;
        p->y = y;
    }
    
    needsRefresh = true;
    countHistory.clear();
    countHistory.push_back(particleCount);
    stbi_image_free(data);
    return true;
}
void Canvas::setBrushSize(float size){
    brushSize = size;   
}
void Canvas::setBrushColor(Color color){
    brushColor = color;   
}
void Canvas::setBackgroundColor(Color color){
    backgroundColor = color;
}
bool Canvas::undo(){
    if(historyIndex==0) return false;
    
    historyIndex--;
    particleCount = countHistory[historyIndex];
    // engone::log::out << "UNDO index "<<historyIndex << " "<<particleCount<<"\n";
    return true;
}
bool Canvas::redo(){
    if(historyIndex==countHistory.size()-1) return false;
    
    historyIndex++;
    particleCount = countHistory[historyIndex];
    // engone::log::out << "REDO index "<<historyIndex << " "<<particleCount<<"\n";
    return true;
}
bool Canvas::submitUndo(){
    // do nothing if particle count is the same as last time
    if(particleCount==countHistory.back())
        return false;
    
    historyIndex++;
    if(countHistory.size()==historyIndex)
        countHistory.push_back(particleCount);
    else{
        countHistory.resize(historyIndex+1);
        countHistory[historyIndex] = particleCount;
    }
    // engone::log::out << "SUBMIT index "<<historyIndex << " "<<particleCount<<"\n";
    return true;
}
void Canvas::clear(){
    particleCount = 0;
    historyIndex=0;
    countHistory.resize(1);
    countHistory[0]=particleCount;
}
void Canvas::pruneAlgorithm(){
    int popped=0;
    markedParticles.resize(particleCount,MARK_AVAILABLE);
    
    for(int i=0;i<particleCount;i++){
        for(int j=i+1;j<particleCount;j++){
            Particle* a = particles+i;
            Particle* b = particles+j;
            
            float dx = (a->x-b->x);
            float dy = (a->y-b->y);
            // float sqrDistance = sqrt(dx*dx+dy*dy);
            float distance = sqrt(dx*dx+dy*dy);
            
            // if(a->s*a->s/4>sqrDistance+b->s*b->s/4){
            // if(a->s/2>distance+b->s/2){
                // a is covering b
                // *b = *(particles+particleCount-1);
                // particleCount--;
                // j--;
                // poppings.push_back();
                // popped++;
                // i is safe, no decrement
            // } else if(b->s*b->s/4>sqrDistance+a->s*a->s/4){
            // }
            // else
            if(b->s/2>distance+a->s/2){
                markedParticles[i]=MARK_DEL;
                a->color = {1,0,0,1};
                popped++;
                break;
                // b is covering a
                // *a = *(particles+particleCount-1);
                // particleCount--;
                // i--;
                // j=i+1; // reset j
            }
        }
    }
    int index=0;
    // while(index<particleCount){
    //     if(markedParticles[index]==MARK_DEL){
    //         // find non marked, and use it.
    //         int found = -1;
    //         for(int i=index+1;i<particleCount;i++){
    //             if(markedParticles[i]==MARK_AVAILABLE){
    //                 found = i; 
    //             }
    //         }
    //         if(found!=-1){
    //             markedParticles[found]=MARK_EMPTY;
    //             particles[index] = particles[found];
    //         }
    //     }
    //     index++;
    // }
    // particleCount-=popped;
    if(popped!=0){
        // countHistory.clear();
    }
    needsRefresh=true;
    // engone::log::out << "Popped: "<<popped<<"\n";
}
void Canvas::render(){
    using namespace engone;
    if(!shader) return;
    
    shader->bind();
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    // EnableBlend();
    // EnableDepth();
    
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    shader->setFloat("uZoom",zoomFactor);
    shader->setVec2("uOffset",{offsetX,offsetY});
    shader->setVec2("uWindow",{ mainWindow->winWidth, mainWindow->winHeight });
    shader->setVec4("uBackgroundColor",*(glm::vec4*)&backgroundColor);
    
    
    if(needsRefresh)
        shaderBuffer.setData(particleCount*sizeof(Particle),particles);
    needsRefresh = false;
    
    if(shaderBuffer.initialized())
        shaderBuffer.drawPoints(particleCount,0);
        
    // float sw = mainWindow->winWidth,sh=mainWindow->winHeight ;
    // float mx = mainWindow->inputModule.getMouseX(), my=mainWindow->inputModule.getMouseY();
    // brushShader->bind();
    // brushShader->setVec4("uColor",{1,0,0,1});
    // brushShader->setVec2("uPos",{info.window->getMouseX(),info.window->getMouseY()});
    // brushShader->setFloat("uRadius",{brushSize});
    // brushShader->setFloat("uThickness",{1});
    // brushShader->setVec2("uWindow",{sw,sh});
    
    // renderer->drawQuad(info);
    
}