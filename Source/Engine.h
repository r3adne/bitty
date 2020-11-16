/*
  ==============================================================================

    Engine.h
    Created: 10 Nov 2020 1:19:42am
    Author:  Zachary Lewis-Towbes

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <array>
#include <memory>
#include <cmath>


class BitmaskerEngine
{
public:
    BitmaskerEngine()
    {
#if N_BITS == 8
        andmask.store(std::bitset<8>("11111111"));
        ormask.store(std::bitset<8>("00000000"));
        xormask.store(std::bitset<8>("00000000"));
        bitremap.store({0, 1, 2, 3, 4, 5, 6, 7});
#elif N_BITS == 16
        andmask.store(std::bitset<16>("1111111111111111"));
        ormask.store(std::bitset<16>("0000000000000000"));
        xormask.store(std::bitset<16>("0000000000000000"));
        bitremap.store({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
#endif



        removedenormals.store(false);
    }
    ~BitmaskerEngine() { }

    std::atomic<std::array<uint8, N_BITS>> bitremap;
//    std::atomic<uint8> andmask, ormask, xormask;
    std::atomic<std::bitset<N_BITS>> andmask, ormask, xormask;
    std::atomic<bool> removedenormals;

public:

//    void processSamplesContextReplacing(float* data, size_t size)
//    {
//
//    }

    void processSamplesContextReplacing(AudioBuffer<float>& a)
    {
        ScopedNoDenormals nodenormals;

#if N_BITS == 8
        AudioBuffer<char> convertedBuffer(a.getNumChannels(), a.getNumSamples());
#elif N_BITS == 16
        AudioBuffer<char16_t> convertedBuffer(a.getNumChannels(), a.getNumSamples());
#endif


        for (int chan = 0; chan < a.getNumChannels(); ++chan)
        {
            AudioData::Pointer<AudioData::Float32,
                                AudioData::LittleEndian,
                                AudioData::NonInterleaved,
                                AudioData::Const> src(a.getReadPointer(chan));

            AudioData::Pointer<
#if N_BITS == 8
                                AudioData::Int8,
#elif N_BITS == 16
                                AudioData::Int16,
#endif
                                AudioData::LittleEndian,
                                AudioData::NonInterleaved,
                                AudioData::NonConst> dst(convertedBuffer.getWritePointer(chan));


            dst.convertSamples(src, a.getNumSamples());
        }


        // at some point we will make this simd, obviously.
        for (int chan = 0; chan < convertedBuffer.getNumChannels(); ++chan)
        {
            for (int i = 0; i < convertedBuffer.getNumSamples(); ++i)
            {
#if DEBUG
                [[maybe_unused]] float floatval = a.getSample(chan, i); // orig float value
    #if N_BITS == 8
                [[maybe_unused]] int8 intval = convertedBuffer.getSample(chan, i);
    #elif N_BITS == 16
                [[maybe_unused]] int16 intval = convertedBuffer.getSample(chan, i);
    #endif
#endif

                std::bitset<N_BITS> c(convertedBuffer.getSample(chan, i));
                std::bitset<N_BITS> d;

                for (uint8 idx = 0; idx < N_BITS; ++idx) { d.set(bitremap.load()[idx], c[idx]); }

                d &= andmask.load();
                d |= ormask.load();
                d ^= xormask.load();

#if N_BITS == 8
                convertedBuffer.setSample(chan, i, d.to_ulong());
#elif N_BITS == 16
                convertedBuffer.setSample(chan, i, d.to_ulong());
#endif
            }
        }

        for (int chan = 0; chan < a.getNumChannels(); ++chan)
        {
            AudioData::Pointer<AudioData::Float32,
                                AudioData::LittleEndian,
                                AudioData::NonInterleaved,
                                AudioData::NonConst> dst(a.getWritePointer(chan));

            AudioData::Pointer<
#if N_BITS == 8
                                AudioData::Int8,
#elif N_BITS == 16
                                AudioData::Int16,
#endif
                                AudioData::LittleEndian,
                                AudioData::NonInterleaved,
                                AudioData::Const> src(convertedBuffer.getReadPointer(chan));


            dst.convertSamples(src, a.getNumSamples());
        }
    }

    void setandmask(String newandmask) { andmask.store(std::bitset<N_BITS>(newandmask.toStdString())); }
    void setormask(String newormask) { ormask.store(std::bitset<N_BITS>(newormask.toStdString())); }
    void setxormask(String newxormask) { xormask.store(std::bitset<N_BITS>(newxormask.toStdString())); }

    void setBitRemapBit(uint8 bitToSet, uint8 valueToSet)
    {
        assert(bitToSet < N_BITS);
        assert(valueToSet < N_BITS);

        bitremap.load()[bitToSet] = valueToSet;
    }

    void setEntireBitRemap(std::array<uint8, N_BITS> newBitRemap)
    {
        bitremap.store(newBitRemap);
    }


    String getandmask() { return String(andmask.load().to_string()); }
    String getormask() { return String(ormask.load().to_string()); }
    String getxormask() { return String(xormask.load().to_string()); }
    std::array<uint8, N_BITS> getbitremap() { return bitremap.load(); }

};
