#pragma once
#include <cstdint>

namespace Ultraleap {
namespace Haptics {
    /** Log level for a message */
    enum class LogLevel : uint32_t
    {
        /** A fatal error indicating that further operation, if possible, will be unpredictable */
        Fatal = 0,
        /** An error which may impact operation of the system */
        Error = 1,
        /** A warning which might indicate suboptimal operation of the system */
        Warning = 2,
        /** Informational output */
        Verbose = 3,
        /** Very verbose informational output */
        ExtraVerbose = 4,
        /** Trace output used for debugging purposes */
        Debug = 5,
    };

    inline const char* get_description(const LogLevel n)
    {
        switch (n)
        {
        case LogLevel::Fatal: return "A fatal error indicating that further operation, if possible, will be unpredictable";
        case LogLevel::Error: return "An error which may impact operation of the system";
        case LogLevel::Warning: return "A warning which might indicate suboptimal operation of the system";
        case LogLevel::Verbose: return "Informational output";
        case LogLevel::ExtraVerbose: return "Very verbose informational output";
        case LogLevel::Debug: return "Trace output used for debugging purposes";
        default: return "";
        }
    }
    /** Error codes for specific error conditions */
    enum class ErrorCode : uint32_t
    {
        /** Operation completed successfully */
        NoError = 0x00000000,
        /** No additional information is available about the error */
        UnknownError = 0x00000001,
        /** The error was related to internal operation of the system */
        InternalError = 0x00000002,
        /** The SDK failed to access a file, likely due to non-existence or permission issues */
        FileAccessError = 0x00000003,
        /** The operation attempted cannot be performed in this library mode */
        InvalidLibraryMode = 0x00000004,
        /** The message sent to/from the daemon was invalid */
        InvalidMessage = 0x00000005,
        /** The library is not currently connected */
        LibraryNotConnected = 0x00000006,
        /** The handle provided with a request was not valid */
        InvalidHandle = 0x00000007,
        /** The argument provided was not valid */
        InvalidArgument = 0x00000008,
        /** The operation attempted was not valid */
        InvalidOperation = 0x00000009,
        /** The argument provided was out of range */
        ArgumentOutOfRange = 0x0000000A,
        /** The versions of the headers used to build and the library binary being run are not the same */
        LibraryVersionMismatch = 0x0000000B,
        /** The versions of the headers used to build and the library binary being run are known to be incompatible */
        LibraryVersionIncompatible = 0x0000000C,
        /** The library used to communicate with the Ultraleap Haptics hardware could not be loaded */
        HardwareLibraryUnavailable = 0x0000000D,
        /** There are no results that match the specified requirements */
        NoMatches = 0x0000000E,
        /** User-provided code indicated failure */
        UserCodeFailed = 0x0000000F,
        /** The operation is redundant and resulted in either a no-op or unnecessary computation */
        RedundantOperation = 0x00000010,
        /** An operation involving a device was requested, but was unable to find a device to use */
        DeviceUnavailable = 0x00000100,
        /** The requested device is in DFU mode, and as such cannot be used by the SDK until restarted */
        DeviceInDFUMode = 0x00000101,
        /** The requested device is using a firmware version not supported by this SDK */
        DeviceFirmwareVersionInvalid = 0x00000102,
        /** The requested device could not be fully initialised */
        DeviceFailedToInitialise = 0x00000103,
        /** The requested device could not be claimed, possibly indicating it is in use by another process */
        DeviceFailedToClaim = 0x00000104,
        /** The specified device does not have a claim */
        DeviceDoesNotHaveClaim = 0x00000105,
        /** The specified device has already been claimed by this process */
        DeviceAlreadyClaimed = 0x00000106,
        /** The available device does not support the capabilities requested */
        DeviceCapabilitiesMismatch = 0x00000107,
        /** A device was not able to estimate the host-to-device or device-to-host latency */
        DeviceFailedToEstimateLatency = 0x00000108,
        /** The device may not be used yet as it is in the process of initialising */
        DeviceInitialisationInProgress = 0x00000109,
        /** The Device object being accessed is not in a valid state */
        DeviceNotBound = 0x0000010A,
        /** A device did not understand a query sent to it */
        DeviceUnknownQuery = 0x0000010B,
        /** The available device does not support the feature requested */
        DeviceFeatureUnsupported = 0x0000010C,
        /** The device's configuration was invalid */
        DeviceConfigError = 0x0000010D,
        /** The requested operation requires a known device model, and this device's model is unknown */
        DeviceModelUnknown = 0x0000010E,
        /** This emitter is in an invalid state for the requested operation (may be disconnected) */
        EmitterInvalid = 0x00000200,
        /** This emitter could not initialise properly */
        EmitterInitialisationFailed = 0x00000201,
        /** Emitter failed to send an update to the device; frequent occurrences of this message may result in dropouts */
        EmitterFailedToSendUpdate = 0x00000203,
        /** The emitter was requested to stop but was unable to do so */
        EmitterFailedToStop = 0x00000204,
        /** An attempt was made to modify a property of an emitter which may only be changed while the emitter is stopped */
        EmitterCannotModifyWhileRunning = 0x00000205,
        /** The operation could not be completed because the emitter has no devices attached */
        EmitterHasNoDevices = 0x00000206,
        /** The operation could not be completed because all devices attached to the emitter are unavailable */
        EmitterHasNoAvailableDevices = 0x00000207,
        /** The device's configuration could not be changed in a way that matched all provided constraints */
        EmitterDeviceConfigInvalid = 0x00000208,
        /** The emitter does not contain the specified device */
        EmitterDoesNotContainDevice = 0x00000209,
        /** The emitter already contains the specified device */
        EmitterAlreadyContainsDevice = 0x0000020A,
        /** The emitter is disabled and cannot be queried or modified */
        EmitterDisabled = 0x0000020B,
        /** The emitter can only perform this operation while running */
        EmitterNotRunning = 0x0000020C,
        /** An error was encountered when communicating with the device over USB */
        DeviceCommunicationError = 0x00000400,
        /** The requested control point configuration included points closer together than permitted */
        ControlPointsTooCloseTogether = 0x00000500,
        /** The requested control point configuration included more control points than permitted */
        CannotExceedControlPointLimit = 0x00000501,
        /** The emitter's user callback took too long to run, resulting in data being unavailable to send to the device */
        EmitterCallbackTooSlow = 0x00000502,
        /** The requested update rate is not valid for the current configuration */
        DeviceInvalidUpdateRate = 0x00000503,
        /** The control point data provided to the device was invalid */
        InvalidControlPointData = 0x00000504,
        /** An error was encountered when communicating with the backend */
        NetworkCommunicationError = 0x00000600,
        /** A timeout occurred when communicating with the backend */
        NetworkCommunicationTimeout = 0x00000601,
        /** An error was encountered when loading a sensation archive */
        SensationArchiveLoadError = 0x00000700,
        /** An error was encountered when loading a specific sensation from an archive */
        SensationLoadError = 0x00000701,
        /** An error was encountered with an argument provided for a sensation */
        SensationArgumentError = 0x00000702,
    };

    inline const char* get_description(const ErrorCode n)
    {
        switch (n)
        {
        case ErrorCode::NoError: return "Operation completed successfully";
        case ErrorCode::UnknownError: return "No additional information is available about the error";
        case ErrorCode::InternalError: return "The error was related to internal operation of the system";
        case ErrorCode::FileAccessError: return "The SDK failed to access a file, likely due to non-existence or permission issues";
        case ErrorCode::InvalidLibraryMode: return "The operation attempted cannot be performed in this library mode";
        case ErrorCode::InvalidMessage: return "The message sent to/from the daemon was invalid";
        case ErrorCode::LibraryNotConnected: return "The library is not currently connected";
        case ErrorCode::InvalidHandle: return "The handle provided with a request was not valid";
        case ErrorCode::InvalidArgument: return "The argument provided was not valid";
        case ErrorCode::InvalidOperation: return "The operation attempted was not valid";
        case ErrorCode::ArgumentOutOfRange: return "The argument provided was out of range";
        case ErrorCode::LibraryVersionMismatch: return "The versions of the headers used to build and the library binary being run are not the same";
        case ErrorCode::LibraryVersionIncompatible: return "The versions of the headers used to build and the library binary being run are known to be incompatible";
        case ErrorCode::HardwareLibraryUnavailable: return "The library used to communicate with the Ultraleap Haptics hardware could not be loaded";
        case ErrorCode::NoMatches: return "There are no results that match the specified requirements";
        case ErrorCode::UserCodeFailed: return "User-provided code indicated failure";
        case ErrorCode::RedundantOperation: return "The operation is redundant and resulted in either a no-op or unnecessary computation";
        case ErrorCode::DeviceUnavailable: return "An operation involving a device was requested, but was unable to find a device to use";
        case ErrorCode::DeviceInDFUMode: return "The requested device is in DFU mode, and as such cannot be used by the SDK until restarted";
        case ErrorCode::DeviceFirmwareVersionInvalid: return "The requested device is using a firmware version not supported by this SDK";
        case ErrorCode::DeviceFailedToInitialise: return "The requested device could not be fully initialised";
        case ErrorCode::DeviceFailedToClaim: return "The requested device could not be claimed, possibly indicating it is in use by another process";
        case ErrorCode::DeviceDoesNotHaveClaim: return "The specified device does not have a claim";
        case ErrorCode::DeviceAlreadyClaimed: return "The specified device has already been claimed by this process";
        case ErrorCode::DeviceCapabilitiesMismatch: return "The available device does not support the capabilities requested";
        case ErrorCode::DeviceFailedToEstimateLatency: return "A device was not able to estimate the host-to-device or device-to-host latency";
        case ErrorCode::DeviceInitialisationInProgress: return "The device may not be used yet as it is in the process of initialising";
        case ErrorCode::DeviceNotBound: return "The Device object being accessed is not in a valid state";
        case ErrorCode::DeviceUnknownQuery: return "A device did not understand a query sent to it";
        case ErrorCode::DeviceFeatureUnsupported: return "The available device does not support the feature requested";
        case ErrorCode::DeviceConfigError: return "The device's configuration was invalid";
        case ErrorCode::DeviceModelUnknown: return "The requested operation requires a known device model, and this device's model is unknown";
        case ErrorCode::EmitterInvalid: return "This emitter is in an invalid state for the requested operation (may be disconnected)";
        case ErrorCode::EmitterInitialisationFailed: return "This emitter could not initialise properly";
        case ErrorCode::EmitterFailedToSendUpdate: return "Emitter failed to send an update to the device; frequent occurrences of this message may result in dropouts";
        case ErrorCode::EmitterFailedToStop: return "The emitter was requested to stop but was unable to do so";
        case ErrorCode::EmitterCannotModifyWhileRunning: return "An attempt was made to modify a property of an emitter which may only be changed while the emitter is stopped";
        case ErrorCode::EmitterHasNoDevices: return "The operation could not be completed because the emitter has no devices attached";
        case ErrorCode::EmitterHasNoAvailableDevices: return "The operation could not be completed because all devices attached to the emitter are unavailable";
        case ErrorCode::EmitterDeviceConfigInvalid: return "The device's configuration could not be changed in a way that matched all provided constraints";
        case ErrorCode::EmitterDoesNotContainDevice: return "The emitter does not contain the specified device";
        case ErrorCode::EmitterAlreadyContainsDevice: return "The emitter already contains the specified device";
        case ErrorCode::EmitterDisabled: return "The emitter is disabled and cannot be queried or modified";
        case ErrorCode::EmitterNotRunning: return "The emitter can only perform this operation while running";
        case ErrorCode::DeviceCommunicationError: return "An error was encountered when communicating with the device over USB";
        case ErrorCode::ControlPointsTooCloseTogether: return "The requested control point configuration included points closer together than permitted";
        case ErrorCode::CannotExceedControlPointLimit: return "The requested control point configuration included more control points than permitted";
        case ErrorCode::EmitterCallbackTooSlow: return "The emitter's user callback took too long to run, resulting in data being unavailable to send to the device";
        case ErrorCode::DeviceInvalidUpdateRate: return "The requested update rate is not valid for the current configuration";
        case ErrorCode::InvalidControlPointData: return "The control point data provided to the device was invalid";
        case ErrorCode::NetworkCommunicationError: return "An error was encountered when communicating with the backend";
        case ErrorCode::NetworkCommunicationTimeout: return "A timeout occurred when communicating with the backend";
        case ErrorCode::SensationArchiveLoadError: return "An error was encountered when loading a sensation archive";
        case ErrorCode::SensationLoadError: return "An error was encountered when loading a specific sensation from an archive";
        case ErrorCode::SensationArgumentError: return "An error was encountered with an argument provided for a sensation";
        default: return "";
        }
    }
    /** Hardware error codes */
    enum class DeviceError : uint32_t
    {
        /** A core device component has failed to boot */
        BootFailure = (1 << 0),
        /** A heartbeat signal has not been received in the required time */
        DeviceUnresponsive = (1 << 1),
        /** The maximum operating temperature has been exceeded */
        Overheat = (1 << 2),
        /** Emission data was supplied at an insufficient rate */
        EmissionDataUnderflow = (1 << 3),
        /** Emission data was supplied at an excessive rate */
        EmissionDataOverflow = (1 << 4),
        /** One or more temperature sensors has become unresponsive */
        TemperatureSensorFailure = (1 << 5),
        /** Unexpected voltage within operational threshold */
        VoltageWarning = (1 << 6),
        /** Unexpected voltage outside operational threshold */
        VoltageFault = (1 << 12),
        /** No additional information is available about the error */
        UnknownError = (1 << 18),
    };

    inline const char* get_description(const DeviceError n)
    {
        switch (n)
        {
        case DeviceError::BootFailure: return "A core device component has failed to boot";
        case DeviceError::DeviceUnresponsive: return "A heartbeat signal has not been received in the required time";
        case DeviceError::Overheat: return "The maximum operating temperature has been exceeded";
        case DeviceError::EmissionDataUnderflow: return "Emission data was supplied at an insufficient rate";
        case DeviceError::EmissionDataOverflow: return "Emission data was supplied at an excessive rate";
        case DeviceError::TemperatureSensorFailure: return "One or more temperature sensors has become unresponsive";
        case DeviceError::VoltageWarning: return "Unexpected voltage within operational threshold";
        case DeviceError::VoltageFault: return "Unexpected voltage outside operational threshold";
        case DeviceError::UnknownError: return "No additional information is available about the error";
        default: return "";
        }
    }


}
}
