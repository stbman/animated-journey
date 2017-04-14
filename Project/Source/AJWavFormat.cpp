//--------------------------------------------------------------------------------------
// Author: Adrian Lim Xuan Wei
// Supervising: Prof Terence Sim
// Title : 
// MComp Thesis for National University of Singapore
//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------

#include "../Source/AJWavFormat.h"

AJWavFormat::AJWavFormat()
{
}

AJWavFormat::~AJWavFormat()
{
}

void AJWavFormat::Init(std::wstring filePath, unsigned short channel_count, unsigned sampling_rate, unsigned short bits_per_sample)
{
    m_Data.clear();

    m_File.open(filePath.c_str(), std::ios::in | std::ios::out | std::ios::trunc | std::ios::binary);

    if (!m_File.is_open())
    {
        return;
    }

    // Write the header
    // Put #1, 1, "RIFF"        ' RIFF marker
    // Put #1, 5, CInt(0)       ' file-size (equals file-size - 8)
    // Put #1, 9, "WAVE"        ' Mark it as type "WAVE"
    // Put #1, 13, "fmt "        ' Mark the format section.
    // Put #1, 17, CLng(16)      ' Length of format data.  Always 16
    // Put #1, 21, CInt(1)       ' Wave type PCM
    // Put #1, 23, CInt(2)       ' 2 channels
    // Put #1, 25, CLng(44100)   ' 44.1 kHz Sample Rate (CD-Quality)
    // Put #1, 29, CLng(88200)   ' (Sample Rate * Bit Size * Channels) / 8
    // Put #1, 33, CInt(2)       ' (Bit Size * Channels) / 8
    // Put #1, 35, CInt(16)      ' Bits per sample (=Bit Size * Samples)
    // Put #1, 37, "data"        ' "data" marker
    // Put #1, 41, CInt(0)       ' data-size (equals file-size - 44).

    memcpy(m_FileHeader.riff_label, "RIFF", 4);
    m_FileHeader.riff_size = 0;
    memcpy(m_FileHeader.file_tag, "WAVE", 4);
    memcpy(m_FileHeader.fmt_label, "fmt ", 4);
    m_FileHeader.fmt_size = 16;
    m_FileHeader.audio_format = 1;
    m_FileHeader.channel_count = channel_count;
    m_FileHeader.sampling_rate = sampling_rate;
    m_FileHeader.bytes_per_second = (sampling_rate * bits_per_sample * channel_count) / 8;
    m_FileHeader.bytes_per_sample = (bits_per_sample * channel_count) / 8;
    m_FileHeader.bits_per_sample = bits_per_sample;
    memcpy(m_FileHeader.data_label, "data", 4);
    m_FileHeader.data_size = 0;
}

void AJWavFormat::WriteBuffer(char* buffer, unsigned size)
{
    //cout << "EPGNUIN PENGUIN"

    for (unsigned i = 0; i < size; ++i)
    {
        m_Data.push_back(buffer[i]);
    }

    m_FileHeader.data_size = (unsigned) m_Data.size();
}

void AJWavFormat::Close()
{
    m_File.write((char*)&m_FileHeader, sizeof(WaveHeader));
    m_File.write(&m_Data[0], m_Data.size());

    m_File.close();

    m_Data.clear();
}