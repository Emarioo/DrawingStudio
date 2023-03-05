#pragma once

//check big vs little endian machine
bool IsBigEndian(void);

int ConvertToInt(char* buffer, int len);
//Location and size of data is found here: http://www.topherlee.com/software/pcm-tut-wavformat.html
char* LoadWAV(std::string filename, int& channels, int& sampleRate, int& bps, int& size);
void writeWAV(std::string filename, int& channels, int& sampleRate, int& bps, int& size, char* data);