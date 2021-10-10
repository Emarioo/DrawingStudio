#pragma once

#include "AssetManager.h"

namespace assets
{
	std::unordered_map<std::string, Font*> fonts;
	std::unordered_map<std::string, Texture*> textures;
	std::unordered_map<std::string, Shader*> shaders;
	Font* GetFont(const std::string& name)
	{
		return fonts[name];
	}
	void AddFont(const std::string& name, Font* font)
	{
		fonts[name] = font;
	}
	Shader* GetShader(const std::string& name)
	{
		return shaders[name];
	}
	void AddShader(const std::string& name, Shader* shader)
	{
		shaders[name] = shader;
	}
	Texture* GetTexture(const std::string& name)
	{
		return textures[name];
	}
	void AddTexture(const std::string& name, Texture* texture)
	{
		textures[name] = texture;
	}
}