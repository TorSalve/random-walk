#pragma once

#include "ultraleap/haptics/errors.hpp"
#include "tl/expected.hpp"
#include "ultraleap/haptics/common.hpp"

namespace Ultraleap
{
namespace Haptics
{

class Error
{
public:
    Error() {}
    Error(const ErrorCode& code)
        : code_(code)
    {}

    ErrorCode code() const { return code_; }
    ErrorCode value() const { return code(); }
    const char* message() const { return get_description(code()); }

    operator ErrorCode() const { return code(); }

private:
    ErrorCode code_ = ErrorCode::NoError;
};

template <class T, class E>
class expected : public tl::expected<T, E>
{
public:
    using tl::expected<T, E>::expected;

    template <class U = T, tl::detail::enable_if_t<!std::is_void<U>::value>* = nullptr>
    constexpr const U& operator*() const&
    {
        return tl::expected<T, E>::operator*();
    }

    template <class U = T, tl::detail::enable_if_t<!std::is_void<U>::value>* = nullptr>
    TL_EXPECTED_11_CONSTEXPR U& operator*() &
    {
        return tl::expected<T, E>::operator*();
    }

    template <class U = T, tl::detail::enable_if_t<!std::is_void<U>::value>* = nullptr>
    constexpr const U operator*() const&&
    {
        return tl::expected<T, E>::operator*();
    }

    template <class U = T, tl::detail::enable_if_t<!std::is_void<U>::value>* = nullptr>
    TL_EXPECTED_11_CONSTEXPR U operator*() &&
    {
        return tl::expected<T, E>::operator*();
    }

    template <class U = T, tl::detail::enable_if_t<!std::is_void<U>::value>* = nullptr>
    TL_EXPECTED_11_CONSTEXPR const U& value() const&
    {
        return tl::expected<T, E>::value();
    }

    template <class U = T, tl::detail::enable_if_t<!std::is_void<U>::value>* = nullptr>
    TL_EXPECTED_11_CONSTEXPR U& value() &
    {
        return tl::expected<T, E>::value();
    }

    template <class U = T, tl::detail::enable_if_t<!std::is_void<U>::value>* = nullptr>
    TL_EXPECTED_11_CONSTEXPR const U value() const&&
    {
        return tl::expected<T, E>::value();
    }

    template <class U = T, tl::detail::enable_if_t<!std::is_void<U>::value>* = nullptr>
    TL_EXPECTED_11_CONSTEXPR U value() &&
    {
        return tl::expected<T, E>::value();
    }

    constexpr const E& error() const& { return tl::expected<T, E>::error(); }
    TL_EXPECTED_11_CONSTEXPR E& error() & { return tl::expected<T, E>::error(); }
    constexpr const E error() const&& { return tl::expected<T, E>::error(); }
    TL_EXPECTED_11_CONSTEXPR E error() && { return tl::expected<T, E>::error(); }
};

template <class E>
class expected<bool, E> : public tl::expected<bool, E>
{
public:
    using tl::expected<bool, E>::expected;
    // don't allow cast operator if expected type is bool
    constexpr explicit operator bool() const noexcept = delete;
};

template <typename T, typename E = Error>
using result = expected<T, E>;

template <typename E = Error>
using unexpected = tl::unexpected<E>;

template <
    typename E,
    typename = typename std::enable_if<
        !std::is_same<E, typename Ultraleap::Haptics::result<class RT, class RE>>::value>::type>
unexpected<typename std::decay<E>::type> make_unexpected(E&& e)
{
    return tl::make_unexpected(std::forward<E>(e));
}

template <typename T1, typename T2>
result<T1> result_cast(result<T2>&& r)
{
    if (r)
        return static_cast<T1>(r.value());
    else
        return make_unexpected(r.error());
}

template <typename T, typename E>
unexpected<typename std::decay<E>::type> make_unexpected(Ultraleap::Haptics::result<T, E> r)
{
    if (r.has_value())
        return make_unexpected(static_cast<E>(ErrorCode::InternalError));
    return make_unexpected(std::move(r.error()));
}

template <typename T, typename E>
bool operator==(const result<T, E>& lhs, const result<T, E>& rhs)
{
    return static_cast<expected<T, E>>(lhs) == static_cast<expected<T, E>>(rhs);
}

template <typename T, typename E>
bool operator!=(const result<T, E>& lhs, const result<T, E>& rhs)
{
    return static_cast<expected<T, E>>(lhs) != static_cast<expected<T, E>>(rhs);
}

} // namespace Haptics
} // namespace Ultraleap
