//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#ifndef AJ_WAV_FORMAT_STATE_H
#define AJ_WAV_FORMAT_STATE_H
//---------------------------------------------------------------------------

#include <string>
#include <vector>
#include <fstream>

class AJWavFormat
{
private:
    struct WaveHeader {
        char riff_label[4]; // (offset 0) = {‘R’, ‘I’, ‘F’, ‘F’}
        unsigned riff_size; // (offset 4) = 36 + data_size
        char file_tag[4]; // (offset 8) = {‘W’, ‘A’, ‘V’, ‘E’}
        char fmt_label[4]; // (offset 12) = {‘f’, ‘m’,’t’, ’ ‘}
        unsigned fmt_size; // (offset 16) = 16
        unsigned short audio_format; // (offset 20) = 1
        unsigned short channel_count; // (offset 22) = 1 or 2
        unsigned sampling_rate; // (offset 24) = <anything>
        unsigned bytes_per_second; // (offset 28) = <ignore>
        unsigned short bytes_per_sample; // (offset 32) = <ignore>
        unsigned short bits_per_sample; // (offset 34) = 16
        char data_label[4]; // (offset 36) = {‘d’, ‘a’, ‘t’, ‘a’}
        unsigned data_size; // (offset 40) = <# of bytes of data>
    };

    WaveHeader m_FileHeader;
    std::vector<char> m_Data;
    std::fstream m_File;

public:
    AJWavFormat();
    ~AJWavFormat();

    void Init(std::wstring filePath, unsigned short channel_count, unsigned sampling_rate, unsigned short bits_per_sample);
    void WriteBuffer(char* buffer, unsigned size);
    void Close();
};

#endif