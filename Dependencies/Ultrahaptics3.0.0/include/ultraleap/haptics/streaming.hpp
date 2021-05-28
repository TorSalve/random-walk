
#pragma once

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/control_point.hpp"
#include "ultraleap/haptics/emitter.hpp"
#include "ultraleap/haptics/local_time.hpp"
#include "ultraleap/haptics/library.hpp"
#include "ultraleap/haptics/transform.hpp"

namespace Ultraleap
{
namespace Haptics
{

class OutputInterval;

/** \brief A point in time on which control points can be set
 *
 * Class that describes a point in time on which control
 * points can be set.
 */
class ULH_API_CLASS TimePointOnOutputInterval : public LocalTimePoint
{
    friend class OutputIntervalIterator;

public:
    /** Get a reference to a persistent control point at a given index
     *
     * @param idx The index of the control point to get
     *
     * @return A reference to the persistent control point at the given index
     */
    ULH_API ControlPoint& controlPoint(size_t idx);
    /** Get a constant reference to a persistent control point at a given index
     *
     * @param idx The index of the control point to get
     *
     * @return A constant reference to the persistent control point at the given index
     */
    ULH_API const ControlPoint& controlPoint(size_t idx) const;

protected:
    /** Protected constructor for output interval of time point.
     *
     * @param itime The time which this timepoint corresponds to
     *
     * @param iinterval The interval that this time point refers back to.
     */
    TimePointOnOutputInterval(const LocalTimePoint& itime, OutputInterval& iinterval);
    /** Reference to interval */
    OutputInterval& interval;
};

/** \brief Iterator for the output interval
 *
 * Class that describes a point in time on which control
 * points can be set, which is also an iterator for the output interval.
 */
class ULH_API_CLASS OutputIntervalIterator
{
    friend class OutputInterval;

public:
    /** Constant reference to the time point on the output interval in question. */
    typedef const TimePointOnOutputInterval& const_reference;
    /** Constant pointer to the time point on the output interval in question. */
    typedef const TimePointOnOutputInterval* const_pointer;
    /** Reference to the time point on the output interval in question. */
    typedef TimePointOnOutputInterval& reference;
    /** Pointer to the time point on the output interval in question. */
    typedef TimePointOnOutputInterval* pointer;
    /** Operator overload for pre-increment
     *
     * @return This with sample interval increment.
     */
    ULH_API OutputIntervalIterator& operator++();
    /** Operator overload for less than compare
     *
     * @param other The other iterator to compare this to.
     */
    ULH_API bool operator<(const OutputIntervalIterator& other) const;
    /** Operator overload for unequal comparison
     *
     * @param other The other iterator to compare this to.
     */
    ULH_API bool operator!=(const OutputIntervalIterator& other) const;
    /** Operator overload of dereference
     *
     * @return Reference to the dereferenced time point that is held by this iterator.
     */
    ULH_API reference operator*();
    /** Operator overload of pointer member access.
     *
     * @return Pointer to time point that is held by this iterator.
     */
    ULH_API pointer operator->();
    /** Operator overload of constant dereference
     *
     * @return Reference to the dereferenced time point that is held by this iterator.
     */
    ULH_API const_reference operator*() const;
    /** Operator overload of constant pointer member access.
     *
     * @return Pointer to time point that is held by this iterator.
     */
    ULH_API const_pointer operator->() const;
    /** Get the iterator interval.
     *
     * @return The duration corresponding to the iterator time step.
     */
    ULH_API const LocalDuration& iteratorTimeInterval() const;
    /** Get the iterator time.
     *
     * @return The time point corresponding to the iterator.
     */
    ULH_API const LocalTimePoint& time() const;

protected:
    /** Protected constructor. */
    OutputIntervalIterator(const LocalTimePoint& itime, OutputInterval& iinterval);
    /** Protected time point corresponding to the current time. */
    TimePointOnOutputInterval current_time_point;
    /** Reference to interval */
    OutputInterval& interval;
};

class OutputIntervalImplementation;

/** Class that describes and escapsulates an output interval in time.
     *
     * \brief Representation of an output time interval */
class ULH_API_CLASS OutputInterval
{
    friend class OutputIntervalIterator;
    friend class TimePointOnOutputInterval;

public:
    /** Public typedef for iterator */
    typedef OutputIntervalIterator iterator;
    /** Get the start of the interval as a timepoint.
     *
     * @return Time point pointing to the beginning of the interval.
     */
    ULH_API virtual const LocalTimePoint& intervalBegin() const = 0;
    /** Get a time point pointing to the end of the interval.
     *
     * @return Time point pointing to the end of the interval.
     */
    ULH_API virtual const LocalTimePoint& intervalEnd() const = 0;
    /** Get a time point pointing to the first sample of the interval.
     *
     * @return Time point pointing to the first sample of the interval.
     */
    ULH_API virtual const LocalTimePoint& firstSample() const = 0;
    /** Get the first output position of the interval,
     * according to the sampling rate.
     *
     * @return Iterator pointing to the first output position.
     */
    ULH_API iterator begin();
    /** Get an iterator that points to a time point after the
     * last valid output time.
     *
     * @return Iterator pointing after the last valid output time.
     */
    ULH_API iterator end();
    /** Get the iterator time step.
     *
     * @return The duration corresponding to the iterator time step.
     */
    ULH_API virtual const LocalDuration& iteratorTimeInterval() const = 0;

protected:
    /** Protected constructor. */
    OutputInterval() = default;

    /** Deleted copy constructor. */
    OutputInterval(const OutputInterval& other) = delete;
    /** Deleted copy assignment operator. */
    OutputInterval& operator=(const OutputInterval& other) = delete;
    /** Destructor. */
    virtual ~OutputInterval() = default;

private:
    /** \cond EXCLUDE_FROM_DOCS */
    virtual ControlPoint& controlPoint(size_t idx) = 0;
    virtual const ControlPoint& controlPoint(size_t idx) const = 0;
    virtual bool commit(const LocalTimePoint& icurrent_time) = 0;
    /** \endcond */
};

/** \cond EXCLUDE_FROM_DOCS */
class StreamingEmitter;
/** \endcond */

/** Callback function for a Streaming Emitter.
 * @param emitter The StreamingEmitter this callback was called on.
 * @param interval The time interval that control point updates are being requested for.
 * @param submission_deadline The time by which the callback must complete to ensure continuous output.
 * @param user_pointer A user data pointer for storing arbitrary information.
 */
typedef void (*EmissionCallback)(
    const StreamingEmitter& emitter,
    OutputInterval& interval,
    const LocalTimePoint& submission_deadline,
    void* user_pointer);

using EmissionCallbackFunction = std::function<
    void(const StreamingEmitter& emitter,
        OutputInterval& interval,
        const LocalTimePoint& submission_deadline)>;

/** Callback function for Streaming Emitter.
 *
 * This is a convenience function that is being called for each timepoint. It takes a single
 * duration as argument, which represents the time since the emitter callback was set.
 *
 * @param duration duration since the emission callback was set
 */
using DurationEmissionFunction = std::function<
    std::vector<ControlPoint>(const StreamingEmitter& emitter,
        LocalTimePoint::duration duration)>;

/** Callback function for Streaming Emitter.
 *
 * This is a convenience function that is being called for each timepoint. It takes a single
 * time point as an argument, which represents the time at which these points will be emitted.
 *
 * @param time_point local time point that control points are being requested for
 */
using TimePointEmissionFunction = std::function<
    std::vector<ControlPoint>(const StreamingEmitter& emitter,
        LocalTimePoint time_point)>;

/** \brief Emit haptic points with custom modulation using a custom callback function.
 *
 * Unlike the Basic emitter, where a default sine wave modulation is always used,
 * the Streaming emitter allows an arbitrary modulation signal to be specified. This provides more
 * advanced control over the haptic sensation and even allows you to use the array to play music.
 *
 * See \rstref{TutorialStreaming.rst} for an introduction to the Streaming emitter.
 *
 * This class is movable.
 *
 * For an example of how to use a StreamingEmitter see the Streaming examples */
class ULH_API_CLASS StreamingEmitter : public Emitter
{
public:
    /** Constructs a streaming emitter using an existing library.
     *
     * @param ulh_lib A library with which to construct this emitter. */
    ULH_API explicit StreamingEmitter(Library ulh_lib);
    /** \cond EXCLUDE_FROM_DOCS */
    /** Move constructor. */
    StreamingEmitter(StreamingEmitter&& other) noexcept
        : Emitter(std::move(other))
        , callback(std::move(other.callback))
    {
        changeFrontend();
    }

    /** Move assignment operator. */
    StreamingEmitter& operator=(StreamingEmitter&& other) noexcept
    {
        if (this != &other) {
            Emitter::operator=(std::move(other));
            callback = std::move(other.callback);
            changeFrontend();
        }
        return *this;
    }

    /** Deleted copy constructor. */
    StreamingEmitter(const StreamingEmitter& other) = delete;
    /** Deleted copy assignment operator. */
    StreamingEmitter& operator=(const StreamingEmitter& other) = delete;
    /** \endcond */
    /** Destroys this emitter and all of its resources. */
    ULH_API virtual ~StreamingEmitter();

    /** Set emission callback.
     *
     * @param callback_fn The callback function for updating the timepoint emissions.
     * If the array sample rate is 16000Hz and the emission callback requests data for 8 intervals,
     * then you'd expect the callback to be called every 500 microseconds.
     *
     * @param set_user_pointer Set user pointer for the user of the callback to access
     * their data from inside the callback function.
     *
     * @return No value if the callback was set successfully, or an Error. */
    ULH_API result<void> setEmissionCallback(EmissionCallback callback_fn, void* set_user_pointer);

    inline result<void> setEmissionCallback(EmissionCallbackFunction&& callback_fn)
    {
        if (callback_fn != nullptr) {
            callback = std::move(callback_fn);
            return setEmissionCallback(&StreamingEmitter::callback_function, this);
        }
        return setEmissionCallback(nullptr, nullptr);
    }

    /** Set emission callback.
     *
     * Helper function to allow the use of a member function as a callback.
     *
     * @param fun reference to a member function of a user defined object,
     * similar to EmissionCallbackFunction except the user_pointer is the owning object.
     *
     * @param t the instace of the user defined object from which to call fun
     *
     * @return No value if the callback was set successfully, or an Error. */
    template <typename Ret, typename T, typename... Args>
    inline result<void> setEmissionCallback(Ret (T::*fun)(Args...), T& t)
    {
        static_assert(std::is_same<
                          decltype(fun),
                          void (T::*)(const StreamingEmitter&,
                              OutputInterval&, const LocalTimePoint&)>::value,
            "Member function signature does not match callback");
        return setEmissionCallback([fun, &t](Args... args) { return (t.*fun)(args...); });
    }

    /** Set emission callback.
     *
     * Helper function to allow the use of a member function as a callback.
     *
     * @param fun reference to a const member function of a user defined object,
     * similar to EmissionCallbackFunction except the user_pointer is the owning object.
     *
     * @param t a const instace of the user defined object from which to call fun
     *
     * @return No value if the callback was set successfully, or an Error. */
    template <typename Ret, typename T, typename... Args>
    inline result<void> setEmissionCallback(Ret (T::*fun)(Args...) const, const T& t)
    {
        static_assert(std::is_same<
                          decltype(fun),
                          void (T::*)(const StreamingEmitter&,
                              OutputInterval&, const LocalTimePoint&) const>::value,
            "Member function signature does not match callback");
        return setEmissionCallback([fun, &t](Args... args) { return (t.*fun)(args...); });
    }

    /** Set duration emission callback.
     *
     * @param callback_fn The callback function for updating the timepoint emissions.
     *
     * @return No value if the callback was set successfully, or an Error. */
    inline result<void> setEmissionCallback(DurationEmissionFunction callback_fn);

    /** Set timepoint emission callback.
     *
     * @param callback_fn The callback function for updating the timepoint emissions.
     *
     * @return No value if the callback was set successfully, or an Error. */
    inline result<void> setEmissionCallback(TimePointEmissionFunction callback_fn);

    /** Set the number of control points available for controlling in the callback.
     *
     * @param count Maximum number of control points that it is expected you will use.
     *
     * @param adjust Enum value to determine how device update rates are adjust when this
     * function is called.
     * @see AdjustRate
     *
     * @return No value or an Error. */
    ULH_API result<void> setControlPointCount(size_t count, AdjustRate adjust);

    /** Get the number of control points available for controlling in the callback.
     *
     * @return The number of control points that can be used or an Error. */
    ULH_API result<size_t> getControlPointCount() const;

    /** Start the emission callback for this emitter
     *
     * @return No value or an Error. */
    ULH_API result<void> start();

    /** Gets the number of times the callback has been called since start
     *
     * @return the number of calls. */
    ULH_API result<size_t> getCallbackIterations() const;

    /** Gets the number of times the callback has been missed since start
     *
     * @return the number of missed calls. */
    ULH_API result<size_t> getMissedCallbackIterations() const;

    /** Gets the rate at which the callback function will be called by this emitter.
     *
     * @return The rate at which the callback will be called in Hertz. */
    ULH_API float getCallbackRate() const;
    /** Gets the interval between which the callback function will be called by this emitter.
     *
     * @return The duration of the intervals between calls to the callback.
     * @see LocalDuration */
    ULH_API LocalDuration getCallbackInterval() const;
    /** Sets the rate at which the callback function will be called by this emitter.
     *
     * @param new_rate The rate at which the callback will be called in Hertz.
     *
     * @return No value if the rate was set, or an Error if it was not. */
    ULH_API result<void> setCallbackRate(float new_rate);

    /** The default rate at which the user callback will be called, in Hz */
    static constexpr float DefaultCallbackRate = 1000.0f;

private:
    /** \cond EXCLUDE_FROM_DOCS */
    EmissionCallbackFunction callback;

    static void callback_function(const StreamingEmitter& emitter,
        OutputInterval& interval,
        const LocalTimePoint& submission_deadline,
        void* user_pointer)
    {
        auto* self = static_cast<StreamingEmitter*>(user_pointer);
        EmissionCallbackFunction local_callback = self->callback;
        local_callback(emitter, interval, submission_deadline);
    }

    ULH_API void changeFrontend();
    /** \endcond */
};
/** \cond EXCLUDE_FROM_DOCS */
inline result<void> StreamingEmitter::setEmissionCallback(DurationEmissionFunction callback_fn)
{
    LocalTimePoint start = LocalTimePoint::clock::now();
    return setEmissionCallback(
        [=](const StreamingEmitter& emitter, LocalTimePoint tp) {
            return callback_fn(emitter, tp - start);
        });
}

inline result<void> StreamingEmitter::setEmissionCallback(TimePointEmissionFunction callback_fn)
{
    return setEmissionCallback(
        [=](const StreamingEmitter& emitter, OutputInterval& interval, const LocalTimePoint& submission_deadline) {
            for (auto& sample : interval) {
                LocalTimePoint tp = sample;
                auto cps = callback_fn(emitter, tp);
                for (size_t idx = 0; idx < *emitter.getControlPointCount(); ++idx) {
                    if (idx >= cps.size()) {
                        sample.controlPoint(idx).setIntensity(0.0f);
                        continue;
                    }
                    sample.controlPoint(idx).setPosition(cps[idx].getPosition());
                    sample.controlPoint(idx).setDirection(cps[idx].getDirection());
                    sample.controlPoint(idx).setIntensity(cps[idx].getIntensity());
                }
            }
        });
}
/** \endcond */
} // namespace Haptics

} // namespace Ultraleap
