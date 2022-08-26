#pragma once

#include "../Data/Manifest.hpp"
#include "../Utils/ZipUtils.hpp"
#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/Object.hpp"
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "custom-types/shared/coroutine.hpp"
#include <concepts>
#include <type_traits>

namespace Qosmetics::Core::BundleUtils
{
    /// @brief Loads a bundle from a zip file as a coroutine
    /// @param filePath path of the zip
    /// @param fileName the filename within the zip to load as a bundle
    /// @param out reference to the assetbundle pointer variable
    /// @return coroutine
    custom_types::Helpers::Coroutine LoadBundleFromZipAsync(std::string_view filePath, std::string_view fileName, UnityEngine::AssetBundle*& out);

    /// @brief Loads a bundle from a bundle file as a coroutine
    /// @param filePath the full path to the file
    /// @param out reference to the asset bundle pointer variable
    /// @return coroutine
    custom_types::Helpers::Coroutine LoadBundleFromFileAsync(std::string_view filePath, UnityEngine::AssetBundle*& out);

    /// @brief Loads a bundle from memory bytes
    /// @param bytes the byte array
    /// @param out reference to the asset bundle pointer variable
    /// @return coroutine
    custom_types::Helpers::Coroutine LoadBundleFromMemoryAsync(ArrayW<uint8_t> bytes, UnityEngine::AssetBundle*& out);

    /// @brief loads an asset from a bundle as a coroutine
    /// @param bundle the bundle to load from
    /// @param name the name of the object to load
    /// @param out reference to the object output
    /// @return coroutine
    custom_types::Helpers::Coroutine LoadAssetFromBundleAsync(UnityEngine::AssetBundle* bundle, std::string_view name, System::Type* type, UnityEngine::Object*& out);

    template <typename T>
    concept UnityObject = std::is_convertible_v<T, UnityEngine::Object*>;

    /// @brief loads an asset from a bundle as a coroutine
    /// @tparam T the object type
    /// @param bundle the bundle to load from
    /// @param name the name of the object to load
    /// @param out reference to the object output
    /// @return coroutine
    template <UnityObject T>
    requires(!std::is_same_v<T, UnityEngine::Object*>)
        custom_types::Helpers::Coroutine LoadAssetFromBundleAsync(UnityEngine::AssetBundle* bundle, std::string_view name, T& out)
    {
        return LoadAssetFromBundleAsync(bundle, name, reinterpret_cast<System::Type*>(csTypeOf(T)), reinterpret_cast<UnityEngine::Object*&>(out));
    }
}