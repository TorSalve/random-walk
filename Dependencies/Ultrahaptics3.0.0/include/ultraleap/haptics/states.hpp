#pragma once

#include "ultraleap/haptics/common.hpp"

#include <cstdint>
#include <cstdlib>

#if !defined(NO_CXX_COMPLEX_AVAILABLE)
    #include <complex>
#endif // !defined(NO_CXX_COMPLEX_AVAILABLE)

namespace Ultraleap
{
namespace Haptics
{
/** \brief Transmission coefficients (phase & amplitude) for all transducers.
 *
 * This class stores the phase and amplitude coefficients as a complex number for each
 * transducer in an array. Internally it is equivalent to `std::vector<std::complex<float>>`.
 *
 * By streaming this state to the device you can arbitrarily control the activation of the
 * transducers.
 *
 * See \rstref{LowLevelAccess.rst} for more information on
 * state emitters.
 */
class ULH_RESEARCH_API_CLASS TransducersState
{
public:
    /** Basic constructor */
    ULH_RESEARCH_API TransducersState();
    /** Constructor for a transducer activation vector for a given transducer count
     *
     * @param itransducer_count The number of the transducers in the
     * transducer activation vector */
    ULH_RESEARCH_API explicit TransducersState(size_t itransducer_count);
    /** Constructor for a transducer activation vector, given a pre-existing set of
     * complex-valued amplitude/phases transducer initialisations.
     *
     * @param interleaved_complex An array of interleaved single-precision
     * complex numbers
     *
     * @param itransducer_count The transducer count, and also how many
     * complex numbers to use from the array to initialise the system */
    ULH_RESEARCH_API TransducersState(const float interleaved_complex[], size_t itransducer_count);
    /** Copy constructor for a transducer activation vector.
     *
     * @param other The other transducer activation vector to copy */
    ULH_RESEARCH_API TransducersState(const TransducersState& other);
    /** Copy assignment operator for a transducer activation vector.
     *
     * @param other The other transducer activation vector to copy
     *
     * @return Itself */
    ULH_RESEARCH_API TransducersState& operator=(const TransducersState& other);
    /** Transducer state in-place addition operator.
     *
     * @param other The other transducer state to add to this one
     *
     * @return Itself. */
    ULH_RESEARCH_API TransducersState& persistentAccumulate(const TransducersState& other);
    /** Transducer state in-place addition operator.
     *
     * @param other The other transducer state to add to this one
     *
     * @return Itself */
    ULH_RESEARCH_API TransducersState& operator+=(const TransducersState& other);
    /** Transducer state addition operator.
     *
     * @param other The other transducer state to add to this one
     *
     * @return The new added states */
    ULH_RESEARCH_API TransducersState operator+(const TransducersState& other) const;
    /** Transducer state in-place scaling operator.
     *
     * @param scale The scaling factor to scale this state by.
     *
     * @return Itself after scaling */
    ULH_RESEARCH_API TransducersState& operator*=(float scale);
    /** Transducer state scaling operator.
     *
     * @param scale The scaling factor to scale this state by.
     *
     * @return The new scaled state */
    ULH_RESEARCH_API TransducersState operator*(float scale) const;
    /** Destructor */
    ULH_RESEARCH_API ~TransducersState();
    /** Set the transducer activation vector to all zero values. */
    ULH_RESEARCH_API void setZero();
#if !defined(NO_CXX_COMPLEX_AVAILABLE)
    /** Return a reference to the input value.
     *
     * @param idx The index of the input to return a reference to
     *
     * @return A reference to the complex value of a particular
     * input */
    ULH_RESEARCH_API std::complex<float>& complexActivationAt(size_t idx);
    /** Return a constant reference to the input initial value.
     *
     * @param idx The index of the input to return a constant
     * reference to
     *
     * @return A constant reference to the complex value of a particular
     * input */
    ULH_RESEARCH_API const std::complex<float>& complexActivationAt(size_t idx) const;
#endif // !defined(NO_CXX_COMPLEX_AVAILABLE)

    ULH_RESEARCH_API float* data();
    ULH_RESEARCH_API const float* data() const;
    ULH_RESEARCH_API size_t dataSize() const;
    /** Set the abstract device input state size to a given element count.
     *
     * @param isolution_size The abstract device input state size */
    ULH_RESEARCH_API void setSize(size_t isolution_size);
    /** @return The current transducer count of this abstract device input state */
    ULH_RESEARCH_API size_t size() const;
    /** Is the solution completely zero? */
    ULH_RESEARCH_API bool isZero() const;
    /** Set failed solution */
    ULH_RESEARCH_API void setStateInvalid();
    /** Check for failed solution */
    ULH_RESEARCH_API bool isInvalid() const;
    /** Check for failed solution */
    ULH_RESEARCH_API bool isValid() const;
    /** Normalise the range of the transducer activation vectors to a given level
     * between zero and one, where one signifies full power.
     *
     * @param level The level to normalise this transducer activation vector to */
    ULH_RESEARCH_API void normaliseRangeTo(float level);
    /** Normalise each individual transducer to the given level between
     * zero and one, where one signifies full power.
     *
     * @param level The level to normalise each transducer of this
     * transducer activation vector to */
    ULH_RESEARCH_API void normaliseIndividuallyTo(float level);
    /** Get average amplitude */
    ULH_RESEARCH_API float averageAmplitude() const;
    /** Print out solution vector */
    ULH_RESEARCH_API void print() const;

protected:
    /** The stored abstract device input state as the std::complex<float> values */
    float* activation;
    /** The number of activation coefficients in this container */
    size_t activations_count;
    /** Is this a valid set of coefficients for reproduction? */
    bool valid;
};

/** \brief "Reduced representation" complex values for all focal points.
 *
 * This contains a single complex number for each focal point. It is an intermediate
 * result in the computation of the transducer transmission coefficients, which can
 * be calculated from this and the array geometry.
 *
 * To save USB bandwidth we can send this intermediate result instead of the full
 * transducer states, and then the array hardware can perform the final expansion
 * to transducer states.
 *
 * See \rstref{LowLevelAccess.rst} for more information on
 * state emitters.
 */
class ULH_RESEARCH_API_CLASS FocusPointState
{
public:
    /** Basic constructor */
    ULH_RESEARCH_API FocusPointState();
    /** Constructor for a focus point state for a given control point count
     *
     * @param icontrol_point_count The number of the control points in the
     * focus point state */
    ULH_RESEARCH_API explicit FocusPointState(size_t icontrol_point_count);
    /** Constructor for a focus point state, given a pre-existing set of
     * complex-valued amplitude/phases focus point initialisations.
     *
     * @param iposition_x Array of 32-bit floating point x positions in metres
     *
     * @param iposition_y Array of 32-bit floating point y positions in metres
     *
     * @param iposition_z Array of 32-bit floating point z positions in metres
     *
     * @param ireal Array of 32-bit floating point values which are the real
     * parts of the wave phasor at the focus point
     *
     * @param iimag Array of 32-bit floating point values which are the real
     * parts of the wave phasor at the focus point
     *
     * @param ifocuspoints_count The focus point count, and also how many
     * numbers to use from each array to initialise the system */
    ULH_RESEARCH_API FocusPointState(
        const float iposition_x[],
        const float iposition_y[],
        const float iposition_z[],
        const float ireal[],
        const float iimag[],
        size_t ifocuspoints_count);
    /** Constructor for a focus point state, given a pre-existing set of
     * complex-valued amplitude/phases focus point initialisations.
     *
     * @param iposition_x Array of 32-bit floating point x positions in metres
     *
     * @param iposition_y Array of 32-bit floating point y positions in metres
     *
     * @param iposition_z Array of 32-bit floating point z positions in metres
     *
     * @param iactivations Array of 32-bit complex-valued floating point
     * values which represent the wave phasor at the focus point
     *
     * @param ifocuspoints_count The focus point count, and also how many
     * numbers to use from each array to initialise the system */
    ULH_RESEARCH_API FocusPointState(
        const float iposition_x[],
        const float iposition_y[],
        const float iposition_z[],
        const std::complex<float> iactivations[],
        size_t ifocuspoints_count);
    /** Constructor for a focus point state, given a pre-existing set of
     * complex-valued amplitude/phases focus point initialisations.
     *
     * @param ixyzri An interleaved array of x, y, z, real and imaginary values,
     * where the x, y and z values are in metres, and the real and imaginary
     * values are part of the complex wave phasor at the focus point
     * @param ifocuspoints_count The number of focus points represented by ixyzri */
    ULH_RESEARCH_API FocusPointState(
        const float ixyzri[],
        size_t ifocuspoints_count);
    /** Copy constructor for a focus point state.
     *
     * @param other The other focus point state to copy from. */
    ULH_RESEARCH_API FocusPointState(const FocusPointState& other);
    /** Focus point state destructor. */
    ULH_RESEARCH_API ~FocusPointState();
    /** Copy assignment operator for a focus point state.
     *
     * @param other The other focus point state to copy from
     *
     * @return Itself. */
    ULH_RESEARCH_API FocusPointState& operator=(const FocusPointState& other);
    /** Summation/amalgamation operator for a persistent focus point state.
     *
     * @param other The other persistent focus point state to add from
     *
     * @return Itself. */
    ULH_RESEARCH_API FocusPointState& persistentAccumulate(const FocusPointState& other);
    /** Summation/amalgamation operator for a focus point state.
     *
     * @param other The other focus point state to add from
     *
     * @return Itself. */
    ULH_RESEARCH_API FocusPointState& operator+=(const FocusPointState& other);
    /** Union operator for a focus point state.
     *
     * @param other The other focus point state to combine with
     *
     * @return New union of two focus point states. */
    ULH_RESEARCH_API FocusPointState operator+(const FocusPointState& other) const;
    /** Product operator for a control point state.
     *
     * @param rhs The scale factor to scale the control point state level by
     *
     * @return Itself. */
    ULH_RESEARCH_API FocusPointState& operator*=(float rhs);
    /** Multiply operator for a control point state.
     *
     * @param rhs The scale factor to scale the control point state level by
     *
     * @return New multiplied control point state. */
    ULH_RESEARCH_API FocusPointState operator*(float rhs) const;
    /** Set all the positions and focus point wave phasors to zero.
     *
     * @return Itself after being zeroed. */
    ULH_RESEARCH_API FocusPointState& setZero();
    /** Set all the complex phasor values of the wave at each focus point to zero.
     *
     * @return Itself after being zeroed. */
    ULH_RESEARCH_API FocusPointState& setComplexActivationZero();
    /** @return An array of 32-bit floating-point values, a block of
     * interleaved position vector component first, followed by the
     * complex-values. */
    ULH_RESEARCH_API float* data();
    /** @return An constant array of 32-bit floating-point values, a block of
     * interleaved position vector component first, followed by the
     * complex-values. */
    ULH_RESEARCH_API const float* data() const;

    ULH_RESEARCH_API size_t dataSize() const;

    ULH_RESEARCH_API float* positionAt(size_t idx);
    ULH_RESEARCH_API const float* positionAt(size_t idx) const;

    /** Get a reference to the x position of the given focus point.
     *
     * @param idx The index of the given focus point whose
     * x position reference is required
     *
     * @return Reference to the position in x of the focus point at the given index. */
    ULH_RESEARCH_API float& xAt(size_t idx);
    /** Get a constant reference to the x position of the given focus point.
     *
     * @param idx The index of the given focus point whose
     * x position reference is required
     *
     * @return Constant reference to the position in x of the focus point at the given index. */
    ULH_RESEARCH_API const float& xAt(size_t idx) const;
    /** Get a reference to the y position of the given focus point.
     *
     * @param idx The index of the given focus point whose
     * y position reference is required
     *
     * @return Reference to the position in y of the focus point at the given index. */
    ULH_RESEARCH_API float& yAt(size_t idx);
    /** Get a constant reference to the y position of the given focus point.
     *
     * @param idx The index of the given focus point whose
     * y position reference is required
     *
     * @return Constant reference to the position in y of the focus point at the given index. */
    ULH_RESEARCH_API const float& yAt(size_t idx) const;
    /** Get a reference to the z position of the given focus point.
     *
     * @param idx The index of the given focus point whose
     * z position reference is required
     *
     * @return Reference to the position in z of the focus point at the given index. */
    ULH_RESEARCH_API float& zAt(size_t idx);
    /** Get a constant reference to the z position of the given focus point.
     *
     * @param idx The index of the given focus point whose
     * z position reference is required
     *
     * @return Constant reference to the position in z of the focus point at the given index. */
    ULH_RESEARCH_API const float& zAt(size_t idx) const;
    /** Get a reference to the first of the interleaved 32-bit floating-point values that
     * constitute the focus point phasor value.
     *
     * @param idx The index of the given focus point whose
     * activation phasor requires referencing
     *
     * @return The reference to the first of the interleaved 32-bit
     * floating-point values that constitute the focus point phasor value. */
    ULH_RESEARCH_API float& activationAt(size_t idx);
    /** Get a constant reference to the first of the interleaved 32-bit floating-point values that
     * constitute the focus point phasor value.
     *
     * @param idx The index of the given focus point whose
     * activation phasor requires referencing
     *
     * @return The constant reference to the first of the interleaved 32-bit
     * floating-point values that constitute the focus point phasor value. */
    ULH_RESEARCH_API const float& activationAt(size_t idx) const;
#if !defined(NO_CXX_COMPLEX_AVAILABLE)
    /** Get a reference to the 32-bit complex-valued floating-point value that
     * constitutes the focus point phasor value.
     *
     * @param idx The index of the given focus point whose
     * activation phasor requires referencing
     *
     * @return The reference to the 32-bit complex-valued
     * floating-point value that constitutes the focus point phasor value. */
    ULH_RESEARCH_API std::complex<float>& complexActivationAt(size_t idx);
    /** Get a constant reference to the 32-bit complex-valued floating-point value that
     * constitutes the focus point phasor value.
     *
     * @param idx The index of the given focus point whose
     * activation phasor requires referencing
     *
     * @return The constant reference to the 32-bit complex-valued
     * floating-point value that constitutes the focus point phasor value. */
    ULH_RESEARCH_API const std::complex<float>& complexActivationAt(size_t idx) const;
#endif // !defined(NO_CXX_COMPLEX_AVAILABLE)
    /** @return True if the state is zeroed. Both
     * positions and complex phasors must be zero for this to be true.
     * Otherwise this is false. */
    ULH_RESEARCH_API bool isAllZero() const;
    /** @return True if the complex phasors are all zeroed.
     * Otherwise this is false. */
    ULH_RESEARCH_API bool isPhasorAllZero() const;
    /** Sets the size of the focus point state - how many focus points are in the state.
     *
     * @param ifocuspoints_count Number of focus point to change to. */
    ULH_RESEARCH_API void setSize(size_t ifocuspoints_count);
    /** @return The total count of the focus points in the state. */
    ULH_RESEARCH_API size_t size() const;
    /** Print out solution vector */
    ULH_RESEARCH_API void print() const;

protected:
    /** The stored floating point vectors of the focus point positions and then 32-bit complex floating-point values */
    float* focuspoint;
    /** Focus points count */
    size_t focuspoints_count;
};

/** \brief Set of all control point and their instantaneous modulation amplitudes.
 *
 * This class represents the state of all control points - their positions, and the
 * instantaneous modulation amplitudes of the 40 kHz ultrasound field at each control
 * point.
 *
 * This data is the input from the user for Time Point Streaming. Normally it would be
 * processed through the solver, but some arrays can run the solver on-board and for those
 * we can send this input directly.
 *
 * See \rstref{LowLevelAccess.rst} for more information on
 * state emitters.
 */
class ULH_RESEARCH_API_CLASS ControlPointState
{
public:
    /** Basic constructor */
    ULH_RESEARCH_API ControlPointState();
    /** Constructor for a control point state for a given control point count
     *
     * @param icontrol_point_count The number of the control points in the
     * control point state */
    ULH_RESEARCH_API explicit ControlPointState(size_t icontrol_point_count);
    /** Constructor for a control point state, given a pre-existing set of positions and intensities. Directions are zero-initialised.
     *
     * @param iposition_x Array of 32-bit floating point x positions in metres
     *
     * @param iposition_y Array of 32-bit floating point y positions in metres
     *
     * @param iposition_z Array of 32-bit floating point z positions in metres
     *
     * @param iintensity Array of 32-bit floating point values which are the
     * intensities of the wave at each control point, between 0.0 and 1.0
     *
     * @param igroups Array of integer group IDs
     *
     * @param icontrolpoints_count The control point count, and also how many
     * numbers to use from each array to initialise the system */
    ULH_RESEARCH_API ControlPointState(
        const float iposition_x[],
        const float iposition_y[],
        const float iposition_z[],
        const float iintensity[],
        const uint32_t igroups[],
        size_t icontrolpoints_count);
    /** Constructor for a control point state, given a pre-existing set of positions and intensities and directions.
     *
     * @param iposition_x Array of 32-bit floating point x positions in metres
     *
     * @param iposition_y Array of 32-bit floating point y positions in metres
     *
     * @param iposition_z Array of 32-bit floating point z positions in metres
     *
     * @param iintensity Array of 32-bit floating point values which are the
     * intensities of the wave at each control point, between 0.0 and 1.0
     *
     * @param idirection_x Array of 32-bit floating point x directions in metres. Can be nullptr.
     *
     * @param idirection_y Array of 32-bit floating point y directions in metres. Can be nullptr.
     *
     * @param idirection_z Array of 32-bit floating point z directions in metres. Can be nullptr.
     *
     * @param iimaginary Array of 32-bit floating point imaginary values. Can be nullptr.
     *
     * @param igroups Array of integer group IDs
     *
     * @param icontrolpoints_count The control point count, and also how many
     * numbers to use from each array to initialise the system */
    ULH_RESEARCH_API ControlPointState(
        const float iposition_x[],
        const float iposition_y[],
        const float iposition_z[],
        const float iintensity[],
        const float idirection_x[],
        const float idirection_y[],
        const float idirection_z[],
        const float iimaginary[],
        const uint32_t igroups[],
        size_t icontrolpoints_count);

    /** Copy constructor for a control point state.
     *
     * @param other The other control point state to copy from. */
    ULH_RESEARCH_API ControlPointState(const ControlPointState& other);
    /** Control point state destructor. */
    ULH_RESEARCH_API virtual ~ControlPointState();
    /** Copy assignment operator for a control point state.
     *
     * @param other The other control point state to copy from
     *
     * @return Itself. */
    ULH_RESEARCH_API ControlPointState& operator=(const ControlPointState& other);
    /** Summation/amalgamation operator for a persistent control point state.
     *
     * @param other The other persistent control point state to add from
     *
     * @return Itself. */
    ULH_RESEARCH_API ControlPointState& persistentAccumulate(const ControlPointState& other);
    /** Summation/amalgamation operator for a control point state.
     *
     * @param other The other control point state to add from
     *
     * @return Itself. */
    ULH_RESEARCH_API ControlPointState& operator+=(const ControlPointState& other);
    /** Union operator for a control point state.
     *
     * @param other The other control point state to combine with
     *
     * @return Itself. */
    ULH_RESEARCH_API ControlPointState operator+(const ControlPointState& other) const;
    /** Product operator for a control point state.
     *
     * @param rhs The scale factor to scale the control point state level by
     *
     * @return Itself. */
    ULH_RESEARCH_API ControlPointState& operator*=(float rhs);
    /** Multiply operator for a control point state.
     *
     * @param rhs The scale factor to scale the control point state level by
     *
     * @return New multiplied control point state. */
    ULH_RESEARCH_API ControlPointState operator*(float rhs) const;
    /** Set all the positions and intensities to zero.
     *
     * @return Itself after being zeroed. */
    ULH_RESEARCH_API ControlPointState& setZero();
    /** Set all the intensities to zero.
     *
     * @return Itself after being zeroed. */
    ULH_RESEARCH_API ControlPointState& setIntensitiesZero();
    /** @return An array of 32-bit floating-point values, a block of
     * interleaved position vector component first, followed by the
     * floating-point intensity values. */
    ULH_RESEARCH_API float* data();
    /** @return An constant array of 32-bit floating-point values, a block of
     * interleaved position vector component first, followed by the
     * floating-point intensity values. */
    ULH_RESEARCH_API const float* data() const;

    ULH_RESEARCH_API size_t dataSize() const;

    /** Get a reference to the x position of the given control point.
     *
     * @param idx The index of the given control point whose x position reference is required
     *
     * @return Reference to the position in x of the control point at the given index. */
    ULH_RESEARCH_API float& xAt(size_t idx);
    /** Get a constant reference to the x position of the given control point.
     *
     * @param idx The index of the given control point whose x position reference is required
     *
     * @return Constant reference to the position in x of the control point at the given index. */
    ULH_RESEARCH_API const float& xAt(size_t idx) const;
    /** Get a reference to the y position of the given control point.
     *
     * @param idx The index of the given control point whose y position reference is required
     *
     * @return Reference to the position in y of the control point at the given index. */
    ULH_RESEARCH_API float& yAt(size_t idx);
    /** Get a constant reference to the y position of the given control point.
     *
     * @param idx The index of the given control point whose y position reference is required
     *
     * @return Constant reference to the position in y of the control point at the given index. */
    ULH_RESEARCH_API const float& yAt(size_t idx) const;
    /** Get a reference to the z position of the given control point.
     *
     * @param idx The index of the given control point whose z position reference is required
     *
     * @return Reference to the position in z of the control point at the given index. */
    ULH_RESEARCH_API float& zAt(size_t idx);
    /** Get a constant reference to the z position of the given control point.
     *
     * @param idx The index of the given control point whose z position reference is required
     *
     * @return Constant reference to the position in z of the control point at the given index. */
    ULH_RESEARCH_API const float& zAt(size_t idx) const;
    /** Get a reference to the floating-point values that
     * constitute each control point intensity value, between 0.0 and 1.0.
     *
     * @param idx The index of the given control point whose intensity requires referencing
     *
     * @return The reference to the floating-point value that
     * constitutes the control point amplitude value. */
    ULH_RESEARCH_API float& intensityAt(size_t idx);
    /** Get a constant reference to the floating-point values that
     * constitute each control point intensity value, between 0.0 and 1.0.
     *
     * @param idx The index of the given control point whose intensity requires referencing
     *
     * @return The constant reference to the floating-point value that
     * constitutes the control point amplitude value. */
    ULH_RESEARCH_API const float& intensityAt(size_t idx) const;

    /** Get a reference to the x direction of the given control point.
     *
     * @param idx The index of the given control point whose x direction reference is required
     *
     * @return Reference to the direction in x of the control point at the given index. */
    ULH_RESEARCH_API float& xDirectionAt(size_t idx);
    /** Get a constant reference to the x direction of the given control point.
     *
     * @param idx The index of the given control point whose x direction reference is required
     *
     * @return Constant reference to the direction in x of the control point at the given index. */
    ULH_RESEARCH_API const float& xDirectionAt(size_t idx) const;
    /** Get a reference to the y direction of the given control point.
     *
     * @param idx The index of the given control point whose y direction reference is required
     *
     * @return Reference to the direction in y of the control point at the given index. */
    ULH_RESEARCH_API float& yDirectionAt(size_t idx);
    /** Get a constant reference to the y direction of the given control point.
     *
     * @param idx The index of the given control point whose y direction reference is required
     *
     * @return Constant reference to the direction in y of the control point at the given index. */
    ULH_RESEARCH_API const float& yDirectionAt(size_t idx) const;
    /** Get a reference to the z direction of the given control point.
     *
     * @param idx The index of the given control point whose z direction reference is required
     *
     * @return Reference to the direction in z of the control point at the given index. */
    ULH_RESEARCH_API float& zDirectionAt(size_t idx);
    /** Get a constant reference to the z normal of the given control point.
     *
     * @param idx The index of the given control point whose z direction reference is required
     *
     * @return Constant reference to the direction in z of the control point at the given index. */
    ULH_RESEARCH_API const float& zDirectionAt(size_t idx) const;

    /** Get a constant reference to the imaginary of the given control point.
     *
     * @param idx The index of the given control point whose imaginary reference is required
     *
     * @return Constant reference to the imaginary of the control point at the given index. */
    ULH_RESEARCH_API const float& imaginaryAt(size_t idx) const;
    ULH_RESEARCH_API float& imaginaryAt(size_t idx);
    ULH_RESEARCH_API const float& rangeAt(size_t idx) const;
    ULH_RESEARCH_API float& rangeAt(size_t idx);

    /** Get a reference to the group ID for the specified control point in the state
     *
     * @param idx The index of the control point to get a group ID reference for
     * @return The reference to the given group ID */
    ULH_RESEARCH_API uint32_t& groupAt(size_t idx);
    /** Get a constant reference to the group ID for the specified control point in the state
     *
     * @param idx The index of the control point to get a group ID reference for
     * @return The reference to the given group ID */
    ULH_RESEARCH_API const uint32_t& groupAt(size_t idx) const;
    /** @return True if the state is zeroed.
     * All positions and amplitudes must be zero for this to be true, otherwise it is false. */
    ULH_RESEARCH_API bool isAllZero() const;
    /** @return True if the itensities are all zeroed.
     * Otherwise this is false. */
    ULH_RESEARCH_API bool isIntensityAllZero() const;
    /** Sets the size of the control point state - how many control points are in the state.
     *
     * @param icontrolpoints_count Number of focus point to change to. */
    ULH_RESEARCH_API void setSize(size_t icontrolpoints_count);
    /** @return The total count of the control points in the state. */
    ULH_RESEARCH_API size_t size() const;
    /** Print out solution vector */
    ULH_RESEARCH_API void print() const;

protected:
    /** The stored floating point vectors of the control point positions and then 32-bit floating point intensities.
     * This is stored as x1,y1,z1,x2,y2,z2, ...., i1,i2,i3, ... to simplify AMControlPointState. */
    float* controlpoint;
    /** The stored control point group IDs */
    uint32_t* cpgroups;
    /** Control points count */
    size_t controlpoints_count;
    float* cprange;
};

/** \brief Set of all control point and their sinusoidal modulation frequency.
 *
 * This class represents the state of all control points that have a default sinusoidal
 * modulation. It contains their positions, and the frequency of the modulation.
 *
 * This data is the input from the user for Amplitude Modulation. Normally it would be
 * processed through the solver, but some arrays can run the solver on-board and for those
 * we can send this input directly.
 *
 * See \rstref{LowLevelAccess.rst} for more information on
 * state emitters.
 */
class ULH_RESEARCH_API_CLASS AMControlPointState : public ControlPointState
{
public:
    /** Basic constructor */
    ULH_RESEARCH_API AMControlPointState();
    /** Constructor for a control point state for a given control point count
     *
     * @param icontrol_point_count The number of the control points in the
     * control point state */
    ULH_RESEARCH_API explicit AMControlPointState(size_t icontrol_point_count);
    /** Constructor for a control point state, given a pre-existing set of
     * positions, intensities and frequencies
     *
     * @param iposition_x Array of 32-bit floating point x positions in metres
     *
     * @param iposition_y Array of 32-bit floating point y positions in metres
     *
     * @param iposition_z Array of 32-bit floating point z positions in metres
     *
     * @param iintensity Array of 32-bit floating point values which are the
     * intensities of the wave at each control point, between 0.0 and 1.0
     *
     * @param igroups Array of integer control point group IDs
     *
     * @param icontrolpoints_count The focus point count, and also how many
     * numbers to use from each array to initialise the system
     *
     * @param ifrequency The frequency of the control points in this state */
    ULH_RESEARCH_API AMControlPointState(
        const float iposition_x[],
        const float iposition_y[],
        const float iposition_z[],
        const float iintensity[],
        const uint32_t igroups[],
        size_t icontrolpoints_count,
        float ifrequency);
    /** Constructor for a control point state, given a pre-existing set of positions and intensities and directions.
     *
     * @param iposition_x Array of 32-bit floating point x positions in metres
     *
     * @param iposition_y Array of 32-bit floating point y positions in metres
     *
     * @param iposition_z Array of 32-bit floating point z positions in metres
     *
     * @param iintensity Array of 32-bit floating point values which are the
     * intensities of the wave at each control point, between 0.0 and 1.0
     *
     * @param idirection_x Array of 32-bit floating point x directions in metres. Can be nullptr.
     *
     * @param idirection_y Array of 32-bit floating point y directions in metres. Can be nullptr.
     *
     * @param idirection_z Array of 32-bit floating point z directions in metres. Can be nullptr.
     *
     * @param igroups Array of integer group IDs
     *
     * @param icontrolpoints_count The control point count, and also how many
     * numbers to use from each array to initialise the system
     *
     * @param ifrequency The frequency of the control points in this state */
    ULH_RESEARCH_API AMControlPointState(
        const float iposition_x[],
        const float iposition_y[],
        const float iposition_z[],
        const float iintensity[],
        const float idirection_x[],
        const float idirection_y[],
        const float idirection_z[],
        const uint32_t igroups[],
        size_t icontrolpoints_count,
        float ifrequency);

    /** Copy constructor for a control point state.
     *
     * @param other The other control point state to copy from. */
    ULH_RESEARCH_API AMControlPointState(const AMControlPointState& other);
    /** Control point state destructor. */
    ULH_RESEARCH_API ~AMControlPointState() override;
    /** Copy assignment operator for a control point state.
     *
     * @param other The other control point state to copy from
     *
     * @return Itself. */
    ULH_RESEARCH_API AMControlPointState& operator=(const AMControlPointState& other);
    /** Get a reference to the floating-point frequency value of this state. */
    ULH_RESEARCH_API float& frequency();
    /** Get a constant reference to the floating-point frequency value of this state. */
    ULH_RESEARCH_API const float& frequency() const;

private:
    float frequency_;
};

/** \brief A set of transducer states which will be cycled between at a certain frequency.
 *
 * This class stores an array of TransducersState objects. Each TransducersState object
 * contains the phase and amplitude coefficients for every transducer. Therefore an array
 * of them provides a sequency of phase/amplitude coefficients to loop through.
 *
 * This class should be sent using SingleStateEmitter. You can send it once and then it
 * will keep looping through the states until you send a replacement CyclingTransducersState.
 *
 * See \rstref{LowLevelAccess.rst} for more information on
 * state emitters.
 */
class ULH_RESEARCH_API_CLASS CyclingTransducersState
{
public:
    /** Basic constructor */
    ULH_RESEARCH_API CyclingTransducersState();
    /** Constructor for a state object for a given state count
     *
     * @param istate_count The number of states in the state array
     * @param ifrequency The frequency to cycle between states at */
    ULH_RESEARCH_API CyclingTransducersState(size_t istate_count, float ifrequency);
    /** Constructor for a state object containing a given state count
    *
    * @param istate_count The number of states in the state array
    * @param istate_individual_count The size (number of transducers) of each state in the state array
    * @param ifrequency The frequency to cycle between states at */
    ULH_RESEARCH_API CyclingTransducersState(size_t istate_count, size_t istate_individual_count, float ifrequency);
    /** Constructor for a state object containing a set of transducer states
     *
     * @param istates The array of states to use when initialising this state container
     * @param istate_count The state count, and also how many states to use from the array to initialise the system
     * @param ifrequency The frequency to cycle between states at */
    ULH_RESEARCH_API CyclingTransducersState(const TransducersState* istates, size_t istate_count, float ifrequency);
    /** Copy constructor for a cycling transducer state.
     *
     * @param other The other state container to copy */
    ULH_RESEARCH_API CyclingTransducersState(const CyclingTransducersState& other);
    /** Copy assignment operator for a cycling transducer state.
     *
     * @param other The other cycling transducer state to copy
     *
     * @return Itself */
    ULH_RESEARCH_API CyclingTransducersState& operator=(const CyclingTransducersState& other);
    /** Cycling transducer state in-place scaling operator.
     *
     * @param scale The scaling factor to scale these states by.
     *
     * @return Itself after scaling */
    ULH_RESEARCH_API CyclingTransducersState& operator*=(float scale);
    /** Cycling transducer state scaling operator.
     *
     * @param scale The scaling factor to scale these states by.
     *
     * @return The new scaled states */
    ULH_RESEARCH_API CyclingTransducersState operator*(float scale) const;
    /** Destructor */
    ULH_RESEARCH_API ~CyclingTransducersState();
    /** Set all states to all zero values. */
    ULH_RESEARCH_API void setZero();
    /** Access the state at the specified index
     *
     * @param idx The index into this object to access.
     * @return A reference to the state held within this object. */
    ULH_RESEARCH_API TransducersState& stateAt(size_t idx);
    /** Access the state at the specified index
     *
     * @param idx The index into this object to access.
     * @return A reference to the state held within this object. */
    ULH_RESEARCH_API const TransducersState& stateAt(size_t idx) const;
    /** Set the frequency of this cycling state
     * @param ifrequency the new frequency to use */
    ULH_RESEARCH_API void setFrequency(float ifrequency);
    /** The current frequency of this cycling state */
    ULH_RESEARCH_API float frequency() const;
    /** Change the number of states contained within this object
     *
     * @param istate_count The new state count of this object */
    ULH_RESEARCH_API void setSize(size_t istate_count);
    /** @return The current state count of this object */
    ULH_RESEARCH_API size_t size() const;
    /** Are all states completely zero? */
    ULH_RESEARCH_API bool isZero() const;
    /** Get average amplitude */
    ULH_RESEARCH_API float averageAmplitude() const;
    /** Access underlying TransducersState objects */
    ULH_RESEARCH_API TransducersState* data();
    /** Access underlying TransducersState objects */
    ULH_RESEARCH_API const TransducersState* data() const;

protected:
    /** The stored abstract device input state as the std::complex<float> values */
    TransducersState* states;
    /** The number of activation coefficients in this container */
    size_t states_count;
    /** The frequency at which to cycle between states */
    float cycling_frequency;
};

/** \brief A single uniform transmission coefficient (phase & amplitude) for all transducers.
 *
 * This class stores the phase and amplitude coefficient as a complex number which will be applied
 * to all the transducers in an array. Internally it is equivalent to `std::vector<std::complex<float>>`.
 *
 * By streaming this state to the device you can uinformly control the activation of the
 * transducers.
 *
 * See \rstref{LowLevelAccess.rst} for more information on
 * state emitters.
 */
class ULH_RESEARCH_API_CLASS CommonTransducerState
{
public:
    /** Basic constructor */
    ULH_RESEARCH_API CommonTransducerState() = default;
    /** Constructor for a uniform transducer activation, given a
     * complex-valued transducer initialisation.
     *
     * @param real the real part of the complex transducer activation
     * @param imag the imaginary part of the complex transducer activation */
    ULH_RESEARCH_API CommonTransducerState(float real, float imag);
    /** Copy constructor for a uniform transducer activation.
     *
     * @param other The other transducer activation to copy */
    ULH_RESEARCH_API CommonTransducerState(const CommonTransducerState& other);
    /** Copy assignment operator for a uniform transducer activation.
     *
     * @param other The other transducer activation to copy
     *
     * @return Itself */
    ULH_RESEARCH_API CommonTransducerState& operator=(const CommonTransducerState& other);
    /** Common transducer state in-place addition operator.
     *
     * @param other The other common transducer state to add to this one
     *
     * @return Itself. */
    ULH_RESEARCH_API CommonTransducerState& persistentAccumulate(const CommonTransducerState& other);
    /** Common transducer state in-place addition operator.
     *
     * @param other The other common transducer state to add to this one
     *
     * @return Itself */
    ULH_RESEARCH_API CommonTransducerState& operator+=(const CommonTransducerState& other);
    /** Common transducer state addition operator.
     *
     * @param other The other common transducer state to add to this one
     *
     * @return The new added states */
    ULH_RESEARCH_API CommonTransducerState operator+(const CommonTransducerState& other) const;
    /** Common transducer state in-place scaling operator.
     *
     * @param scale The scaling factor to scale this state by.
     *
     * @return Itself after scaling */
    ULH_RESEARCH_API CommonTransducerState& operator*=(float scale);
    /** Common transducer state scaling operator.
     *
     * @param scale The scaling factor to scale this state by.
     *
     * @return The new scaled state */
    ULH_RESEARCH_API CommonTransducerState operator*(float scale) const;
#if !defined(NO_CXX_COMPLEX_AVAILABLE)
    /** Return a reference to the input value.
     *
     * @return A reference to the complex value of the input */
    ULH_RESEARCH_API std::complex<float>& complexActivation();
    /** Return a constant reference to the input value.
     *
     * @return A reference to the complex value of the input */
    ULH_RESEARCH_API const std::complex<float>& complexActivation() const;
#endif // !defined(NO_CXX_COMPLEX_AVAILABLE)

    ULH_RESEARCH_API float* data();
    ULH_RESEARCH_API const float* data() const;
    ULH_RESEARCH_API size_t dataSize() const;
    /** This is a place holder function, size is always 1 for this type.*/
    ULH_RESEARCH_API void setSize(size_t isolution_size);
    /** This is a place holder function, size is always 1 for this type.*/
    ULH_RESEARCH_API size_t size() const;

protected:
    /** The stored device input state as the std::complex<float> value
     * two extra floats for SIMD padding. */
    float activation[4] = { 0, 0, 0, 0 };
};
} // namespace Haptics

} // namespace Ultraleap
