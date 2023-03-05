#include "DrwStd/Canvas.h"

#include "Engone/PlatformLayer.h"

static const char* particleGLSL = {
#include "DrwStd/Shaders/particle.glsl" 
};

#include "Engone/vendor/stb_image/stb_image_write.h"

void Canvas::init(engone::Window* window){
    mainWindow = window;
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
// void Canvas::drawBrush(float mx, float my){
//     int SIZE = (int)brushSize;
//     float off = 0.5;
//     if (SIZE/2.f- SIZE/2 != 0) {
//         off = 0;
//     }
//     brushSize = 1;
//     int count=0;
//     for (int i = 0; i < SIZE;i++) {
//         for (int j = 0; j < SIZE; j++) {
//             float tx = i-SIZE/2;
//             float ty = j-SIZE/2;
//             if ((SIZE / 2.f) * (SIZE / 2.f) >= pow(tx+off, 2) + pow(ty+off, 2)) {
//                 // float x = (mx - mainWindow->getWidth() / 2) / zoomFactor - offsetX;
//                 // float y = (mainWindow->getHeight() / 2 - my) / zoomFactor - offsetY;
//                 float x = mx;
//                 float y = my;
    
//                 drawRawPixel((int)(x+off) + (int)tx, (int)(y+off) + (int)ty);
//                 count++;
//             }
//         }
// 	}
//     needsRefresh=true;
//     countHistory.push_back(count);
//     brushSize = SIZE;
// }
void Canvas::drawPixel(float mx, float my){
    // drawBrush(mx,my);
    // return;
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
    float tx = (mx - mainWindow->getWidth() / 2) / zoomFactor - offsetX;
    float ty = (mainWindow->getHeight() / 2 - my) / zoomFactor - offsetY;
    
    particles[particleCount].x=tx;
    particles[particleCount].y=ty;
    particles[particleCount].color=brushColor;
    particles[particleCount].s=brushSize;
    particleCount++;
    needsRefresh=true;
}
void Canvas::computeBorder(float outBorders[4]){
    float t=-INFINITY,r=-INFINITY,b=INFINITY,l=INFINITY;
    for(int i=0;i<particleCount;i++){
        float x = particles[i].x,y=particles[i].y;
        float s = particles[i].s/2;
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
uint64 Canvas::getUsedMemory(){
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
    return (x - mainWindow->getWidth() / 2) / zoomFactor - offsetX;
}
float Canvas::toCanvasY(float y){
    return (mainWindow->getHeight() / 2 - y) / zoomFactor - offsetY;
}
float Canvas::fromCanvasX(float x){
    return (x+offsetX)*zoomFactor + mainWindow->getWidth()/2;
}
float Canvas::fromCanvasY(float y){
    return -((y+offsetY)*zoomFactor - mainWindow->getHeight()/2);
    // return (mainWindow->getHeight() / 2 - y) / zoomFactor - offsetY;
}
void Canvas::drawFromTo(float fromX, float fromY, float toX, float toY){
    needsRefresh=true;
    
    float factor = brushSize/3;
    if(factor<1) factor=1;
    
    fromX = toCanvasX(fromX);
    fromY = toCanvasY(fromY);
    toX = toCanvasX(toX);
    toY = toCanvasY(toY);
    float dx = (toX-fromX)/factor;
    float dy = (toY-fromY)/factor;
    /*if (dx == 0) {
        if (abs(dy) < 2) {
            Pencil(layer, toX, toY);
            return;
        }
    }
    else if (dy == 0) {
        if (abs(dx) < 2) {
            Pencil(layer, toX, toY);
            return;
        }
    }*/
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
bool Canvas::save(const std::string& path){
    using namespace engone;
    auto file = FileOpen(path,0,FILE_WILL_CREATE);
    if(file){
        bool yes = FileWrite(file,particles,particleCount*sizeof(Particle));
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
    
    // Todo: auto tune width and height to some appropriate values
    // pixelWidth = 100;
    // pixelHeight = 100;
    
    
    float maxWidth = 2000;
    float maxHeight = 2000;
    
    pixelWidth = w;
    pixelHeight = h;
    if(pixelWidth>maxWidth){    
        pixelWidth = maxWidth;
        pixelHeight = pixelWidth*h/w;
    }
    if(pixelHeight>maxHeight){
        pixelHeight = maxHeight;
        pixelWidth = pixelHeight*w/h;   
    }
    
    int pixelCount = pixelWidth*pixelHeight;
    uint8* rawdata = (uint8*)Allocate(4*pixelCount);
    if(!rawdata) return false;
    
    Color background = {0,0.5,1,1};
    Pixel pback = ColorToPixel(background);
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
    
    int res = stbi_write_png(path.c_str(),pixelWidth,pixelHeight,4,rawdata,pixelWidth*4); // comp = 4 (RGBA), stride_in_bytes = row of pixels
    
    Free(rawdata,4*pixelCount);
        
    return res;
}
bool Canvas::load(const std::string& path){
    using namespace engone;
    uint64 fileSize;
    auto file = FileOpen(path,&fileSize,FILE_ONLY_READ);
    
    uint64 newParticles = fileSize/sizeof(Particle) + 10000;// plus some extra to delay the next reallocation
    if(file){
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
        particleCount = fileSize/sizeof(Particle);
        countHistory.clear();
        bool yes = FileRead(file,particles,fileSize);
        FileClose(file);
        return true;
    }else{
        return false;   
    }
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
void Canvas::undo(){
    if(historyIndex==-1) return;
    
    particleCount = countHistory[historyIndex];
    historyIndex--;
    
    // int num = countHistory.back();
    // particleCount = num;
    // countHistory.erase(countHistory.begin()+countHistory.size()-1);   
}
void Canvas::redo(){
    if(historyIndex==countHistory.size()-1) return;
    
    historyIndex++;
    particleCount = countHistory[historyIndex];
}
void Canvas::submitUndo(){
    // do nothing if particle count is the same as last time
    if(!countHistory.empty())
        if(particleCount==countHistory.back())
            return;
    
    countHistory.push_back(particleCount);
    historyIndex++;
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
    engone::log::out << "Popped: "<<popped<<"\n";
}
void Canvas::render(engone::LoopInfo& info){
    using namespace engone;
    if(!shader) return;
    
    shader->bind();
    EnableBlend();
    // EnableDepth();
    
    glEnable(GL_PROGRAM_POINT_SIZE);
    
    shader->setFloat("uZoom",zoomFactor);
    shader->setVec2("uOffset",{offsetX,offsetY});
    shader->setVec2("uTransform",{zoomFactor / (info.window->getWidth()), zoomFactor / (info.window->getHeight())});
    
    CommonRenderer* renderer = info.window->getCommonRenderer();
    renderer->updateOrthogonal(shader);
    
    // Todo: have multiple particle arrays with a color for each instead of each individual particle having it's own color.
    //      This should be an option though.
    
    // Todo: only set data if it has changed.
    if(needsRefresh)
        shaderBuffer.setData(particleCount*sizeof(Particle),particles);
    needsRefresh = false;
    
    if(shaderBuffer.initialized())
        shaderBuffer.drawPoints(particleCount,0);
}