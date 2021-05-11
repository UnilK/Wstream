#ifndef WAVESTREAM_H
#define WAVESTREAM_H

#include <fstream>
#include <vector>
#include <string>

class waveconfig{

protected:

	/*
	  	I'm following the documentation on the wave format found on this site:
		http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html
	*/

	// standard info
	uint16_t
		format = 0,			// 0x0001 (PCM), 0x0003 (IEEE) or 0xfffe (EXTENSIBLE)
		channels = 0,		// 1 - 18 of channels
		frameSize = 0,		// 1 frame = (1 sample from each channel)
		sampleBits = 0,		// number of (used?) bits in a sample.
		sampleSize = 0,		// sample size in bytes.
		extensionSize = 0;	// Useless, formatSize already has this info


	uint32_t
		fileSize = 0,		// (size of the whole file in bytes) - 4 
		formatSize = 0,		// format chunk size
		frameRate = 0,		// sampling rate, frames per second
		byteRate = 0,		// bytes per second
		dataSize = 0;		// data chunk size



	// info for extensible format:

	uint16_t 
		validSampleBits = 0,	// same as sampleBits, this is somewhat confusing.
		subformat = 0;			// actual format of the wave format extensible.

	uint32_t
		channelMask = 0;	// bitmask of the speaker positions for the channels
	/*

	   	the positions in the mask are:

		0.   Front Left - FL,
		1.   Front Right - FR,
		2.   Front Center - FC
		3.   Low Frequency - LF
		4.   Back Left - BL
		5.   Back Right - BR
		6.   Front Left of Center - FLC
		7.   Front Right of Center - FRC
		8.   Back Center - BC
		9.   Side Left - SL
		10.  Side Right - SR
		11.  Top Center - TC
		12.  Top Front Left - TFL
		13.  Top Front Center - TFC
		14.  Top Front Right - TFR
		15.  Top Back Left - TBL
		16.  Top Back Center - TBC
		17.  Top Back Right - TBR
		
		mono uses FL							000000000000000001
		stereo uses FL, FR						000000000000000011
		5.1 uses FL, FR, FC, LF, BL, BR 		000000000000111111 (probably)
		7.1 uses FL, FR, FC, LF, BL, BR, SL, SR 000000011000111111 (I guess)
		
	*/

	char GUID[16];	// the first 2 bytes are the subformat.

	std::vector<std::string> log;

public:

	waveconfig();
	waveconfig(
			uint16_t format,
			uint16_t channel_amount,
			uint16_t sample_size,
			uint32_t frame_rate,
			uint16_t subformat = 0,
			uint32_t mask = 0);

	waveconfig(waveconfig *other);

	void add_log(std::string message);
	std::vector<std::string> get_log();

	bool set_format(uint16_t);
	bool set_channel_amount(uint16_t);
	bool set_sample_bitsize(uint16_t);
	bool set_frame_rate(uint32_t);		// frames per second
	bool set_subformat(uint16_t);
	bool set_channel_mask(uint32_t);

	bool config(
			uint16_t format,
			uint16_t channel_amount,
			uint16_t sample_size,
			uint32_t frame_rate,
			uint16_t subformat = 0,
			uint32_t mask = 0);

	bool copy_config(waveconfig*);

};



class iwavestream : public waveconfig{

protected:
	
	std::string source;
	std::ifstream wavFile;
	uint32_t dataBegin;

	float (*listen_data)(char*);

	uint16_t read_uint16();
	uint32_t read_uint32();

	bool handle_unexpected_chunk();
	bool compare_id(char*, std::string);

public:
	
	iwavestream();
	iwavestream(std::string source_);

	bool initialize();

	bool open(std::string source_);

	bool read_frames(std::vector<float> *waves, uint32_t amount);
	bool read_frames(std::vector<float> *waves, uint32_t beginFrame, uint32_t amount);
	bool read_file(std::vector<float> *waves);
		
};



class owavestream : public waveconfig{

protected:

	std::string outSource;
	std::ofstream wavFile;

	void (*speak_data)(float, char*);
	
	void write_uint16(uint16_t);
	void write_uint32(uint32_t);

public:

	owavestream();
	owavestream(std::string file);
	owavestream(
			std::string outSource_,
			uint16_t format,
			uint16_t channel_amount,
			uint16_t sample_size,
			uint32_t frame_rate,
			uint16_t subformat = 0,
			uint32_t mask = 0);

	owavestream(std::string outSource_, waveconfig *other);

	bool open(std::string outSource_);

	bool write_file(std::vector<float> *waves);
	bool write_file(std::vector<float> *waves, waveconfig*);

};

#endif