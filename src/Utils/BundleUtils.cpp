#include "Utils/BundleUtils.hpp"
#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/AssetBundleCreateRequest.hpp"
#include "UnityEngine/AssetBundleRequest.hpp"
#include "logging.hpp"

namespace Qosmetics::Core::BundleUtils
{
    custom_types::Helpers::Coroutine LoadBundleFromFileAsync(std::string_view filePath, UnityEngine::AssetBundle*& out) {
        if (!fileexists(filePath)) {
            ERROR("File {} did not exist", filePath);
            out = nullptr;
            co_return;
        }

        SafePtr<UnityEngine::AssetBundleCreateRequest> req = UnityEngine::AssetBundle::LoadFromFileAsync(filePath);
        req->set_allowSceneActivation(true);
        while (!req->get_isDone())
            co_yield nullptr;

        out = req->get_assetBundle();
        co_return;
    }

    custom_types::Helpers::Coroutine LoadBundleFromZipAsync(std::string_view filePath, std::string_view fileName, UnityEngine::AssetBundle*& out) {
        std::vector<uint8_t> bytes;
        if (!ZipUtils::GetBytesFromZipFile(filePath, fileName, bytes)) {
            ERROR("Could not read bytes from zip {}", filePath);
            out = nullptr;
            co_return;
        }

        auto byteArr = ArrayW(bytes);
        co_yield custom_types::Helpers::CoroutineHelper::New(LoadBundleFromMemoryAsync(byteArr, out));
        co_return;
    }

    custom_types::Helpers::Coroutine LoadBundleFromMemoryAsync(ArrayW<uint8_t> bytes, UnityEngine::AssetBundle*& out) {
        using AssetBundle_LoadFromMemoryAsync = function_ptr_t<UnityEngine::AssetBundleCreateRequest*, ArrayW<uint8_t>, uint32_t>;
        static AssetBundle_LoadFromMemoryAsync assetBundle_LoadFromMemoryAsync = reinterpret_cast<AssetBundle_LoadFromMemoryAsync>(il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadFromMemoryAsync_Internal"));

        SafePtr<UnityEngine::AssetBundleCreateRequest> req = assetBundle_LoadFromMemoryAsync(bytes, 0);
        req->set_allowSceneActivation(true);
        while (!req->get_isDone())
            co_yield nullptr;

        out = req->get_assetBundle();
        co_return;
    }

    custom_types::Helpers::Coroutine LoadAssetFromBundleAsync(UnityEngine::AssetBundle* bundle, std::string_view name, System::Type* type, UnityEngine::Object*& out) {
        SafePtr<UnityEngine::AssetBundleRequest> req = bundle->LoadAssetAsync(name, type);

        req->set_allowSceneActivation(true);
        while (!req->get_isDone())
            co_yield nullptr;

        out = req->get_asset();
        co_return;
    }
}
