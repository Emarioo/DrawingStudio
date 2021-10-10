#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <random>
#include <fstream>
#include <sys/stat.h>

namespace engone
{
	bool FindFile(const std::string& path);
	std::vector<std::string> ReadFile(const std::string& path);
	std::vector<std::string> SplitString(std::string text, const std::string& delim);
	std::string SanitizeString(std::string s);
	float lerp(float a, float b, float c);
	float inverseLerp(float min, float max, float x);
	float distance(float x, float y, float x1, float y1);
	float bezier(float x, float xStart, float xEnd);
	std::string Crypt(const std::string& word, const std::string& key, bool encrypt);
	double GetSystemTime();

	double GetRandom();
	std::string GetClock();
	void CountingTest(int times, int numElements, std::function<int()> func);
#ifdef ENGONE_DEF
	bool FindFile(const std::string& path)
	{
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}
	std::vector<std::string> ReadFile(const std::string& path)
	{
		std::ifstream file(path);

		std::vector<std::string> out;
		if (!file) {
			return out;
		}
		std::string line;
		while (getline(file, line))
		{
			out.push_back(line);
		}
		file.close();
		return out;
	}
	std::vector<std::string> SplitString(std::string text, const std::string& delim)
	{
		std::vector<std::string> out;
		unsigned int at = 0;
		while ((at = text.find(delim)) != std::string::npos) {
			std::string push = text.substr(0, at);
			if (push.size() > 0) {
				out.push_back(push);
			}
			text.erase(0, at + delim.length());
		}
		out.push_back(text);
		return out;
	}
	static char SanitizedChars[64]{
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'0','1','2','3','4','5','6','7','8','9','_','-'
	};
	std::string SanitizeString(std::string s)
	{
		std::string out;
		for (int i = 0; i < s.size(); i++) {
			for (int j = 0; j < 64; j++) {
				if (s.at(i) == SanitizedChars[j]) {
					out += s.at(i);
					break;
				}
			}
		}
		return out;
	}
	float lerp(float a, float b, float c)
	{
		return (1 - c) * a + c * b;
	}
	float inverseLerp(float min, float max, float x)
	{
		return (x - min) / (max - min);
	}
	float distance(float x, float y, float x1, float y1)
	{
		return sqrt(pow(x1 - x, 2) + pow(y1 - y, 2));
	}
	float bezier(float x, float xStart, float xEnd)
	{
		float t = (x - xStart) / (xEnd - xStart);
		float va = /*(pow(1 - t, 3) + 3 * pow(1 - t, 2) * t) +*/(3 * (1 - t) * pow(t, 2) + pow(t, 3));
		return va;
	}
	std::string Crypt(const std::string& word, const std::string& key, bool encrypt)
	{
		std::string out = "";
		for (int i = 0; i < word.length(); i++) {
			int at = word[i];
			for (int j = 0; j < key.length(); j++) {
				int val = (i - j) * key[j];
				if (encrypt) at += val;
				else at -= val;
			}
			out += (at % 256);
		}
		return out;
	}
	double GetSystemTime()
	{
		return std::chrono::system_clock::now().time_since_epoch().count() / 10000000.0;
	}
	static std::mt19937 mt19937_random;
	std::uniform_real_distribution uniform_distrubtion;
	double GetRandom()
	{
		mt19937_random.seed(GetSystemTime());
		return uniform_distrubtion(mt19937_random);
	}
	std::string GetClock()
	{
		time_t t;
		time(&t);
		tm tm;
		localtime_s(&tm, &t);
		std::string str = std::to_string(tm.tm_hour) + ":" + std::to_string(tm.tm_min) + ":" + std::to_string(tm.tm_sec);
		return str;
	}
	void CountingTest(int times, int numElements, std::function<int()> func)
	{
		std::vector<int> occurrences(numElements);

		double bef = engone::GetSystemTime();
		for (int i = 0; i < times; i++) {
			int index = func();
			if (index > -1 && index < numElements)
				occurrences[func()]++;
		}
		double aft = engone::GetSystemTime();
		for (int i = 0; i < numElements; i++) {
			std::cout << i << " " << occurrences[i] << std::endl;
		}
		std::cout << "Time: " << (aft - bef) << std::endl;
	}
#endif
}