#ifndef _WAVEFORM_DATA_H
#define _WAVEFORM_DATA_H

#include <cmath>
#include <cstdio>
#include <cstring>
#include <vector>
#include <stdio.h>

// Utility class to handle Waveform data from a WAV file. Also supports
// resampling to different sample rates.
class WaveformData
{
public:
    WaveformData() : sample_rate(0), bWaveformLoaded(false)
    {}

    WaveformData(const char* filename);
    WaveformData(const float sample_rate, const char* filename);

    void readRiffWave(const char* filename);
    void writeRiffWave(const char* filename);

    inline WaveformData operator+(const float rhs) const;
    inline WaveformData operator*(const float rhs) const;

    void resample(const unsigned int target_sample_rate, const size_t fft_size = 4096);

    bool isWaveformLoaded() { return bWaveformLoaded; }

    const float sampleRate() const
    {
        return sample_rate;
    }

    const float at(const size_t idx) const
    {
        return waveform.at(idx % waveform.size());
    }

    const size_t size() const
    {
        return waveform.size();
    }

private:
    float sample_rate;
    std::vector<float> waveform;
    bool bWaveformLoaded;
};

// Helper function, not part of the WaveformData class
void FFT_C2C(float* complex_data, float* cache, const int64_t length, const int64_t dir);

WaveformData::WaveformData(const char* filename)
{
    readRiffWave(filename);
}

WaveformData::WaveformData(const float sample_rate, const char* filename) :
    sample_rate(sample_rate)
{
    readRiffWave(filename);
}

void WaveformData::readRiffWave(const char* filename)
{
    // Clear any previously stored data
    waveform.clear();

    FILE *FP;
    fopen_s(&FP, filename, "rb");
    if (FP == nullptr)
    {
        std::printf("\'%s\' not found.\n", filename);
        return;
    }

    // Determine if this is a RIFF file
    char buffer[5];
    std::fread(buffer, 4, 1, FP);
    buffer[4] = '\0';
    if (std::strcmp(buffer, "RIFF") != 0)
    {
        std::printf("not a RIFF file\n");
        return;
    }

    // Determine if this is a WAVE file
    uint32_t chunksize;
    std::fread(&chunksize, 4, 1, FP);
    std::fread(buffer, 4, 1, FP);
    buffer[4] = '\0';
    if (std::strcmp(buffer, "WAVE") != 0)
    {
        std::printf("not a WAVE file\n");
        return;
    }

    std::fread(buffer, 4, 1, FP);
    buffer[4] = '\0';
    // Get the data format subchunk
    if (std::strcmp(buffer, "fmt ") != 0)
    {
        std::printf("not the fmt subchunk\n");
        return;
    }

    uint32_t subchunk1_size;
    uint16_t audioformat;
    std::fread(&subchunk1_size, 4, 1, FP);
    std::fread(&audioformat, 2, 1, FP);

    // Ensure this is in PCM format
    if (audioformat != 1)
    {
        std::printf("not in PCM format\n");
        return;
    }

    uint16_t num_channels;
    uint32_t input_sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    std::fread(&num_channels, 2, 1, FP);
    std::fread(&input_sample_rate, 4, 1, FP);
    std::fread(&byte_rate, 4, 1, FP);
    std::fread(&block_align, 2, 1, FP);
    std::fread(&bits_per_sample, 2, 1, FP);
    std::fread(buffer, 4, 1, FP);
    buffer[4] = '\0';

    // Ensure this is the data subchunk
    if (std::strcmp(buffer, "data") != 0)
    {
        std::printf("not the data subchunk\n");
        return;
    }

    // Read SubChunk2
    uint32_t subchunk2_size;
    std::fread(&subchunk2_size, 4, 1, FP);

    // Loop through the channels as we are reading the file
    while (!std::feof(FP))
    {
        for (size_t i = 0; i < num_channels; i++)
        {
            union
            {
                uint8_t u8[4];
                int16_t i16[2];
                uint32_t u32;
            } sample;

            // Read next sample
            fread(sample.u8, bits_per_sample / 8, 1, FP);
            // If we are on the first channel (only using the first channel)
            if (i == 0)
            {
                float this_sample = 0;
                switch (bits_per_sample)
                {
                case 8:
                    this_sample = ((float)(sample.u8[0] - 0x80)) / ((float)0x7F);
                    break;
                case 16:
                    this_sample = ((float)(sample.i16[0])) / ((float)0x7FFF);
                    break;
                case 32:
                    this_sample = ((float)(sample.u32 - 0x80000000)) / ((float)0x7FFFFFFF);
                    break;
                default:
                    break;
                }
                // Push onto waveform stack
                waveform.push_back(this_sample);
            }
            // If we have an unexpected end-of-file, break out of this loop.
            if (std::feof(FP)) break;
        }
    }
    // Close wave file
    std::fclose(FP);
    // Set sample rate to that in the file
    sample_rate = input_sample_rate;

    bWaveformLoaded = true;
}

// Save wave in RIFF format
void WaveformData::writeRiffWave(const char* filename)
{
    FILE* FP;
    fopen_s(&FP, filename, "wb");
    if (FP == nullptr)
    {
        std::printf("Faile to open \'%s\'.\n", filename);
        return;
    }

    // Write header
    const uint32_t datasize = 36 + sizeof(uint16_t) * size();
    std::fprintf(FP, "RIFF");
    std::fwrite(&datasize, sizeof(uint32_t), 1, FP);

    // Write sub chunk 1
    const uint32_t subchunk1_size = 16;
    const uint16_t pcm_format = 1;
    const uint16_t num_channels = 1;
    const uint32_t output_sample_rate = sample_rate;
    const uint32_t output_byte_rate = sample_rate * sizeof(uint16_t);
    const uint16_t block_align = sizeof(uint16_t);
    const uint16_t bits_per_sample = sizeof(uint16_t) * 8;

    std::fprintf(FP, "WAVE");
    std::fprintf(FP, "fmt ");
    std::fwrite(&subchunk1_size, sizeof(uint32_t), 1, FP);
    std::fwrite(&pcm_format, sizeof(uint16_t), 1, FP);
    std::fwrite(&num_channels, sizeof(uint16_t), 1, FP);
    std::fwrite(&output_sample_rate, sizeof(uint32_t), 1, FP);
    std::fwrite(&output_byte_rate, sizeof(uint32_t), 1, FP);
    std::fwrite(&block_align, sizeof(uint16_t), 1, FP);
    std::fwrite(&bits_per_sample, sizeof(uint16_t), 1, FP);

    // Write sub chunk 2
    const uint32_t subchunk2_size = sizeof(uint16_t) * size();
    std::fprintf(FP, "data");
    std::fwrite(&subchunk2_size, sizeof(uint32_t), 1, FP);

    if (waveform.size() > 0)
    {
        for (size_t i = 0; i < waveform.size(); i++)
        {
            float value_f = (0x7FFF * waveform.at(i));
            value_f = (value_f > 0x7FFF) ? 0x7FFF : value_f;
            value_f = (value_f < -0x7FFF) ? -0x7FFF : value_f;
            uint16_t value = static_cast<uint16_t>(value_f);
            std::fwrite(&value, sizeof(uint16_t), 1, FP);
        }
    }

    // Close wave file
    std::fclose(FP);
}

WaveformData WaveformData::operator+(const float rhs) const
{
    WaveformData newdata(*this);
    for (std::vector<float>::iterator it = newdata.waveform.begin(); it != newdata.waveform.end(); ++it)
    {
        (*it) += rhs;
    }
    return newdata;
}

WaveformData WaveformData::operator*(const float rhs) const
{
    WaveformData newdata(*this);
    for (std::vector<float>::iterator it = newdata.waveform.begin(); it != newdata.waveform.end(); ++it)
    {
        (*it) *= rhs;
    }
    return newdata;
}

// Resample the waveform data to an abitary sample length.
void WaveformData::resample(const unsigned int target_sample_rate, const size_t fft_size)
{
    if (!bWaveformLoaded || (target_sample_rate == sample_rate))
        return;

    std::vector<float> resampledWaveform;

    size_t real_fft_size = fft_size;
    while (real_fft_size >= waveform.size())
    {
        real_fft_size >>= 1;
    }

    if (real_fft_size <= 2)
    {
        return;
    }

    const double frequency_conversion_ratio = ((double)target_sample_rate) / ((double)sample_rate);
    const double binary_log_sampling_rate_change = std::log2((double)frequency_conversion_ratio);
    const double output_size_ratio = std::exp2(std::ceil(binary_log_sampling_rate_change));
    const unsigned int output_fft_size = real_fft_size * output_size_ratio;
    const double output_freq_conversion_ratio = ((double)target_sample_rate) / ((double)(sample_rate * output_size_ratio));

    float* sample_buffer = (float*)std::calloc(real_fft_size, sizeof(float));
    float* fft_input_buffer = (float*)std::calloc(real_fft_size * 2, sizeof(float));
    float* fft_output_buffer = (float*)std::calloc(output_fft_size * 2, sizeof(float));
    float* fft_cache = (float*)std::calloc(((output_fft_size > real_fft_size) ? output_fft_size : real_fft_size) * 2, sizeof(float));

    const double reciprocal_fft_size_m1 = 1.f / ((double)(real_fft_size - 1));
    const double reciprocal_output_fft_size_m1 = 1.f / ((double)(output_fft_size - 1));
    const double reciprocal_fft_size = 1.f / ((double)(real_fft_size));

    std::vector<float> input_waveform;
    std::vector<float> combined_weighting;
    size_t sample_count = 0;
    size_t fftd_sample_count = 0;

    for (std::vector<float>::const_iterator it = waveform.begin(); it != waveform.end(); ++it)
    {
        size_t index_to_fill = (sample_count % (real_fft_size / 2)) + (real_fft_size / 2);
        sample_buffer[index_to_fill] = *it;
        if ((index_to_fill == (real_fft_size - 1)) || (sample_count == (waveform.size() - 1)))
        {
            for (size_t q = (index_to_fill + 1); q < real_fft_size; q++)
            {
                sample_buffer[q] = 0.;
            }

            // Put sample data into FFT with Blackman window
            for (size_t q = 0; q < real_fft_size; q++)
            {
                const float blackman_sample = 0.5 * (1. - 0.16)
                    + -0.5 * cos(2. * M_PI * ((float)q) * reciprocal_fft_size_m1)
                    + 0.5 * 0.16 * cos(4. * M_PI * ((float)q) * reciprocal_fft_size_m1);
                fft_input_buffer[q * 2 + 0] = sample_buffer[q] * blackman_sample;
                fft_input_buffer[q * 2 + 1] = 0.f;
            }

            FFT_C2C(fft_input_buffer, fft_cache, real_fft_size, 1);

            for (unsigned int q = 0; q <= (output_fft_size / 2); q++)
            {
                if (q <= (real_fft_size / 2))
                {
                    fft_output_buffer[q * 2 + 0] = fft_input_buffer[q * 2 + 0];
                    fft_output_buffer[q * 2 + 1] = fft_input_buffer[q * 2 + 1];

                    if (q > 0)
                    {
                        fft_output_buffer[(output_fft_size - q) * 2 + 0] = fft_input_buffer[(real_fft_size - q) * 2 + 0];
                        fft_output_buffer[(output_fft_size - q) * 2 + 1] = fft_input_buffer[(real_fft_size - q) * 2 + 1];
                    }
                }
                else
                {
                    fft_output_buffer[q * 2 + 0] = 0.;
                    fft_output_buffer[q * 2 + 1] = 0.;

                    if (q > 0)
                    {
                        fft_output_buffer[(output_fft_size - q) * 2 + 0] = 0.;
                        fft_output_buffer[(output_fft_size - q) * 2 + 1] = 0.;
                    }
                }
            }

            FFT_C2C(fft_output_buffer, fft_cache, output_fft_size, -1);

            const size_t current_vector_size = input_waveform.size();

            fftd_sample_count += ((output_fft_size / 2) - 1);
            for (size_t q = 0; q < output_fft_size; q++)
            {
                const float blackman_sample = 0.5 * (1. - 0.16)
                    + -0.5 * cos(2. * M_PI * ((float)q) * reciprocal_output_fft_size_m1)
                    + 0.5 * 0.16 * cos(4. * M_PI * ((float)q) * reciprocal_output_fft_size_m1);

                const int sample_buffer_position = fftd_sample_count + q - ((output_fft_size / 2) - 1);

                if (sample_buffer_position >= current_vector_size)
                {
                    input_waveform.push_back(fft_output_buffer[q * 2 + 0] * reciprocal_fft_size);
                    combined_weighting.push_back(blackman_sample);
                }
                else
                {
                    input_waveform.at(sample_buffer_position) += (fft_output_buffer[q * 2 + 0] * reciprocal_fft_size);
                    combined_weighting.at(sample_buffer_position) += blackman_sample;
                }
            }
            fftd_sample_count++;

            for (size_t q = 0; q < (real_fft_size / 2); q++)
            {
                sample_buffer[q] = sample_buffer[q + (real_fft_size / 2)];
            }
        }
        sample_count++;
    }

    double value = 0, lastvalue = 0;

    for (size_t i = (output_fft_size / 2); i < (combined_weighting.size() - (output_fft_size / 2)); i++)
    {
        value += output_freq_conversion_ratio;

        if (((size_t)value) != ((size_t)lastvalue))
        {
            const float delta_m_value = floor(value) - lastvalue;
            const float l_value = delta_m_value / output_freq_conversion_ratio;
            const float sample_value =
                (1.0f - l_value) * (input_waveform.at(i - 1) / combined_weighting.at(i - 1)) +
                l_value * (input_waveform.at(i) / combined_weighting.at(i));
            resampledWaveform.push_back(sample_value);
        }
        lastvalue = value;
    }

    sample_rate = target_sample_rate;
    waveform = resampledWaveform;

    std::free(fft_cache);
    std::free(fft_output_buffer);
    std::free(fft_input_buffer);
    std::free(sample_buffer);
}

// Complex-to-complex FFT implementation
void FFT_C2C(float* complex_data, float* cache, const int64_t length, const int64_t dir)
{
    // Copy data into working space, applying the bit reversal transformation in the process
    int log2val = log2(length);
    for (uint32_t i = 0; i < length; i++)
    {
        // Byte swap
        uint32_t bval_1 =
            (i << 24) | ((i << 8) & 0x00FF0000) | ((i >> 8) & 0x0000FF00) | (i >> 24);
        bval_1 = ((bval_1 & 0xaaaaaaaa) >> 1) | ((bval_1 & 0x55555555) << 1);
        bval_1 = ((bval_1 & 0xcccccccc) >> 2) | ((bval_1 & 0x33333333) << 2);
        bval_1 = ((bval_1 & 0xf0f0f0f0) >> 4) | ((bval_1 & 0x0f0f0f0f) << 4);
        bval_1 >>= (32 - log2val);

        // Apply bit reversal to input
        const float v_real = complex_data[(i << 1) + 0];
        const float v_imag = complex_data[(i << 1) + 1] * ((float)dir);
        cache[(bval_1 << 1) + 0] = v_real;
        cache[(bval_1 << 1) + 1] = v_imag;
    }

    // Do simple radix-2 FFT butterflies until we have finished
    for (int64_t k = 0; k < log2val; k++)
    {
        for (int64_t i = 0; i < (length / 2); i++)
        {
            const float angle = (i & ((1 << k) - 1)) * M_PI * exp2(-((float)k));
            const float cos_part = cos(angle);
            const float sin_part = sin(angle);
            const size_t aj = (i & ~((1 << k) - 1));
            const size_t ai = (i & ((1 << k) - 1));
            const size_t lo_idx = (aj << 1) + ai;
            const size_t hi_idx = (aj << 1) + (1LL << k) + ai;
            const float lo_real = cache[(lo_idx << 1) + 0];
            const float lo_imag = cache[(lo_idx << 1) + 1];
            const float hi_real = cache[(hi_idx << 1) + 0];
            const float hi_imag = cache[(hi_idx << 1) + 1];
            const float temp_r = hi_real * cos_part + hi_imag * sin_part;
            const float temp_i = hi_imag * cos_part - hi_real * sin_part;
            cache[(hi_idx << 1) + 0] = lo_real - temp_r;
            cache[(hi_idx << 1) + 1] = lo_imag - temp_i;
            cache[(lo_idx << 1) + 0] = lo_real + temp_r;
            cache[(lo_idx << 1) + 1] = lo_imag + temp_i;
        }
    }

    // Copy data from working space, inverting the bit reversal transformation in the process
    for (uint32_t i = 0; i < length; i++)
    {
        // Copy back, applying bit reversal to output
        const float v_real = cache[(i << 1) + 0];
        const float v_imag = cache[(i << 1) + 1];
        complex_data[(i << 1) + 0] = v_real;
        complex_data[(i << 1) + 1] = v_imag * ((float)dir);
    }
}

#endif //_WAVEFORM_DATA_H
