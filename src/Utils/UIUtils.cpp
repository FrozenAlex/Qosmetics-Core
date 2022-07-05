#include "Utils/UIUtils.hpp"
#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include "HMUI/ButtonStaticAnimations.hpp"
#include "HMUI/CurvedCanvasSettingsHelper.hpp"
#include "TMPro/TextMeshProUGUI.hpp"

using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;

namespace Qosmetics::Core::UIUtils
{
    TMPro::TextMeshProUGUI* AddHeader(UnityEngine::Transform* parent, StringW title, const UnityEngine::Color& color)
    {
        VerticalLayoutGroup* vertical = CreateVerticalLayoutGroup(parent);
        vertical->get_rectTransform()->set_anchoredPosition({0.0f, 45.0f});
        HorizontalLayoutGroup* horizontal = CreateHorizontalLayoutGroup(vertical->get_transform());

        TMPro::TextMeshProUGUI* text = CreateText(horizontal->get_transform(), title);
        //->get_gameObject()->AddComponent<Touchable*>();
        text->set_fontSize(text->get_fontSize() * 2.0f);
        text->set_alignment(TMPro::TextAlignmentOptions::Center);

        LayoutElement* layoutelem = text->get_gameObject()->AddComponent<LayoutElement*>();
        layoutelem->set_preferredHeight(10.0f);
        layoutelem->set_preferredWidth(90.0f);

        Backgroundable* background = horizontal->get_gameObject()->AddComponent<Backgroundable*>();
        background->ApplyBackgroundWithAlpha("title-gradient", 1.0f);

        auto* imageView = background->get_gameObject()->GetComponentInChildren<HMUI::ImageView*>();
        imageView->skew = 0.18f;
        imageView->gradient = true;
        imageView->gradientDirection = 0;
        imageView->set_color(color);
        imageView->set_color0(Color(1.0f, 1.0f, 1.0f, 1.0f));
        imageView->set_color1(Color(1.0f, 1.0f, 1.0f, 0.0f));
        imageView->curvedCanvasSettingsHelper->Reset();

        return text;
    }

    void SetTitleColor(HMUI::TitleViewController* titleView, const UnityEngine::Color& color, bool buttonanim)
    {
        Transform* title_T = titleView->get_transform();
        Transform* BG_T = title_T->Find("BG");
        Transform* BackButtonBG_T = title_T->Find("BackButton/BG");

        HMUI::ImageView* imageView = BG_T->get_gameObject()->GetComponent<HMUI::ImageView*>();
        Color oldColor = imageView->get_color();

        imageView->set_color(color);

        HMUI::ImageView* buttonImageView = BackButtonBG_T->get_gameObject()->GetComponent<HMUI::ImageView*>();
        buttonImageView->set_color(color);
        buttonImageView->set_color0(color);
        buttonImageView->set_color1(color * 0.9f);

        HMUI::ButtonStaticAnimations* anim = BackButtonBG_T->get_parent()->get_gameObject()->GetComponent<HMUI::ButtonStaticAnimations*>();
        anim->set_enabled(buttonanim);
    }

    UnityEngine::GameObject* CreateLoadingIndicator(UnityEngine::Transform* parent)
    {
        auto original = UnityEngine::Resources::FindObjectsOfTypeAll<GameObject*>().First([](auto el)
                                                                                          { return el->get_name() == "LoadingIndicator"; });
        auto loadingIndicator = UnityEngine::Object::Instantiate(original, parent, false);
        static ConstString QosmeticsLoadingIndicator("QosmeticsLoadingIndicator");
        loadingIndicator->set_name(QosmeticsLoadingIndicator);

        loadingIndicator->AddComponent<LayoutElement*>();
        return loadingIndicator;
    }
}