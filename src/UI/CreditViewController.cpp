#include "UI/CreditViewController.hpp"
#include "Data/Patrons.hpp"
#include "Utils/DateUtils.hpp"
#include "Utils/RainbowUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "diglett/shared/Localization.hpp"
#include "diglett/shared/Util.hpp"
#include "logging.hpp"
#include "questui/shared/ArrayUtil.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "static-defines.hpp"

#include "GlobalNamespace/SharedCoroutineStarter.hpp"

#include "HMUI/Touchable.hpp"

#include "UnityEngine/Application.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Material.hpp"
#include "UnityEngine/Networking/DownloadHandler.hpp"
#include "UnityEngine/Networking/UnityWebRequest.hpp"
#include "UnityEngine/Networking/UnityWebRequestAsyncOperation.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Resources.hpp"
#include "UnityEngine/Shader.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/ContentSizeFitter.hpp"

DEFINE_TYPE(Qosmetics::Core, CreditViewController);

using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;

HorizontalLayoutGroup* make_patron_display(UnityEngine::Transform* parent, std::vector<std::string>& names, StringW title, UnityEngine::Color headercolor)
{
    auto banner = CreateHorizontalLayoutGroup(parent->get_transform());
    auto bannerText = CreateText(banner->get_transform(), title);
    auto bannerBG = banner->get_gameObject()->AddComponent<QuestUI::Backgroundable*>();
    bannerBG->ApplyBackgroundWithAlpha("title-gradient", 1.0f);
    bannerText->set_alignment(TMPro::TextAlignmentOptions::_get_Center());

    auto contentSizeFitter = banner->get_gameObject()->GetComponent<ContentSizeFitter*>();
    if (!contentSizeFitter)
        contentSizeFitter = banner->get_gameObject()->AddComponent<ContentSizeFitter*>();
    contentSizeFitter->set_horizontalFit(ContentSizeFitter::FitMode::Unconstrained);

    auto layoutElement = bannerText->get_gameObject()->GetComponent<LayoutElement*>();
    if (!layoutElement)
        layoutElement = banner->get_gameObject()->AddComponent<LayoutElement*>();
    layoutElement->set_preferredWidth(50.0f);
    layoutElement->set_preferredHeight(5.0f);

    HMUI::ImageView* imageView =
        bannerBG->get_gameObject()->GetComponentInChildren<HMUI::ImageView*>();
    imageView->gradient = true;
    imageView->gradientDirection = 1;
    imageView->set_color(headercolor);
    imageView->set_color0(Color(1.1f, 1.1f, 1.1f, 1.0f));
    imageView->set_color1(Color(0.9f, 0.9f, 0.9f, 1.0f));

    for (auto& n : names)
    {
        auto t = CreateText(parent, n);
        t->set_alignment(TMPro::TextAlignmentOptions::_get_Center());
    }

    return banner;
}

#define SETUP_WRAPPER()                                                             \
    auto horizontal = CreateHorizontalLayoutGroup(container->get_transform());      \
    auto vertical = CreateVerticalLayoutGroup(horizontal->get_transform());         \
    auto bg = vertical->get_gameObject()->AddComponent<QuestUI::Backgroundable*>(); \
    bg->ApplyBackgroundWithAlpha("title-gradient", 0.5f);                           \
    HMUI::ImageView* imageView =                                                    \
        bg->get_gameObject()->GetComponentInChildren<HMUI::ImageView*>();           \
    imageView->gradient = false;                                                    \
    imageView->gradientDirection = 1;                                               \
    imageView->set_color({0, 0, 0, 0.5f})

namespace Qosmetics::Core
{
    void CreditViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            auto localization = Diglett::Localization::get_instance();
            get_gameObject()->AddComponent<HMUI::Touchable*>();
            container = CreateScrollableSettingsContainer(get_transform());
            ExternalComponents* components = container->GetComponent<ExternalComponents*>();
            RectTransform* rect = components->Get<RectTransform*>();
            rect->set_sizeDelta({0.0f, 0.0f});

            auto vertical = container->GetComponentInChildren<VerticalLayoutGroup*>();
            vertical->set_spacing(5.0f);

            if (DateUtils::isMonth(6))
            {
                UIUtils::AddHeader(get_transform(), RainbowUtils::gayify(static_cast<std::string>(localization->get("QosmeticsCore:Credit:PatreonHeader"))), qosmetics_purple);
            }
            else
            {
                UIUtils::AddHeader(get_transform(), localization->get("QosmeticsCore:Credit:PatreonHeader"), qosmetics_purple);
            }

            auto explHorizontal = CreateHorizontalLayoutGroup(container->get_transform());
            auto explVertical = CreateVerticalLayoutGroup(explHorizontal->get_transform());

            auto mainText = CreateText(explVertical->get_transform(), u"<color=#ff4040><size=6><b>" + localization->get("QosmeticsCore:Credit:PatreonMessageHeader") + u"</b></size></color>\n<size=3>" + localization->get("QosmeticsCore:Credit:PatreonMessageBody") + u"</size>");
            mainText->get_gameObject()->AddComponent<LayoutElement*>()->set_preferredHeight(40.f);
            mainText->set_alignment(TMPro::TextAlignmentOptions::_get_Center());

            auto patreonText = CreateClickableText(explVertical->get_transform(), "Patreon.com/Qosmetics", true, []()
                                                   { Application::OpenURL(patreon_url); });
            patreonText->set_alignment(TMPro::TextAlignmentOptions::_get_Center());
            patreonText->set_defaultColor(Color(1.0f, 0.25f, 0.25f, 1.0f));
            patreonText->set_highlightColor(Color(1.0f, 0.5f, 0.5f, 1.0f));

            AddHoverHint(patreonText->get_gameObject(), localization->get("QosmeticsCore:Credit:OpenToBrowser"));

            auto patronParent = CreateHorizontalLayoutGroup(container->get_transform());
            patronTexts = CreateVerticalLayoutGroup(patronParent->get_transform());
            auto placeholderText = CreateText(patronTexts->get_transform(), localization->get("QosmeticsCore:Credit:Fetching"));

            StartCoroutine(custom_types::Helpers::CoroutineHelper::New(GetPatreonSupporters()));
        }
    }

    custom_types::Helpers::Coroutine CreditViewController::GetPatreonSupporters()
    {
        UnityEngine::Networking::UnityWebRequest* www = UnityEngine::Networking::UnityWebRequest::Get(patron_url);
        auto req = www->SendWebRequest();
        co_yield reinterpret_cast<System::Collections::IEnumerator*>(req);

        bool isHttpError = www->get_isHttpError();
        bool isNetworkError = www->get_isNetworkError();

        auto localization = Diglett::Localization::get_instance();

        if (isHttpError || isNetworkError)
        {
            ERROR("Failed to fetch patrons file from resources repository");
            ERROR("Was http error: {}", isHttpError);
            ERROR("Was network error: {}", isNetworkError);

            auto patronParent = CreateHorizontalLayoutGroup(container->get_transform());
            patronTexts = CreateVerticalLayoutGroup(patronParent->get_transform());
            auto placeholderText = CreateText(patronTexts->get_transform(), localization->get("QosmeticsCore:Credit:Failed"));

            co_return;
        }

        auto downloadHandler = www->get_downloadHandler();
        auto patrons = Qosmetics::Core::Patrons::Parse(static_cast<std::string>(downloadHandler->get_text()));

        auto patronTextsT = patronTexts->get_transform()->get_parent();
        Object::DestroyImmediate(patronTextsT->get_gameObject());

        if (!patrons.any())
        {
            auto patronParent = CreateHorizontalLayoutGroup(container->get_transform());
            patronTexts = CreateVerticalLayoutGroup(patronParent->get_transform());
            auto placeholderText = CreateText(patronTexts->get_transform(), localization->get("QosmeticsCore:Credit:Missing"));
        }
        else
        {
            if (patrons.legendary.size() > 0)
            {
                SETUP_WRAPPER();
                StringW patreontext = u"<color=#000000>" + localization->get("QosmeticsCore:Credit:Legendary") + u"</color> <color=#222222><size=2>(" + localization->get("QosmeticsCore:Credit:Tier4") + u")</size></color>";
                auto banner = make_patron_display(vertical->get_transform(), patrons.legendary, patreontext, Color(0.9f, 0.75f, 0.25f, 1.0f));
                auto bannerimageView = banner->get_gameObject()->GetComponentInChildren<HMUI::ImageView*>();
                auto origMat = UnityEngine::Resources::FindObjectsOfTypeAll<UnityEngine::Material*>().FirstOrDefault(
                    [](UnityEngine::Material* x)
                    {
                        return x->get_name() == "AnimatedButton";
                    });
                auto dupe = Object::Instantiate(origMat);
                dupe->SetColor(Shader::PropertyToID("_ShineColor"), Color(0.9f, 0.75f, 0.25f, 1.0f));
                bannerimageView->set_material(dupe);
            }

            if (patrons.amazing.size() > 0)
            {
                SETUP_WRAPPER();
                StringW patreontext = u"<color=#000000>" + localization->get("QosmeticsCore:Credit:Amazing") + u"</color> <color=#222222><size=2>(" + localization->get("QosmeticsCore:Credit:Tier3") + u")</size></color>";
                make_patron_display(vertical->get_transform(), patrons.amazing, patreontext, Color(0.4f, 0.45f, 0.8f, 1.0f));
            }

            if (patrons.enthusiastic.size() > 0)
            {
                SETUP_WRAPPER();
                StringW patreontext = u"<color=#000000>" + localization->get("QosmeticsCore:Credit:Enthusiastic") + u"</color> <color=#222222><size=2>(" + localization->get("QosmeticsCore:Credit:Tier2") + u")</size></color>";
                make_patron_display(vertical->get_transform(), patrons.enthusiastic, patreontext, Color(0.5f, 0.55f, 0.9f, 1.0f));
            }

            if (patrons.paypal.size() > 0)
            {
                SETUP_WRAPPER();
                StringW patreontext = u"<color=#000000>" + localization->get("QosmeticsCore:Credit:Paypal") + u"</color>";
                make_patron_display(vertical->get_transform(), patrons.paypal, patreontext, Color(0.0f, 0.6f, 0.85f, 1.0f));
            }
        }
        co_return;
    }
}