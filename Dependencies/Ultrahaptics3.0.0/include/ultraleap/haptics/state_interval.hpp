#pragma once

#include <algorithm>
#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/device.hpp"
#include "ultraleap/haptics/local_time.hpp"

namespace Ultraleap
{
namespace Haptics
{

template <class StateType, class TimeType>
class StateIntervalIterator;

template <class StateType, class TimeType>
class StateInterval;

template <class StateType, class TimeType>
class StateIntervalImplementation;

/** A point representing both a time and a state, as part of a state output interval
 *
 * @tparam StateType the type of state in use */
template <class StateType, class TimeType = LocalTimePoint>
class StateIntervalPoint
{
    friend class StateIntervalIterator<StateType, TimeType>;

public:
    /** Overload of less than operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this point in time is earlier than the other */
    inline bool operator<(const StateIntervalPoint<StateType, TimeType>& rhs) const { return (time_index < rhs.time_index); }
    /** Overload of less than or equal to operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this point in time occurs earlier than or at the same time as the other */
    inline bool operator<=(const StateIntervalPoint<StateType, TimeType>& rhs) const { return (time_index <= rhs.time_index); }
    /** Overload of greater than operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this point in time comes later than the other */
    inline bool operator>(const StateIntervalPoint<StateType, TimeType>& rhs) const { return (time_index > rhs.time_index); }
    /** Overload of greater than or equal to operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if this point in time is the same or comes later than the other */
    inline bool operator>=(const StateIntervalPoint<StateType, TimeType>& rhs) const { return (time_index >= rhs.time_index); }
    /** Overload of equal to operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if the two points in time are the same */
    inline bool operator==(const StateIntervalPoint<StateType, TimeType>& rhs) const { return (time_index == rhs.time_index); }
    /** Overload of not equal to operator for timeline comparisons.
     *
     * @param rhs The right hand of the comparison
     *
     * @return True if the two points in time are not the same */
    inline bool operator!=(const StateIntervalPoint<StateType, TimeType>& rhs) const { return (time_index != rhs.time_index); }
    /** Get a pointer to the state object at this point
     *
     * @return A pointer to the state */
    StateType* state(size_t device_index) { return interval.at(time_index, device_index); }
    /** Get a pointer to the const state object at this point
     *
     * @return A pointer to the state */
    const StateType* state(size_t device_index) const { return interval.at(time_index, device_index); }
    /** Get a pointer to the state object at this point
     *
     * @return A pointer to the state */
    StateType* state(const Device& device) { return interval.at(time_index, device); }
    /** Get a pointer to the const state object at this point
     *
     * @return A pointer to the state */
    const StateType* state(const Device& device) const { return interval.at(time_index, device); }
    /** Get a constant reference to the point in time represented by this interval point
     *
     * @return The time point described by this interval point */
    TimeType time() const { return interval.time(time_index) ? *interval.time(time_index) : TimeType(); }

protected:
    /** \cond EXCLUDE_FROM_DOCS */
    /** Protected constructor for interval point.
     *
     * @param itime The time which this interval point corresponds to
     *
     * @param iinterval The interval that this interval point refers back to. */
    StateIntervalPoint(size_t index, StateInterval<StateType, TimeType>& iinterval)
        : interval(iinterval)
        , time_index(index)
    {
    }
    /** Reference to interval */
    StateInterval<StateType, TimeType>& interval;
    /** Current index */
    size_t time_index;
    /** \endcond */
};

/** An iterator used to iterate over points in a state interval
 *
 * @tparam StateType the type of state in use */
template <class StateType, class TimeType = LocalTimePoint>
class StateIntervalIterator
{
    friend class StateInterval<StateType, TimeType>;

public:
    /** Constant reference to the time point on the output interval in question. */
    typedef const StateIntervalPoint<StateType, TimeType>& const_reference;
    /** Constant pointer to the time point on the output interval in question. */
    typedef const StateIntervalPoint<StateType, TimeType>* const_pointer;
    /** Reference to the time point on the output interval in question. */
    typedef StateIntervalPoint<StateType, TimeType>& reference;
    /** Pointer to the time point on the output interval in question. */
    typedef StateIntervalPoint<StateType, TimeType>* pointer;
    /** Operator to indicate whether this iterator's current interval point is earlier in time than another iterator's current point */
    bool operator<(const StateIntervalIterator<StateType, TimeType>& other) const { return (current_point < other.current_point); }
    /** Operator to indicate whether this iterator's current interval point is earlier in time than another iterator's current point */
    bool operator!=(const StateIntervalIterator<StateType, TimeType>& other) const { return (current_point != other.current_point); }
    /** Dereference operator to access the current interval point */
    reference operator*() { return current_point; }
    /** Dereference operator to access the current interval point */
    pointer operator->() { return &current_point; }
    /** Dereference operator to access the current interval point */
    const_reference operator*() const { return current_point; }
    /** Dereference operator to access the current interval point */
    const_pointer operator->() const { return &current_point; }
    /** Returns the current interval point's time */
    TimeType time() const { return current_point.time(); }

    /** Operator to move to the next interval point */
    StateIntervalIterator<StateType, TimeType>& operator++()
    {
        // If we haven't already iterated over this, populate n+1 with n
        if (current_point.time_index >= interval.getNextCommitTimeIndex())
            interval.copyStates(current_point.time_index, current_point.time_index + 1);
        ++current_point.time_index;
        interval.getNextCommitTimeIndex() = (std::max)(interval.getNextCommitTimeIndex(), current_point.time_index);
        return *this;
    }

protected:
    /** \cond EXCLUDE_FROM_DOCS */
    /** Protected constructor. */
    StateIntervalIterator(size_t index, StateInterval<StateType, TimeType>& iinterval)
        : current_point(index, iinterval)
        , interval(iinterval)
    {
    }

    /** Protected point corresponding to the current time. */
    StateIntervalPoint<StateType, TimeType> current_point;
    /** Reference to interval */
    StateInterval<StateType, TimeType>& interval;
    /** \endcond */
};

template <class StateType, class TimeType = LocalTimePoint>
class ULH_RESEARCH_API_CLASS StateInterval
{
    friend class StateIntervalIterator<StateType, TimeType>;

public:
    using iterator = StateIntervalIterator<StateType, TimeType>;
    using const_iterator = const iterator;

    ULH_RESEARCH_API StateInterval(size_t nTimes, Device* devices, size_t nDevices);
    ULH_RESEARCH_API StateInterval(const TimeType* times, size_t nTimes, Device* devices, size_t nDevices);
    ULH_RESEARCH_API StateInterval(StateInterval&& other);
    ULH_RESEARCH_API ~StateInterval();

    ULH_RESEARCH_API StateInterval& operator=(StateInterval&& other);

    ULH_RESEARCH_API size_t deviceCount() const;
    ULH_RESEARCH_API size_t timeCount() const;

    ULH_RESEARCH_API const Device* device(size_t index) const;
    ULH_RESEARCH_API Device* device(size_t index);
    ULH_RESEARCH_API bool setDevice(size_t index, Device device);

    ULH_RESEARCH_API const TimeType* time(size_t index) const;
    ULH_RESEARCH_API bool setTime(size_t index, TimeType time);

    ULH_RESEARCH_API void reset(size_t nTimes, Device* devices, size_t nDevices);
    ULH_RESEARCH_API void resetTimes(size_t nTimes);
    ULH_RESEARCH_API void resetStates();

    ULH_RESEARCH_API bool copyStates(size_t from_time_index, size_t to_time_index);

    ULH_RESEARCH_API const StateType* at(const TimeType& time, const Device& device) const;
    ULH_RESEARCH_API const StateType* at(size_t time_index, const Device& device) const;
    ULH_RESEARCH_API const StateType* at(const TimeType& time, size_t device_index) const;
    ULH_RESEARCH_API const StateType* at(size_t time_index, size_t device_index) const;

    ULH_RESEARCH_API StateType* at(const TimeType& time, const Device& device);
    ULH_RESEARCH_API StateType* at(size_t time_index, const Device& device);
    ULH_RESEARCH_API StateType* at(const TimeType& time, size_t device_index);
    ULH_RESEARCH_API StateType* at(size_t time_index, size_t device_index);

    ULH_RESEARCH_API bool set(const TimeType& time, const Device& device, StateType&& state);
    ULH_RESEARCH_API bool set(size_t time_index, const Device& device, StateType&& state);
    ULH_RESEARCH_API bool set(const TimeType& time, size_t device_index, StateType&& state);
    ULH_RESEARCH_API bool set(size_t time_index, size_t device_index, StateType&& state);

    ULH_RESEARCH_API const StateIntervalIterator<StateType, TimeType> begin() const;
    ULH_RESEARCH_API const StateIntervalIterator<StateType, TimeType> end() const;
    ULH_RESEARCH_API StateIntervalIterator<StateType, TimeType> begin();
    ULH_RESEARCH_API StateIntervalIterator<StateType, TimeType> end();

    ULH_RESEARCH_API StateInterval<StateType, TimeType> clone() const;

    const StateIntervalImplementation<StateType, TimeType>* getImplementation() const;
    StateIntervalImplementation<StateType, TimeType>* getImplementation();

private:
    StateInterval(StateIntervalImplementation<StateType, TimeType>* iimpl);
    StateIntervalImplementation<StateType, TimeType>* impl;
    ULH_RESEARCH_API size_t& getNextCommitTimeIndex();
};

} // namespace Haptics

} // namespace Ultraleap
