#include "Studio.h"

#include <vector>
#include <unordered_map>

#include "Engone/Handlers/AssetHandler.h"

#include "DrawingStudio/Objects/Image.h"

#include "Engone/Rendering/Texture.h"

#include "Tools.h"

namespace studio
{
	static const std::string basic = {
#include "../Shaders/basic.glsl"
	};
	static const std::string coloring = {
#include "../Shaders/color.glsl"
	};

	// images in the studio
	static std::vector<Image*> images;
	static Image* selectedImage;
	static int winW = 500, winH = 500;

	static bool doDraw, move;
	static int lastmx, lastmy;

	static float offsetX = 0, offsetY = 0;
	static float zoom = 12;

	void InitAssets()
	{
		using namespace engone;
		AddFont("consolas", new Font("assets/consolas"));
		AddShader("basic",new Shader(basic,true));
		AddShader("color",new Shader(coloring,true));
		//std::cout << GetShader("color")->error<<"\n";
		AddTexture("pencil",new Texture("assets/pencil.png"));
		AddTexture("brush",new Texture("assets/brush.png"));
		AddTexture("colorMarker",new Texture("assets/colorMarker.png"));
	}
	float last=0;
	struct Drag
	{
		float x, y, x2, y2;
	};
	std::vector<Drag> draggings;
	void Tick()
	{
		float delta = glfwGetTime() - last;
		last = glfwGetTime();

		engone::UpdatePanels(delta);

		if (selectedImage != nullptr) {
			if (selectedImage->selectedLayer != nullptr) {
				while (draggings.size() > 0) {
					tools::DragTool(selectedImage->selectedLayer, draggings[0].x, draggings[0].y, draggings[0].x2,draggings[0].y2);
					draggings.erase(draggings.begin());
				}
			}
		}

		glViewport(0, 0, winW, winH);

		glClearColor(0.05, 0.1, 0.15, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		Render();

		engone::RenderPanels();

		/*
		engone::Shader* shad = engone::GetShader("gui");
		shad->Bind();
		shad->SetVec2("uPos", { 100,100 });
		shad->SetVec2("uSize", { 1,1 });
		shad->SetVec4("uColor", 0.1, .1, .1, 1);
		shad->SetInt("uTextured", 1);
		
		engone::DrawString(engone::GetFont("consolas"),"Hello",true,50,100,50,0);
		*/
		//engone::DrawRect();

		glfwSwapBuffers(engone::GetWindow());
	}
	float lastDrawX = -1, lastDrawY = -1;
	void Init()
	{
		using namespace engone;
		winW = Width();
		winH = Height();
		InitAssets();
		history::Init();

		Panel* filePanel = new Panel();
		filePanel->Left(0)->Top(0)->Width(60)->Height(25)->Color({ 1.f });
		Transition& t = filePanel->AddTransition("a");
		t.eventType = EventType::Move;
		t.Fade({ .8f,.8f,1.f }, 0.2);
		filePanel->AddComponent(new Text("File", GetFont("consolas"), true));
		AddPanel(filePanel);

		Panel* toolPanel = new Panel();
		toolPanel->Right(0)->CenterY(0)->Width(40)->Height(200)->Color({.8f,.9f,.9f});
		Grid* grid = new Grid(1, 0, [](int index,Event& e) {
			if (index == 0) {
				tools::SetTool(tools::ToolPencil);
			}else if (index == 1) {
				tools::SetTool(tools::ToolBrush);
			}
			return false;
			});
		grid->AddItem(GetTexture("pencil"));
		grid->AddItem(GetTexture("brush"));
		toolPanel->AddComponent(grid);
		AddPanel(toolPanel);

		Panel* colorPanel = new Panel();
		colorPanel->Left(0)->Bottom(0)->Width(200)->Height(200)->Color({.8f,.9f,.9f});
		class ColorComp : public Component
		{
		public:
			ColorComp()=default;
			
			bool selectedHue = false;
			bool selectedFade = false;
			bool selectedAlpha = false;

			// hue is a decimal from 0 to 1
			void Hue(float* rgb,float hue,float saturation=1, float value=1)
			{
				hue *= 6;
				if (hue <= 1) {
					rgb[0] = 1;
					rgb[1] = hue;
					rgb[2] = 0;
				}
				else if (hue <= 2) {
					rgb[0] = 2-hue;
					rgb[1] = 1;
					rgb[2] = 0;
				}
				else if (hue <= 3) {
					rgb[0] = 0;
					rgb[1] = 1;
					rgb[2] = hue-2;
				}
				else if (hue <= 4) {
					rgb[0] = 0;
					rgb[1] = 4-hue;
					rgb[2] = 1;
				}
				else if (hue <= 5) {
					rgb[0] = hue-4;
					rgb[1] = 0;
					rgb[2] = 1;
				}
				else if (hue <= 6) {
					rgb[0] = 1;
					rgb[1] = 0;
					rgb[2] = 6-hue;
				}
				
				rgb[0] = (1 + (rgb[0]-1) * saturation) * value;
				rgb[1] = (1 + (rgb[1]-1) * saturation) * value;
				rgb[2] = (1 + (rgb[2]-1) * saturation) * value;
			}

			float hue = 0;
			float saturation = 1;
			float value = 0;
			float alpha = 1;
			void Init()
			{

			}
			virtual bool OnEvent(Event& e) override
			{
				if (e.eventType == EventType::Click&&e.button == GLFW_MOUSE_BUTTON_1 && e.action == 0) {
					selectedHue = false;
					selectedFade= false;
					selectedAlpha = false;
				}
				if (panel->Inside(e.mx,e.my)) {
					if (fadeX<e.mx&&e.mx<fadeX+fadeW) {
						if (fadeY < e.my && e.my<fadeY+fadeH) {
							if (e.eventType==EventType::Click&&e.button == GLFW_MOUSE_BUTTON_1 && e.action == 1) {
								selectedFade = true;
							}
						}
						else if (hueY < e.my && e.my<hueY+hueH) {
							if (e.eventType == EventType::Click&&e.button == GLFW_MOUSE_BUTTON_1 && e.action == 1) {
								selectedHue = true;
							}
						}
						else if (alphaY< e.my && e.my < alphaY+alphaH) {
							if (e.eventType == EventType::Click && e.button == GLFW_MOUSE_BUTTON_1 && e.action == 1) {
								selectedAlpha = true;
							}
						}
					}
				}
				if (selectedFade) {
					saturation = 1-(e.mx - fadeX) / (fadeW);
					value = 1-(e.my - fadeY) / (fadeH);
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
				if (selectedHue) {
					hue = (e.mx - hueX) / (hueW);
					if (hue < 0)
						hue = 0;
					if (hue > 1) {
						hue = 1;
					}
				}
				if (selectedAlpha) {
					alpha = (e.mx - alphaX) / (alphaW);
					if (alpha < 0)
						alpha = 0;
					if (alpha > 1) {
						alpha = 1;
					}
				}
				if (selectedFade || selectedHue || selectedAlpha) {
					float f[3];
					Hue(f, hue, saturation, value);
					tools::SetColor(f[0], f[1], f[2], alpha);
					return true;
				}
				return false;
			}
			virtual void OnRender() override
			{
				fadeX = panel->renderX+5;
				fadeY = panel->renderY+5;
				fadeW = panel->renderW-10;
				fadeH = panel->renderH-50;
				hueX = fadeX;
				hueY = fadeY + fadeH;
				hueW = fadeW;
				hueH = 20;
				alphaX = fadeX;
				alphaY = hueY + hueH;
				alphaW = fadeW;
				alphaH = 20;
				Shader* shad = engone::GetShader("color");
				shad->Bind();
				shad->SetVec2("uPos", {fadeX,fadeY});
				shad->SetVec2("uSize", { fadeW,fadeH});
				shad->SetVec2("uWindow", { winW,winH });

				float f[3];
				Hue(f,hue);
				shad->SetVec3("uColor", { f[0],f[1],f[2]});
				shad->SetInt("uShaderType", 0);
				
				engone::DrawRect();

				shad->SetVec2("uPos", { hueX,hueY});
				shad->SetVec2("uSize", { hueW, hueH });
				shad->SetInt("uShaderType", 1);
				//shad->SetVec2("uWindow", { winW,winH });
				//colorBuffer.Draw();
				engone::DrawRect();

				shad->SetVec2("uPos", { alphaX,alphaY});
				shad->SetVec2("uSize", { alphaW,alphaH});
				shad->SetInt("uShaderType", 2);
				//shad->SetVec2("uWindow", { winW,winH });
				//colorBuffer.Draw();
				engone::DrawRect();

				Shader* guiShader = engone::GetShader("gui");
				guiShader->Bind();

				engone::GetTexture("colorMarker")->Bind();
				int w = engone::GetTexture("colorMarker")->GetWidth();
				int h = engone::GetTexture("colorMarker")->GetHeight();
				guiShader->SetInt("uTextured", 1);
				guiShader->SetVec2("uSize", {w,h});
				guiShader->SetVec4("uColor", 1,1,1,1);

				guiShader->SetVec2("uPos", {fadeX+fadeW*(1-saturation)-w/2,fadeY+fadeH*(1-value)-w/2});
				engone::DrawRect();

				guiShader->SetVec2("uPos", { hueX+ hueW * hue-w/2,hueY + hueH/2-h/2 });
				engone::DrawRect();

				guiShader->SetVec2("uPos", { alphaX + alphaW * alpha - w / 2,alphaY + alphaH / 2 - h / 2 });
				engone::DrawRect();
			}
		private:
			float fadeX, fadeY, fadeW, fadeH;
			float hueX, hueY, hueW, hueH;
			float alphaX, alphaY, alphaW, alphaH;
		};
		ColorComp* colorComp = new ColorComp();
		colorComp->Init();
		colorPanel->AddComponent(colorComp);
		AddPanel(colorPanel);

		Panel* propertyPanel = new Panel();
		propertyPanel->Left(0)->Top(0, filePanel)->Width(150)->Height(60)->Color({.8f,.9f,.9f});
		Text* sizeComp = new Text("2", GetFont("consolas"), true, true);
		propertyPanel->AddComponent(sizeComp);
		AddListener(new Listener(EventType::Key|EventType::Scroll, [=](Event& e) {
			if (e.eventType == EventType::Key) {
				if (sizeComp->isEditing) {
					if (sizeComp->text.size() > 0) {
						try {
							tools::SetSize(std::stoi(sizeComp->text));
						}
						catch (std::invalid_argument) {

						}
					}
				}
			}
			else if(e.eventType==EventType::Scroll) {
				if (propertyPanel->Inside(e.mx, e.my)) {
					float newSize = tools::GetSize() - e.scrollY;
					if (newSize < 0)
						newSize = 0;
					tools::SetSize(newSize);
					
					sizeComp->text = std::to_string((int)newSize);
					return true;
				}
			}
			return false;
			}));
		AddPanel(propertyPanel);
		
		AddListener(new Listener(EventType::Key,-10, [=](Event& e) {
			if (e.action==1) {
				if (e.key==GLFW_KEY_P) {
					tools::SetTool(tools::ToolPencil);
				}
				else if(e.key==GLFW_KEY_B) {
					tools::SetTool(tools::ToolBrush);
				}
			}
			return false;
			}));
		AddListener(new Listener(EventType::Click | EventType::Move | EventType::Scroll,-10, [=](Event& e) {
			//std::cout << e.mx << " " << e.my << " " << e.action << " " << e.button << "\n";
			if (e.eventType == EventType::Click) {
				if (e.button == GLFW_MOUSE_BUTTON_1) {
					doDraw = e.action == 1;
				
					if (e.action==1) {
						selectedImage = nullptr;
						for (int i = 0; i < images.size(); i++) {
							Image* image = images[i];
							float tx = ((float)e.mx - winW / 2) / zoom + image->width / 2 - offsetX - image->x;
							float ty = (winH / 2 - (float)e.my) / zoom + image->height / 2 - offsetY - image->y;
							
							//std::cout << tx << " " << ty << "\n";
							if (tx > 0 && tx < image->width && ty>0 && ty < image->height) {
								selectedImage = image;
								if (image->selectedLayer != nullptr) {
									lastDrawX = tx;
									lastDrawY = ty;
									//tools::SetColor(1,0,0,1);
									tools::UseTool(image->selectedLayer,tx,ty);
								}
							}
						}
					}
				}
				else if (e.button == GLFW_MOUSE_BUTTON_2) {
					//move = e.action == 1;
				}
				else if (e.button == GLFW_MOUSE_BUTTON_3) {
					move = e.action == 1;
					lastmx = e.mx;
					lastmy = e.my;
				}
			}
			else if (e.eventType == EventType::Move) {
				if (doDraw&& selectedImage != nullptr) {
					Layer* layer= selectedImage->selectedLayer;
					if (selectedImage->selectedLayer != nullptr) {
						if (layer->data != nullptr) {
							float tx = ((float)e.mx - winW / 2) / zoom + layer->width / 2 - offsetX- selectedImage->x;
							float ty = (winH / 2 - (float)e.my) / zoom + layer->height / 2 - offsetY- selectedImage->y;
							//if (tx > 0 && tx < layer->width && ty>0 && ty < layer->height) {

								/*
								if (lastDrawX == -1) {
									lastDrawX = tx;
									lastDrawY = ty;
								}*/
							//std::cout << "Draw\n";
							//tools::SetColor(0, 0, 0, 1);
							draggings.push_back({ lastDrawX,lastDrawY,tx,ty });
								//tools::DragTool(layer,lastDrawX,lastDrawY,tx,ty);
							//std::cout << "stop\n";

								//tools::DragBrush(layer,lastDrawX,lastDrawY, tx, ty);
								
								lastDrawX = tx;
								lastDrawY = ty;
							//}
						}
					}
				}
				if (move) {
					offsetX += (e.mx - lastmx) / zoom;
					offsetY -= (e.my - lastmy) / zoom;
					lastmx = e.mx;
					lastmy = e.my;
				}
			}
			else if (e.eventType == EventType::Scroll) {
				if ((int)e.scrollY  == e.scrollY) {
					float lastZoom = zoom;
					zoom *= (e.scrollY > 0 ? pow(1.1, abs(e.scrollY)) : pow(0.9, abs(e.scrollY)));
					offsetX += ((e.mx - winW / 2) / zoom - (e.mx - winW / 2) / lastZoom);
					offsetY -= ((e.my - winH / 2) / zoom - (e.my - winH / 2) / lastZoom);
				}
				else {
					offsetX += e.scrollX*18/zoom;
					offsetY -= e.scrollY*18/zoom;
				}
			}
			return false;
			}));
		AddListener(new Listener(EventType::Resize, [=](Event& e) {
			winW = e.width;
			winH = e.height;
			Tick();
			return false;
			}));

		Image* image = new Image();
		image->x = 0;
		image->Init(1000, 1000);
		images.push_back(image);
		tools::SetSize(3);
		tools::DragBrush(image->layers[0], 450, 450, 550, 530);
		tools::DragBrush(image->layers[0], 400, 400, 320,500);

		/*
		Image* image1 = new Image();
		image1->x = 820;
		image1->Init(800, 800);
		images.push_back(image1);
		*/
		/*
		Image* image2 = new Image();
		image2->y = 820;
		image2->Init(800, 800);
		images.push_back(image2);

		Image* image3 = new Image();
		image3->x = 820; image3->y = 820;
		image3->Init(800, 800);
		images.push_back(image3);
		*/

		//layer->Refresh();
		/*
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 100, 100, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 100, 100);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Oh no " << status << "\n";
		*/
	}


	void Render()
	{
		using namespace engone;
		// Render images
		GetShader("basic")->Bind();
		GetShader("basic")->SetVec2("uOffset", {offsetX, offsetY});
		GetShader("basic")->SetVec2("uTransform", {zoom / winW, zoom / winH});

		for (int i = 0; i < images.size();i++) {
			images[i]->Render();
		}
	}
}