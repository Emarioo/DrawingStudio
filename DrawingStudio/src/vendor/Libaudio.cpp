#include "gonpch.h"

#include "Libaudio.h"

//check big vs little endian machine
bool IsBigEndian(void)
{
	int a = 1;
	return !((char*)&a)[0];
}

int ConvertToInt(char* buffer, int len)
{
	int a = 0;

	if (!IsBigEndian())
	{
		for (int i = 0; i < len; ++i)
		{
			((char*)&a)[i] = buffer[i];
		}
	} else
	{
		for (int i = 0; i < len; ++i)
		{
			((char*)&a)[3 - i] = buffer[i];
		}
	}

	return a;
}
//Location and size of data is found here: http://www.topherlee.com/software/pcm-tut-wavformat.html
char* LoadWAV(std::string filename, int& channels, int& sampleRate, int& bps, int& size)
{
	char buffer[4];

	std::ifstream in(filename.c_str(),std::ios::in|std::ios::binary);
	//std::ofstream out("hello2.wav",std::ios::binary|std::ios::out);
	in.read(buffer, 4);
	//out.write(buffer, 4);

	if (strncmp(buffer, "RIFF", 4) != 0)
	{
		std::cout << "Error here, not a valid WAV file, RIFF not found in header\n This was found instead: "
			<< buffer[0] << buffer[1] << buffer[2] << buffer[3] << std::endl;
	}
	//std::cout << "[" << buffer[0] << buffer[1] << buffer[2] << buffer[3] << "]" << std::endl;
	in.read(buffer, 4);//size of file. Not used. Read it to skip over it.  
	//std::cout << "["<<buffer[0] << buffer[1] << buffer[2] << buffer[3] <<"]"<< std::endl;
	//out.write(buffer, 4);

	in.read(buffer, 4);//Format, should be WAVE
	//out.write(buffer, 4);

	if (strncmp(buffer, "WAVE", 4) != 0)
	{
		std::cout << "Error here, not a valid WAV file, WAVE not found in header.\n This was found instead: "
			<< buffer[0] << buffer[1] << buffer[2] << buffer[3] << std::endl;
	}

	in.read(buffer, 4);//Format Space Marker. should equal fmt (space)
	//out.write(buffer, 4);

	if (strncmp(buffer, "fmt ", 4) != 0)
	{
		std::cout << "Error here, not a valid WAV file, Format Marker not found in header.\n This was found instead: "
			<< buffer[0] << buffer[1] << buffer[2] << buffer[3] << std::endl;
	}

	in.read(buffer, 4);//Length of format data. Should be 16 for PCM, meaning uncompressed.
	//out.write(buffer, 4);

	if (ConvertToInt(buffer, 4) != 16)
	{
		std::cout << "Error here, not a valid WAV file, format length wrong in header.\n This was found instead: "
			<< ConvertToInt(buffer, 4) << std::endl;
	}

	in.read(buffer, 2);//Type of format, 1 = PCM
	//out.write(buffer, 2);

	if (ConvertToInt(buffer, 2) != 1)
	{
		std::cout << "Error here, not a valid WAV file, file not in PCM format.\n This was found instead: "
			<< ConvertToInt(buffer, 4) << std::endl;
	}

	in.read(buffer, 2);//Get number of channels. 
	//out.write(buffer, 2);

	//Assume at this point that we are dealing with a WAV file. This value is needed by OpenAL
	channels = ConvertToInt(buffer, 2);

	in.read(buffer, 4);//Get sampler rate. 
	//out.write(buffer, 4);

	sampleRate = ConvertToInt(buffer, 4);

	//Skip Byte Rate and Block Align. Maybe use later?
	in.read(buffer, 4);//Block Align
	//out.write(buffer, 4);

	in.read(buffer, 2);//ByteRate
	//out.write(buffer, 2);

	in.read(buffer, 2);//Get Bits Per Sample
	//out.write(buffer, 2);

	bps = ConvertToInt(buffer, 2);

	//Skip character data, which marks the start of the data that we care about. 
	in.read(buffer, 4);//"data" chunk. 
	//out.write(buffer, 4);

	in.read(buffer, 4); //Get size of the data
	//out.write(buffer, 4);

	size = ConvertToInt(buffer, 4);

	if (size < 0)
	{
		std::cout << "Error here, not a valid WAV file, size of file reports 0.\n This was found instead: "
			<< size << std::endl;
	}

	char* data = new char[size];

	in.read(data, size);//Read audio data into buffer, return.
	
	//out.write(data, size);

	in.close();
	//out.close();

	return data;
}
void writeWAV(std::string filename, int& channels, int& sampleRate, int& bps, int& size, char* data) {
	char buffer[4];

	std::ofstream out(filename.c_str());
	
	out.write("RIFF", 4);
	/*
	if (strncmp(buffer, "RIFF", 4) != 0)
	{
		std::cout << "Error here, not a valid WAV file, RIFF not found in header\n This was found instead: "
			<< buffer[0] << buffer[1] << buffer[2] << buffer[3] << std::endl;
	}*/

	out.write(buffer, 4);//size of file. Not used. Read it to skip over it.  

	out.write("WAVE", 4);//Format, should be WAVE
	/*
	if (strncmp(buffer, "WAVE", 4) != 0)
	{
		std::cout << "Error here, not a valid WAV file, WAVE not found in header.\n This was found instead: "
			<< buffer[0] << buffer[1] << buffer[2] << buffer[3] << std::endl;
	}*/

	out.write("fmt ", 4);//Format Space Marker. should equal fmt (space)
	/*
	if (strncmp(buffer, "fmt ", 4) != 0)
	{
		std::cout << "Error here, not a valid WAV file, Format Marker not found in header.\n This was found instead: "
			<< buffer[0] << buffer[1] << buffer[2] << buffer[3] << std::endl;
	}*/
	int a = 16;
	char* b = reinterpret_cast<char*>(&a);
	out.write(b, 4);//Length of format data. Should be 16 for PCM, meaning uncompressed.
	/*
	if (ConvertToInt(buffer, 4) != 16)
	{
		std::cout << "Error here, not a valid WAV file, format length wrong in header.\n This was found instead: "
			<< ConvertToInt(buffer, 4) << std::endl;
	}
	*/
	short c = 1;
	b = reinterpret_cast<char*>(&c);
	out.write(b, 2);//Type of format, 1 = PCM
	/*
	if (ConvertToInt(buffer, 2) != 1)
	{
		std::cout << "Error here, not a valid WAV file, file not in PCM format.\n This was found instead: "
			<< ConvertToInt(buffer, 4) << std::endl;
	}
	*/
	c = channels;
	b = reinterpret_cast<char*>(&c);
	out.write(b, 2);//Get number of channels. 

	//Assume at this point that we are dealing with a WAV file. This value is needed by OpenAL
	//channels = ConvertToInt(buffer, 2);
	b = reinterpret_cast<char*>(&sampleRate);
	out.write(b, 4);//Get sampler rate. 

	//sampleRate = ConvertToInt(buffer, 4);

	//Skip Byte Rate and Block Align. Maybe use later?
	out.write(buffer, 4);//Block Align
	out.write(buffer, 2);//ByteRate

	c = bps;
	b = reinterpret_cast<char*>(&c);
	out.write(b, 2);//Get Bits Per Sample

	//bps = ConvertToInt(buffer, 2);

	//Skip character data, which marks the start of the data that we care about. 
	out.write(buffer, 4);//"data" chunk. 

	b = reinterpret_cast<char*>(&size);
	out.write(b, 4); //Get size of the data
	/*
	size = ConvertToInt(buffer, 4);

	if (size < 0)
	{
		std::cout << "Error here, not a valid WAV file, size of file reports 0.\n This was found instead: "
			<< size << std::endl;
	}
	*/
	//char* data = new char[size];

	out.write(data, size);//Read audio data into buffer, return.

	out.close();
}