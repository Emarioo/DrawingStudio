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
	static float zoom = 1;

	void InitAssets()
	{
		assets::AddFont("consolas", new Font("assets/consolas"));
		assets::AddShader("basic",new Shader(basic));
		assets::AddShader("color",new Shader(coloring));
		assets::AddTexture("pencil",new Texture("assets/pencil.png"));
		assets::AddTexture("brush",new Texture("assets/brush.png"));

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

		glfwSwapBuffers(renderer::GetWindow());
	}
	int lastDrawX = -1;
	int lastDrawY = -1;
	void Init()
	{
		InitAssets();

		using namespace overlay;
		Panel* filePanel = new Panel();
		filePanel->x.Left(2)->y.Top(2)->w.Center(60)->h.Center(25);
		filePanel->color = { 1.f,1.f,1.f,1.f };
		Transition& t = filePanel->AddTransition("a");
		t.eventType = input::EventType::Move;
		t.Fade({ .8f,.8f,1.f }, 0.2);
		filePanel->AddComponent(new Text("File", assets::GetFont("consolas"), true));
		AddPanel(filePanel);

		Panel* toolPanel = new Panel();
		toolPanel->x.Right(2)->y.Center(0)->w.Center(40)->h.Center(200);
		toolPanel->color = { .8f,0.9f,0.9f };
		Grid* grid = new Grid(1, 0, [](int index,input::Event& e) {

			});
		grid->AddItem(assets::GetTexture("pencil"));
		grid->AddItem(assets::GetTexture("brush"));
		toolPanel->AddComponent(grid);
		AddPanel(toolPanel);

		Panel* colorPanel = new Panel();
		colorPanel->x.Left(50)->y.Center(0)->w.Center(200)->h.Center(200);
		colorPanel->color = { .8f,0.9f,0.9f };
		class ColorComp : public Component
		{
		public:
			ColorComp()=default;
			
			float r, g, b, a;
			void Init()
			{
				float vert[]{
					0,0,1,1,1,1,
					1,0,1,1,1,1,
					1,1,0,0,0,1,
					0,1,0,0,0,1
				};
				unsigned int index[]{
					0,1,2,
					2,3,0
				};
				buffer.Init(true,vert,4*6,index,6);
				buffer.SetAttrib(0,2,6,0);
				buffer.SetAttrib(1,4,6,2);
			}
			void SetColor(float r,float g,float b,float a)
			{
				this->r = r;
				this->g = g;
				this->b = b;
				this->a = a;
				float data[]{
					0,0,r,g,b,a,
					1,0,1,1,1,1,
					1,1,0,0,0,1,
					0,1,0,0,0,1
				};
				buffer.ModifyVertices(0,4*6,data);
			}
			virtual void Event(input::Event& e) override
			{
				
			}
			virtual void Render() override
			{
				Shader* shad = assets::GetShader("color");
				shad->Bind();
				shad->SetVec2("uPos", {panel->renderX+5,panel->renderY+5});
				shad->SetVec2("uSize", {panel->renderW-10,panel->renderW-10});
				shad->SetVec2("uWindow", {winW,winH});

				buffer.Draw();

				overlay::GetShader().Bind();
			}
		private:
			TriangleBuffer buffer;
		};
		ColorComp* colorComp = new ColorComp();
		colorComp->Init();
		colorPanel->AddComponent(colorComp);
		AddPanel(colorPanel);

		input::AddListener(new input::Listener(input::EventType::Key, [=](input::Event& e) {
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
		input::AddListener(new input::Listener(input::EventType::Click | input::EventType::Move | input::EventType::Scroll, [=](input::Event& e) {
			//std::cout << e.mx << " " << e.my << " " << e.action << " " << e.button << "\n";

			//std::cout << (int)e.eventType <<" " << (e.eventType == input::EventType::Scroll) << " \n";
			if (e.eventType == input::EventType::Click) {
				if (e.button == GLFW_MOUSE_BUTTON_1) {
					doDraw = e.action == 1;
				
					if (e.action==1) {
						selectedImage = nullptr;
						for (int i = 0; i < images.size(); i++) {
							Image* image = images[i];
							int tx = ((float)e.mx - winW / 2) / zoom + image->width / 2 - offsetX - image->x;
							int ty = (winH / 2 - (float)e.my) / zoom + image->height / 2 - offsetY - image->y;
							
							//std::cout << tx << " " << ty << "\n";
							if (tx > 0 && tx < image->width && ty>0 && ty < image->height) {
								selectedImage = image;
								if (image->selectedLayer != nullptr) {
									lastDrawX = tx;
									lastDrawY = ty;
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
							int tx = ((float)e.mx - winW / 2) / zoom + layer->width / 2 - offsetX- selectedImage->x;
							int ty = (winH / 2 - (float)e.my) / zoom + layer->height / 2 - offsetY- selectedImage->y;
							if (tx > 0 && tx < layer->width && ty>0 && ty < layer->height) {

								/*
								if (lastDrawX == -1) {
									lastDrawX = tx;
									lastDrawY = ty;
								}*/
								tools::DragTool(layer,lastDrawX,lastDrawY,tx,ty);

								//tools::DragBrush(layer,lastDrawX,lastDrawY, tx, ty);
								
								lastDrawX = tx;
								lastDrawY = ty;
							}
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