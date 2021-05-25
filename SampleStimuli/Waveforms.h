#pragma once

#include <chrono>

#include <Ultrahaptics.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323
#endif

using Seconds = std::chrono::duration<float>;

namespace Waveforms {

    struct Waveform {
    public:
        Waveform() {
            init();
        }
        virtual float fn(float x) = 0;
        const float mmnorm(float x) {
            if (!normalize) { return x; }
            return (x - min) / (max - min);
        }
        const float evaluateAt(Seconds t) {
            return mmnorm(fn(t.count()));
        };

    protected:
        void setMin(float m) { min = m; }
        void setMax(float m) { max = m; }
        void setNormalize(bool b) { normalize = b; }
        virtual void init() { }
    private:
        float min{ 0 };
        float max{ 1 };
        bool normalize{ false };
    };

    struct Sine : public Waveform {
    public:
        Sine () : Waveform () {}
        Sine(float factor) {
            init();
            setFactor(factor);
        }
        float fn(float x) {
            return std::sin(factor * x);
        }
        void setFactor(float f) {
            factor = f;
        }
    protected:
        float factor{ 1 };
        void init() override {
            setMin(-1);
            setNormalize(true);
        }
    };

    struct SkewedSine : public Sine {
    public:
        SkewedSine() : Sine() {
            /*setMin(-1.557);
            setMax(1.557);*/
        }
        float fn(float x) override {
            float _x = factor * x;
            return -(1 / t) * std::atan((t * std::sin(_x)) / (1 - t * std::cos(_x)));
        }
        void setSkew(float skew) {
            t = skew;
        }
    protected:
        float t{ 0.5f };
    };

    struct Sawtooth : public SkewedSine {
    protected:
        float t() { return 0.999f; }
    };


    struct Square : public Sine {
    public:
        float fn(float x) override {
            float ret = Sine::fn(x) >= 0 ? 1 : -1;
            return ret;
        }
    };

    struct Triangle : public Sine {
    public:
        float fn(float x) override {
            float sine = Sine::fn(M_PI * x);
            return 2 / M_PI * std::asin(sine);
        }
    };

    struct Fixed : public Waveform {
    public:
        float fn(float x) override {
            return fixed;
        }
        void setFixed(float x) {
            fixed = x;
        }
    protected:
        float fixed{ 1 };
        void init() override {
            setNormalize(true);
        }
    };

    struct One : public Fixed {
    protected:
        void init() override {
            setNormalize(true);
            setFixed(1);
        }
    };

}