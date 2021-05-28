#pragma once

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable : 4583)
    #pragma warning(disable : 4582)
#endif
#include "ultraleap/haptics/result.hpp"
#ifdef _MSC_VER
    #pragma warning(pop)
#endif

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/vector3.hpp"
#include "ultraleap/haptics/matrix4x4.hpp"
#include "ultraleap/haptics/stl_compat.hpp"

#include <vector>
#include <fstream>
#include <limits>
#include <algorithm>

namespace Ultraleap
{
namespace Haptics
{

/** \brief Data on individual transducers.
 *
 * Individual transducer data: position, direction and phase_offset. */
struct Transducer
{
    /** Default constructor for a disabled transducer at the origin. */
    Transducer()
        : position()
        , normal()
        , phase_offset(0.f)
        , enabled(false)
    {}

    /** Create a Transducer with the given position, direction and phase multiplier.
     *
     * @param xp x position of this transducer in metres
     * @param yp y position of this transducer in metres
     * @param zp z position of this transducer in metres
     * @param xu x component of the normalised emission direction vector of this transducer
     * @param yu y component of the normalised emission direction vector of this transducer
     * @param zu z component of the normalised emission direction vector of this transducer
     * @param p Phase offset of this transducer
     * @param e True if this transducer is enabled, false if not */
    Transducer(float xp, float yp, float zp, float xu, float yu, float zu, float p = 0.0f, bool e = true)
        : position(xp, yp, zp)
        , normal(xu, yu, zu)
        , phase_offset(p)
        , enabled(e)
    {}

    /** Create a Transducer with the given position, direction and phase multiplier.
     *
     * @param pos position of this transducer in metres
     * @param norm normalised emission direction vector of this transducer
     * @param p Phase offset of this transducer
     * @param e True if this transducer is enabled, false if not */
    Transducer(Vector3 pos, Vector3 norm, float p = 0.0f, bool e = true)
        : position(pos)
        , normal(norm)
        , phase_offset(p)
        , enabled(e)
    {}

    /** Position of this transducer, in metres */
    Vector3 position;
    /** Normal (upvector) of this transducer */
    Vector3 normal;
    /** Phase offset of this transducer */
    float phase_offset;
    /** Whether or not this transducer is enabled */
    bool enabled;

    /** Determine if this transducer is enabled */
    bool isEnabled() const
    {
        return enabled;
    }

    std::string toString() const
    {
        return std::to_string(position.x) + "," + std::to_string(position.y) + "," + std::to_string(position.z) + "," + std::to_string(normal.x) + "," + std::to_string(normal.y) + "," + std::to_string(normal.z) + "," + std::to_string(phase_offset);
    }
};

/** Container for holding physical transducer data. */
class TransducerContainer : public std::vector<Transducer>
{
public:
    /** Construct a transducer container containing zero transducers */
    TransducerContainer()
        : vector()
    {
    }

    /** Construct a transducer container with the specified number of default-constructed transducers
     *
     * @param n the number of transducers this container should be initialised with */
    TransducerContainer(size_t n)
        : vector(n)
    {
    }

    /** Construct a transducer container containing a set of transducers
     *
     * @param idata The array of physical transducer data
     * @param n The number of transducers to copy from the array */
    TransducerContainer(const Transducer* idata, size_t n)
        : vector(idata, idata + n)
    {
    }

    /** Copy constructor
     *
     * @param other The other transducer container to copy from */
    TransducerContainer(const TransducerContainer& other)
        : vector(other)
    {
    }

    /** Move constructor
     *
     * @param other The other transducer container to move from */
    TransducerContainer(TransducerContainer&& other)
        : vector(std::move(other))
    {
    }

    /** Copy assignment operator
     *
     * @param other The other transducer container to copy from
     * @return Itself */
    TransducerContainer& operator=(const TransducerContainer& other)
    {
        std::vector<Transducer>::operator=(other);
        return *this;
    }

    /** Move assignment operator
     *
     * @param other The other transducer container to move from
     * @return Itself */
    TransducerContainer& operator=(TransducerContainer&& other)
    {
        std::vector<Transducer>::operator=(std::move(other));
        return *this;
    }

    /** @return The width (X-axis span) of the transducers in this container */
    float width() const
    {
        if (size() == 0)
            return 0.0f;
        float x_minvalue = std::numeric_limits<float>::max();
        float x_maxvalue = std::numeric_limits<float>::min();
        for (size_t i = 0; i < size(); i++) {
            const float x_p = at(i).position.x;
            x_minvalue = std::min(x_p, x_minvalue);
            x_maxvalue = std::max(x_p, x_maxvalue);
        }
        return x_maxvalue - x_minvalue;
    }

    /** @return The depth (Y-axis span) of the transducers in this container */
    float depth() const
    {
        if (size() == 0)
            return 0.0f;
        float y_minvalue = std::numeric_limits<float>::max();
        float y_maxvalue = std::numeric_limits<float>::min();
        for (size_t i = 0; i < size(); i++) {
            const float y_p = at(i).position.y;
            y_minvalue = std::min(y_p, y_minvalue);
            y_maxvalue = std::max(y_p, y_maxvalue);
        }
        return y_maxvalue - y_minvalue;
    }

    /** Create a new container with the given transformation
     * applied to the physical transducer data
     *
     * @param transform The transformation to apply
     * @return New physical transducer data with transform applied */
    TransducerContainer operator*(const Matrix4x4& transform) const
    {
        TransducerContainer t = *this;
        t *= transform;
        return std::move(t);
    }

    /** Apply transformation to physical transducer data in this container
     *
     * @param transform The transformation to apply
     * @return Itself */
    TransducerContainer& operator*=(const Matrix4x4& transform)
    {
        for (Transducer& t : *this) {
            const float x_position_input = t.position.x;
            const float y_position_input = t.position.y;
            const float z_position_input = t.position.z;
            t.position.x = x_position_input * transform.element[0] + y_position_input * transform.element[1] + z_position_input * transform.element[2] + transform.element[3];
            t.position.y = x_position_input * transform.element[4] + y_position_input * transform.element[5] + z_position_input * transform.element[6] + transform.element[7];
            t.position.z = x_position_input * transform.element[8] + y_position_input * transform.element[9] + z_position_input * transform.element[10] + transform.element[11];

            const float x_direction_input = t.normal.x;
            const float y_direction_input = t.normal.y;
            const float z_direction_input = t.normal.z;
            t.normal.x = x_direction_input * transform.element[0] + y_direction_input * transform.element[1] + z_direction_input * transform.element[2];
            t.normal.y = x_direction_input * transform.element[4] + y_direction_input * transform.element[5] + z_direction_input * transform.element[6];
            t.normal.z = x_direction_input * transform.element[8] + y_direction_input * transform.element[9] + z_direction_input * transform.element[10];
        }
        return *this;
    }

    /** Load a transducer container from the specified file
     *
     * @param filename the path to load a transducer container from
     * @return a transducer container parse for the file filename */
    static result<TransducerContainer> load(const char* filename)
    {
        // read transducer CSV into vector of comma-separated values per line
        std::vector<std::vector<std::string>> tx_strings;
        {
            std::ifstream infile(filename);
            if (!infile.is_open())
                return make_unexpected(ErrorCode::InvalidArgument);
            std::string tx_str;
            while (std::getline(infile, tx_str)) {
                if (!tx_str.empty())
                    tx_strings.push_back(uh::split<','>(tx_str));
            }
        }

        // find the highest index mentioned in the file
        size_t max_index = 0;
        for (size_t row = 0; row < tx_strings.size(); ++row) {
            try {
                size_t index = std::stoul(tx_strings[row].at(0));
                max_index = (index > max_index) ? index : max_index;
            } catch (const std::exception&) {
                return make_unexpected(ErrorCode::InvalidArgument);
            }
        }

        TransducerContainer tx_container(max_index + 1);

        const size_t floats_per_line = 7;
        std::vector<float> tx;
        tx.reserve(floats_per_line);
        for (size_t row = 0; row < tx_strings.size(); ++row) {
            size_t index;
            try {
                index = std::stoul(tx_strings[row].at(0));
            } catch (const std::exception&) {
                return make_unexpected(ErrorCode::InvalidArgument);
            }

            tx.clear();
            for (size_t col = 1; col < tx_strings[row].size(); ++col) {
                try {
                    tx.push_back(std::stof(tx_strings[row][col]));
                } catch (const std::exception&) {
                    return make_unexpected(ErrorCode::InvalidArgument);
                }
            }

            if (tx.size() != floats_per_line)
                return make_unexpected(ErrorCode::InvalidArgument);

            tx_container[index] = Transducer(tx[0], tx[1], tx[2], tx[3], tx[4], tx[5], tx[6]);
        }
        return tx_container;
    }

    /** Save this transducer container to the specified file
     *
     * @param filename the path to save this transducer container to */
    result<void> save(const char* filename)
    {
        std::ofstream outfile(filename);
        if (outfile.good()) {
            size_t index = 0;
            for (Transducer& t : *this) {
                outfile << index << "," << t.toString() << "\n";
                index++;
            }
            if (outfile.good())
                return {};
        }
        return make_unexpected(ErrorCode::FileAccessError);
    }
};

} // namespace Haptics

} // namespace Ultraleap