#include "Studio.h"

#include <vector>
#include <unordered_map>

#include "Tools.h"

namespace studio
{
	const std::string basic = {
#include "../Shaders/basic.glsl"
	};
	const std::string coloring = {
#include "../Shaders/color.glsl"
	};

	// images in the studio
	static std::vector<Image*> images;
	static Image* selectedImage;
	static int winW = 500, winH = 500;

	static bool doDraw, move;
	static int lastmx, lastmy;

	static float offsetX = 0, offsetY = 0;
	static float zoom = 2;

	void InitAssets()
	{
		assets::AddFont("consolas", new Font("assets/consolas"));
		assets::AddShader("basic",new Shader(basic));
		assets::AddShader("color",new Shader(coloring));
		assets::AddTexture("pencil",new Texture("assets/pencil.png"));
		assets::AddTexture("brush",new Texture("assets/brush.png"));
		assets::AddTexture("colorMarker",new Texture("assets/colorMarker.png"));

	}
	float last=0;
	void Tick()
	{
		float delta = glfwGetTime() - last;
		last = glfwGetTime();

		overlay::UpdatePanels(delta);

		glViewport(0, 0, winW, winH);

		glClearColor(0.05, 0.1, 0.15, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		Render();

		overlay::RenderPanels();

		//overlay::GetShader().Bind();
		
		//overlay::GetShader().SetVec2("uPos", { 100,100 });
		//overlay::GetShader().SetVec2("uSize", { 1,1 });
		//overlay::GetShader().SetVec4("uColor", 0.5, 1, .2, 1);
		//overlay::GetShader().SetInt("uTextured", 1);
		
		//renderer::DrawString(assets::GetFont("consolas"),"Hello",true,50,100,50,0);

		glfwSwapBuffers(renderer::GetWindow());
	}
	float lastDrawX = -1, lastDrawY = -1;
	void Init()
	{
		winW = renderer::Width();
		winH = renderer::Height();
		InitAssets();
		history::Init();

		using namespace overlay;
		Panel* filePanel = new Panel();
		filePanel->x.Left(0)->y.Top(0)->w.Center(60)->h.Center(25);
		filePanel->color = { 1.f,1.f,1.f,1.f };
		Transition& t = filePanel->AddTransition("a");
		t.eventType = input::EventType::Move;
		t.Fade({ .8f,.8f,1.f }, 0.2);
		filePanel->AddComponent(new Text("File", assets::GetFont("consolas"), true));
		AddPanel(filePanel);

		Panel* toolPanel = new Panel();
		toolPanel->x.Right(0)->y.Center(0)->w.Center(40)->h.Center(200);
		toolPanel->color = { .8f,0.9f,0.9f };
		Grid* grid = new Grid(1, 0, [](int index,input::Event& e) {
			if (index == 0) {
				tools::SetTool(tools::ToolPencil);
			}else if (index == 1) {
				tools::SetTool(tools::ToolBrush);
			}
			return false;
			});
		grid->AddItem(assets::GetTexture("pencil"));
		grid->AddItem(assets::GetTexture("brush"));
		toolPanel->AddComponent(grid);
		AddPanel(toolPanel);

		Panel* colorPanel = new Panel();
		colorPanel->x.Left(0)->y.Bottom(0)->w.Center(200)->h.Center(200);
		colorPanel->color = { .8f,0.9f,0.9f };
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
				float vert[]{
					0,0,//1,0,0,1,
					1,0,//1,1,1,1,
					1,1,//0,0,0,1,
					0,1,//0,0,0,1
				};
				unsigned int index[]{
					0,1,2,
					2,3,0
				};
				colorBuffer.Init(true, vert, 4 * 2, index, 6);
				colorBuffer.SetAttrib(0, 2, 2, 0);
			}
			virtual bool Event(input::Event& e) override
			{
				if (e.eventType == input::EventType::Click&&e.button == GLFW_MOUSE_BUTTON_1 && e.action == 0) {
					selectedHue = false;
					selectedFade= false;
					selectedAlpha = false;
				}
				if (panel->Inside(e.mx,e.my)) {
					if (fadeX<e.mx&&e.mx<fadeX+fadeW) {
						if (fadeY < e.my && e.my<fadeY+fadeH) {
							if (e.eventType==input::EventType::Click&&e.button == GLFW_MOUSE_BUTTON_1 && e.action == 1) {
								selectedFade = true;
							}
						}
						else if (hueY < e.my && e.my<hueY+hueH) {
							if (e.eventType == input::EventType::Click&&e.button == GLFW_MOUSE_BUTTON_1 && e.action == 1) {
								selectedHue = true;
							}
						}
						else if (alphaY< e.my && e.my < alphaY+alphaH) {
							if (e.eventType == input::EventType::Click && e.button == GLFW_MOUSE_BUTTON_1 && e.action == 1) {
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
			virtual void Render() override
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
				Shader* shad = assets::GetShader("color");
				shad->Bind();
				shad->SetVec2("uPos", {fadeX,fadeY});
				shad->SetVec2("uSize", { fadeW,fadeH});
				shad->SetVec2("uWindow", { winW,winH });
				float f[3];
				Hue(f,hue);
				shad->SetVec3("uColor", { f[0],f[1],f[2]});
				shad->SetInt("uShaderType", 0);
				colorBuffer.Draw();

				//shad = assets::GetShader("hue");
				//shad->Bind();
				shad->SetVec2("uPos", { hueX,hueY});
				shad->SetVec2("uSize", { hueW, hueH });
				shad->SetInt("uShaderType", 1);
				//shad->SetVec2("uWindow", { winW,winH });
				colorBuffer.Draw();

				shad->SetVec2("uPos", { alphaX,alphaY});
				shad->SetVec2("uSize", { alphaW,alphaH});
				shad->SetInt("uShaderType", 2);
				//shad->SetVec2("uWindow", { winW,winH });
				colorBuffer.Draw();

				overlay::GetShader().Bind();

				assets::GetTexture("colorMarker")->Bind();
				int w = assets::GetTexture("colorMarker")->GetWidth();
				int h = assets::GetTexture("colorMarker")->GetHeight();
				overlay::GetShader().SetInt("uTextured", 1);
				overlay::GetShader().SetVec2("uSize", {w,h});
				overlay::GetShader().SetVec4("uColor", 1,1,1,1);

				overlay::GetShader().SetVec2("uPos", {fadeX+fadeW*(1-saturation)-w/2,fadeY+fadeH*(1-value)-w/2});
				renderer::DrawRect();

				overlay::GetShader().SetVec2("uPos", { hueX+ hueW * hue-w/2,hueY + hueH/2-h/2 });
				renderer::DrawRect();

				overlay::GetShader().SetVec2("uPos", { alphaX + alphaW * alpha - w / 2,alphaY + alphaH / 2 - h / 2 });
				renderer::DrawRect();
			}
		private:
			TriangleBuffer colorBuffer;
			float fadeX, fadeY, fadeW, fadeH;
			float hueX, hueY, hueW, hueH;
			float alphaX, alphaY, alphaW, alphaH;
		};
		ColorComp* colorComp = new ColorComp();
		colorComp->Init();
		colorPanel->AddComponent(colorComp);
		AddPanel(colorPanel);

		Panel* propertyPanel = new Panel();
		propertyPanel->x.Left(0)->y.Bottom(0,colorPanel)->w.Center(150)->h.Center(60);
		propertyPanel->color = { .8f,0.9f,0.9f };
		propertyPanel->AddComponent(new Text("Size: ",assets::GetFont("consolas"),true,true));
		AddPanel(propertyPanel);

		input::AddListener(new input::Listener(input::EventType::Key,-10, [=](input::Event& e) {
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
		input::AddListener(new input::Listener(input::EventType::Click | input::EventType::Move | input::EventType::Scroll,-10, [=](input::Event& e) {
			//std::cout << e.mx << " " << e.my << " " << e.action << " " << e.button << "\n";

			//std::cout << (int)e.eventType <<" " << (e.eventType == input::EventType::Scroll) << " \n";
			if (e.eventType == input::EventType::Click) {
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
									tools::SetColor(1,0,0,1);
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
			else if (e.eventType == input::EventType::Move) {
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
							tools::SetColor(0, 0, 0, 1);
								tools::DragTool(layer,lastDrawX,lastDrawY,tx,ty);
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
			else if (e.eventType == input::EventType::Scroll) {
				float lastZoom = zoom;
				zoom *= (e.scroll > 0 ? pow(1.1, abs(e.scroll)) : pow(0.9, abs(e.scroll)));
				offsetX += ((e.mx - winW / 2) / zoom - (e.mx - winW / 2) / lastZoom);
				offsetY -= ((e.my - winH / 2) / zoom - (e.my - winH / 2) / lastZoom);
			}
			return false;
			}));
		input::AddListener(new input::Listener(input::EventType::Resize, [=](input::Event& e) {
			winW = e.width;
			winH = e.height;
			Tick();
			return false;
			}));

		Image* image = new Image();
		image->x = 0;
		image->Init(200, 200);
		images.push_back(image);

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

		/*
		for (int i = 0; i < 100 * 100; i++) {
			layer->data[i * 4] = 255;
			layer->data[i * 4 + 1] = 255;
			layer->data[i * 4 + 2] = 255;
		}*/
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
		using namespace assets;
		// Render images
		GetShader("basic")->Bind();
		GetShader("basic")->SetVec2("uOffset", {offsetX, offsetY});
		GetShader("basic")->SetVec2("uTransform", {zoom / winW, zoom / winH});

		for (int i = 0; i < images.size();i++) {
			images[i]->Render();
		}
	}
}