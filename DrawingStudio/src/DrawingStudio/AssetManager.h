#pragma once

#include <unordered_map>
#include <string>

#include "../Engone/Rendering/Font.h"
#include "../Engone/Rendering/Shader.h"
#include "../Engone/Rendering/Texture.h"

namespace assets
{
	Font* GetFont(const std::string& name);
	void AddFont(const std::string& name, Font* font);
	Shader* GetShader(const std::string& name);
	void AddShader(const std::string& name, Shader* shader);
	Texture* GetTexture(const std::string& name);
	void AddTexture(const std::string& name, Texture* texture);
}