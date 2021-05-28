#pragma once

#include <string>
#include <utility>
#include <vector>
#include <cmath>

#include "ultraleap/haptics/common.hpp"
#include "ultraleap/haptics/streaming.hpp"
#include "ultraleap/haptics/stl_compat.hpp"

namespace Ultraleap
{
namespace Haptics
{

#ifdef __has_cpp_attribute
    #if __has_cpp_attribute(nodiscard)
        #define ULH_NODISCARD [[nodiscard]]
    #elif
        #define ULH_NODISCARD
    #endif
#else
    #define ULH_NODISCARD
#endif

/** \cond EXCLUDE_FROM_DOCS */
class SensationParameterImplementation;
/** \endcond */

/** @brief Contains information about a parameter that can be set on a SensationInstance
 */
class ULH_API_CLASS SensationParameter final
{
public:
    enum class Kind {
        Number,
        Struct
    };

    /** \cond EXCLUDE_FROM_DOCS */
    explicit SensationParameter(SensationParameterImplementation*);

    ULH_API SensationParameter(SensationParameter&& other) noexcept;
    ULH_API SensationParameter& operator=(SensationParameter&& other) noexcept;
    ULH_API SensationParameter(const SensationParameter&);
    ULH_API SensationParameter& operator=(const SensationParameter&);
    ULH_API ~SensationParameter();
    /** \endcond */

    /** The name of the Parameter */
    ULH_NODISCARD ULH_API const char* name() const;

    /** @brief The path to the Parameter
     *
     * This should be used when calling `set` on a SensationInstance
     * @see SensationInstance
     */
    ULH_NODISCARD ULH_API const char* path() const;

    /** The type of the Parameter */
    ULH_NODISCARD ULH_API const char* type() const;

    /** The kind of the Parameter
     *
     * This is either Number, meaning it represents a single float, or Struct, meaning it represents
     * a composite parameter which contains fields.
     */
    ULH_NODISCARD ULH_API Kind kind() const;

    /** The size of the Parameter
     *
     * This is the number of floats required to specify its value.
     */
    ULH_NODISCARD ULH_API size_t size() const;

    /** The default value associated with the parameter
     *
     * This can only be called on parameters with a kind Number.
     * If this is called on Structs, an ErrorCode::InvalidOperation is returned.
     */
    ULH_NODISCARD ULH_API result<float> defaultValue() const;

    /** Get the named field of the parameter
     *
     * This parameter must be a Struct. Calling this on a Number returns an error.
     *
     * @param relative_path The path of the parameter relative to this parameter.
     */
    ULH_NODISCARD result<SensationParameter> field(const std::string& relative_path) const
    {
        return field(relative_path.c_str());
    }
    ULH_NODISCARD ULH_API result<SensationParameter> field(const char* relative_path) const;

    /** Get all fields of the parameter */
    ULH_NODISCARD std::vector<SensationParameter> fields() const
    {
        return _fields();
    }

    /** Apply a function to all nested Number parameters
     *
     * If this parameter is a Number, apply the function to this parameter only.
     * If this parameter is a Struct, recursively check all fields of this Struct and apply this function
     * to all Numbers which are contained.
     *
     * @param func The function to apply
     */
    void walkNumbers(const std::function<void(const char*, const SensationParameter&)>& func) const
    {
        switch (kind()) {
        case Kind::Number:
            return func(path(), *this);
        case Kind::Struct:
            for (const auto& param : fields())
                param.walkNumbers(func);
            break;
        default:
            break;
        }
    }

protected:
    SensationParameterImplementation* impl;

    ULH_API uh::vector_wrapper<SensationParameter> _fields() const;
};

/** \cond EXCLUDE_FROM_DOCS */
class SensationImplementation;
/** \endcond */

/** @brief Contains information about a playable sensation
 *
 * This cannot be directly played on a SensationEmitter as it represents the sensation itself,
 * but does not include any arguments to the sensation. To play a Sensation on a SensationEmitter,
 * first construct a SensationInstance.
 * @see SensationInstance
 */
class ULH_API_CLASS Sensation final
{
public:
    /** \cond EXCLUDE_FROM_DOCS */
    Sensation(SensationImplementation* impl);

    ULH_API Sensation(const Sensation&);
    ULH_API Sensation(Sensation&&) noexcept;
    ULH_API Sensation& operator=(const Sensation&);
    ULH_API Sensation& operator=(Sensation&&) noexcept;
    ULH_API ~Sensation();
    /** \endcond */

    /** Get the name of the sensation */
    ULH_NODISCARD ULH_API const char* name() const;

    /** Get the information about the parameters that can be set on an instance of this Sensation
     *
     * @see SensationInstance
     **/
    ULH_NODISCARD std::vector<SensationParameter> parameters() const
    {
        return _parameters();
    }

    /** @brief Get the parameter at the specified path
     *
     * @param argument_path The path to the parameter
     */
    ULH_NODISCARD ULH_API result<SensationParameter> getParameter(const char* argument_path) const;
    ULH_NODISCARD result<SensationParameter> getParameter(const std::string& argument_path) const
    {
        return getParameter(argument_path.c_str());
    }

    /** \cond EXCLUDE_FROM_DOCS */
    SensationImplementation* getImplementation();
    const SensationImplementation* getImplementation() const;
    /** \endcond */

private:
    /** \cond EXCLUDE_FROM_DOCS */
    ULH_NODISCARD ULH_API uh::vector_wrapper<SensationParameter> _parameters() const;

    SensationImplementation* impl;
    /** \endcond */
};

/** \cond EXCLUDE_FROM_DOCS */
class SensationInstanceImplementation;
/** \endcond */

/** @brief A playable instance of a Sensation
 *
 * This is composed of a Sensation, and arguments for each available parameter.
 *
 * @see SensationEmitter
 */
class ULH_API_CLASS SensationInstance final
{
public:
    /** @brief Create a SensationInstance from a Sensation
     *
     * This is created with the default arguments associated with the Sensation
     *
     * @param sensation The Sensation to create an instance of
     */
    ULH_API explicit SensationInstance(const Sensation& sensation);

    /** \cond EXCLUDE_FROM_DOCS */
    ULH_API SensationInstance(const SensationInstance& other);
    ULH_API SensationInstance(SensationInstance&& other) noexcept;
    ULH_API SensationInstance& operator=(SensationInstance&& other) noexcept;
    ULH_API SensationInstance& operator=(const SensationInstance& other) noexcept;
    ULH_API ~SensationInstance();
    /** \endcond */

    /** @brief Get the Sensation which this is an instance of */
    ULH_NODISCARD ULH_API const Sensation& sensation() const;

    /** @brief Get the current value of the argument at the specified path
     *
     * @param argument_path The path to the argument
     */
    ULH_NODISCARD ULH_API result<float> get(const char* argument_path) const;
    ULH_NODISCARD result<float> get(const std::string& argument_path) const
    {
        return get(argument_path.c_str());
    }

    /** @brief Set the value of the numeric argument at the specified path
     *
     * This sets an argument with a single numeric value - the argument_path must
     * correspond to a parameter which has a SensationParameter::Kind of Number.
     *
     * @param argument_path The path to the argument
     * @param value The value to set
     */
    ULH_API result<void> set(const char* argument_path, float value);
    result<void> set(const std::string& argument_path, float value)
    {
        return set(argument_path.c_str(), value);
    }

    /** @brief Set the value of an argument at the specified path
     *
     * This sets one or more values to the argument pointed to by argument_path.
     * If argument_path corresponds to a parameter of kind Struct, the length of the provided values
     * must match the total length of the parameter (the count of Number parameters within it).
     * If argument_path corresponds to a parameter of kind Number, the length must be 1.
     *
     * @param argument_path The path to the parameter to set
     * @param values The values associated with the parameter
     * @param values_len The length of the values array
     */
    ULH_API result<void> set(const char* argument_path, const float* values, size_t values_len);
    result<void> set(const std::string& argument_path, const float* values, size_t values_len)
    {
        return set(argument_path.c_str(), values, values_len);
    }
    result<void> set(const char* argument_path, const std::vector<float>& values)
    {
        return set(argument_path, values.data(), values.size());
    }
    result<void> set(const std::string& argument_path, const std::vector<float>& values)
    {
        return set(argument_path.c_str(), values.data(), values.size());
    }

    /** \cond EXCLUDE_FROM_DOCS */
    ULH_NODISCARD SensationInstanceImplementation* getImplementation() const;
    /** \endcond */

private:
    /** \cond EXCLUDE_FROM_DOCS */
    SensationInstanceImplementation* impl;
    /** \endcond */
};

/** \cond EXCLUDE_FROM_DOCS */
class SensationPackageImplementation;
/** \endcond */

/** @brief A Package containing definitions of Sensations
 *
 * This can be loaded from a file, directory or memory.
 * Provides access to Sensation objects contained in the package.
 */
class ULH_API_CLASS SensationPackage final
{
public:
    /** @brief Load the package from a file
     *
     * This loads a Spatial Sensation Package ('.ssp') file.
     *
     * @param lib The Library to use to load the package
     * @param path The path to the package file.
     */
    ULH_API static result<SensationPackage> loadFromFile(Library lib, const char* path);
    static result<SensationPackage> loadFromFile(Library lib, const std::string& path)
    {
        return loadFromFile(std::move(lib), path.c_str());
    }

    /** @brief Load the package from a directory
     *
     * This loads a package from a directory which obeys the Spatial Sensation Package structure.
     *
     * @param lib The Library to use to load the package
     * @param path The path to the directory to load
     */
    ULH_API static result<SensationPackage> loadFromDirectory(Library lib, const char* path);
    static result<SensationPackage> loadFromDirectory(Library lib, const std::string& path)
    {
        return loadFromDirectory(std::move(lib), path.c_str());
    }

    /** @brief Load the package from Spatial Sensation Package ('.ssp') data in memory
     *
     * @param lib The Library to use to load the package
     * @param buffer The buffer to load the package from
     * @param len The buffer length
     * @param sourcename The name which should be associated with the package
     */
    ULH_API static result<SensationPackage> loadFromMemory(Library lib, const char* buffer, size_t len, const char* sourcename);
    static result<SensationPackage> loadFromMemory(Library lib, const char* buffer, size_t len, const std::string& sourcename)
    {
        return loadFromMemory(std::move(lib), buffer, len, sourcename.c_str());
    }

    /** \cond EXCLUDE_FROM_DOCS */
    SensationPackage(SensationPackageImplementation*);

    ULH_API SensationPackage(const SensationPackage& other);
    ULH_API SensationPackage(SensationPackage&& other) noexcept;
    ULH_API SensationPackage& operator=(SensationPackage&& other) noexcept;
    ULH_API SensationPackage& operator=(const SensationPackage& other) noexcept;
    ULH_API ~SensationPackage();
    /** \endcond */

    /** The name of the package */
    ULH_NODISCARD ULH_API const char* name() const;

    /** The author of the package */
    ULH_NODISCARD ULH_API const char* author() const;

    /** The description of the package */
    ULH_NODISCARD ULH_API const char* description() const;

    /** The version of the package */
    ULH_NODISCARD ULH_API const char* version() const;

    /** @brief Get a Sensation from the package
     *
     * Get the named sensation from the package if it is defined within it.
     * @param name The name of the sensation to acquire
     */
    ULH_NODISCARD ULH_API result<Sensation> sensation(const char* name) const;
    ULH_NODISCARD result<Sensation> sensation(const std::string& name) const
    {
        return sensation(name.c_str());
    }

    /** Get all available Sensations */
    ULH_NODISCARD std::vector<Sensation> sensations() const
    {
        return _sensations();
    }

private:
    /** \cond EXCLUDE_FROM_DOCS */
    ULH_NODISCARD ULH_API uh::vector_wrapper<Sensation> _sensations() const;

    SensationPackageImplementation* impl;
    /** \endcond */
};

enum class ExecutionMode {
    PreferDevice,
    RequireDevice,
    PreferHost,
    RequireHost
};

/** \brief Emit haptic points using a sensation.
 *
 * This emitter allows playback of Sensations.
 * @see Sensation
 *
 * This class is movable.
 *
 * For examples of how to use a SensationEmitter see the Sensation examples */
class ULH_API_CLASS SensationEmitter final : public Emitter
{
public:
    /** Constructs a sensation emitter using an existing library.
     *
     * @param ulh_lib A library with which to construct this emitter.
     * @param mode The mode in which the sensation should run. This can be run in the host or on the device.
     *
     * */
    explicit ULH_API SensationEmitter(Library ulh_lib, ExecutionMode mode = ExecutionMode::PreferDevice);

    /** \cond EXCLUDE_FROM_DOCS */
    /** Move constructor. */
    ULH_API SensationEmitter(SensationEmitter&& other) noexcept;
    /** Move assignment operator. */
    ULH_API SensationEmitter& operator=(SensationEmitter&& other) noexcept;
    /** Deleted copy constructor. */
    ULH_API SensationEmitter(const SensationEmitter& other) = delete;
    /** Deleted copy assignment operator. */
    ULH_API SensationEmitter& operator=(const SensationEmitter& other) = delete;

    /** Destroys this emitter and all of its resources. */
    ULH_API virtual ~SensationEmitter();
    /** \endcond */

    /** @brief Set sensation on the emitter
      *
      * The sensation will continue to be emitted until a new sensation is set.
      *
      * The optional 'reset_time' parameter can be used to determine whether the sensation
      * should play from the beginning, or play from the current time. In simple situations
      * the default value 'true' should be sufficient.
      *
      * @param instance The SensationInstance to play
      * @param reset_time If true, the sensation will be played from the beginning. If false, the
      * sensation will play from the current time.
      */
    ULH_NODISCARD ULH_API result<void> setSensation(const SensationInstance& instance, bool reset_time = true);

    /** @brief Update the currently set sensation with new arguments for its parameters
     *
     * Update the sensation which is currently set with new values for parameters. The instance
     * must be an instance of the currently set sensation. If it is not, an error is returned.
     *
     * @param instance The SensationInstance to play. This must be an instance of the the currently set sensation.
     * */
    ULH_NODISCARD ULH_API result<void> updateSensationArguments(const SensationInstance& instance);

    /** @brief Clear the sensation on the emitter.
     *
     * After calling this function, the emitter will not have a sensation set on it and will
     * cease outputting until a new sensation instance is assigned.
     */
    ULH_NODISCARD ULH_API result<void> clearSensation();

    /** Start sensation playback on this emitter */
    ULH_NODISCARD ULH_API result<void> start();

    /** Set the execution mode */
    ULH_NODISCARD ULH_API result<void> setExecutionMode(ExecutionMode mode);

    /** Get the execution mode */
    ULH_NODISCARD ULH_API result<ExecutionMode> getExecutionMode() const;

    /** Get the time used when evaluating the sensation */
    ULH_NODISCARD ULH_API result<double> getCurrentSensationTime() const;

    /** Get whether the emitter is paused */
    ULH_NODISCARD ULH_API result<bool> isPaused() const;
};

} // namespace Haptics

} // namespace Ultraleap
