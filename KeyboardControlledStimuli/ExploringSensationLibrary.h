#pragma once
#include <list>

#include <ultraleap/haptics/sensations.hpp>

namespace RandomWalk::Sensations
{
    enum class KnownSensation
    {
        CircleWithFixedSpeed,
        CircleWithFixedFrequency,
        Line,
        Dial,
        RotorWithFixedSpeed,
        RotorWithFixedFrequency,
        Scan,
        OpenCloseWithFixedSpeed,
        OpenCloseWithFixedFrequency,
        Lissajous,
        Ripple,
        AmplitudeModulatedPoint,
        HandCircleWithFixedSpeed,
        HandCircleWithFixedFrequency,
        HandLissajous,
        HandDial,
        HandRotorWithFixedSpeed,
        HandRotorWithFixedFrequency,
        HandScan,
        HandOpenCloseWithFixedSpeed,
        HandOpenCloseWithFixedFrequency,
        HandLightning,
        HandRipple,
    };

    std::list<KnownSensation> allKnownSensations();
    std::string knownSensationString(KnownSensation sensation);

    class LibrarySensation
    {
    public:
        LibrarySensation(Ultraleap::Haptics::Library *lib, Ultraleap::Haptics::SensationEmitter *emitter, std::string package = "StandardSensations.ssp")
            : lib{lib}, emitter{emitter}, sensation_package{loadSensationPackage(package)} {};
        Ultraleap::Haptics::Library *lib;
        Ultraleap::Haptics::SensationEmitter *emitter;
        Ultraleap::Haptics::SensationInstance *sensation_instance;

        Ultraleap::Haptics::result<Ultraleap::Haptics::SensationPackage> loadSensationPackage(std::string package);
        int start(KnownSensation sensation = KnownSensation::CircleWithFixedSpeed);
        int stop();
        int sensationLoop();

    private:
        Ultraleap::Haptics::result<Ultraleap::Haptics::SensationPackage> sensation_package;
        Ultraleap::Haptics::Transform tracking_transform;
        KnownSensation current_sensation;
    };
}