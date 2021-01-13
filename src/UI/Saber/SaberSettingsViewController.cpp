#include "UI/Saber/SaberSettingsViewController.hpp"
#include "config.hpp"
#include "Data/Descriptor.hpp"

#include "UnityEngine/RectOffset.hpp"
#include "UnityEngine/RectTransform.hpp"
#include "UnityEngine/Rect.hpp"
#include "UnityEngine/Vector2.hpp"
#include "UnityEngine/UI/Image.hpp"
#include "UnityEngine/UI/Toggle.hpp"
#include "UnityEngine/UI/Toggle_ToggleEvent.hpp"
#include "UnityEngine/UI/LayoutElement.hpp"
#include "UnityEngine/Events/UnityAction.hpp"
#include "UnityEngine/Events/UnityAction_1.hpp"
#include "HMUI/ScrollView.hpp"
#include "HMUI/ModalView.hpp"
#include "HMUI/Touchable.hpp"
#include "HMUI/InputFieldView.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "questui/shared/CustomTypes/Components/ExternalComponents.hpp"
#include "questui/shared/CustomTypes/Components/Backgroundable.hpp"

#include "UI/Saber/SaberPreviewViewController.hpp"
#include "Qosmetic/QuestSaber.hpp"

#include "Logging/UILogger.hpp"
#define INFO(value...) UILogger::GetLogger().info(value)
extern config_t config;
DEFINE_CLASS(Qosmetics::SaberSettingsViewController);

using namespace QuestUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace UnityEngine::Events;
using namespace HMUI;

namespace Qosmetics
{
    void SaberSettingsViewController::DidDeactivate(bool removedFromHierarchy, bool screenSystemDisabling)
    {
        
    }

    void SaberSettingsViewController::DidActivate(bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
    {
        if (firstActivation)
        {
            INFO("Saber settings View Controller first activation!");
            get_gameObject()->AddComponent<Touchable*>();
            GameObject* container = BeatSaberUI::CreateScrollableSettingsContainer(get_transform());

            container->AddComponent<Backgroundable*>()->ApplyBackground(il2cpp_utils::createcsstr("round-rect-panel"));
            
            ExternalComponents* externalComponents = container->GetComponent<ExternalComponents*>();
            RectTransform* scrollTransform = externalComponents->Get<RectTransform*>();
            scrollTransform->set_sizeDelta(UnityEngine::Vector2(0.0f, 0.0f));

            QuestUI::IncrementSetting* saberWidthSetting = BeatSaberUI::CreateIncrementSetting(container->get_transform(), "Saber Width", 2, 0.05f, config.saberConfig.saberWidth, 0.0f, 10.0f, il2cpp_utils::MakeDelegate<UnityAction_1<float>*>(classof(UnityAction_1<float>*), this, +[](SaberSettingsViewController* view, float value) {
                    if (value >= 0.0f) config.saberConfig.saberWidth = value;
                    SaveConfig();
                    QuestSaber::SelectionDefinitive();
                    SaberPreviewViewController* previewController = Object::FindObjectOfType<SaberPreviewViewController*>();//
                    if (previewController) previewController->UpdatePreview();
                    else INFO("Couldn't find preview controller");
                }));
            BeatSaberUI::AddHoverHint(saberWidthSetting->get_gameObject(), "Makes the saber more or less thick");
            BeatSaberUI::CreateToggle(container->get_transform(), "Override Trail Length", config.saberConfig.overrideTrailLength, il2cpp_utils::MakeDelegate<UnityAction_1<bool>*>(classof(UnityAction_1<bool>*), this, +[](SaberSettingsViewController* view, bool value) { 
                    config.saberConfig.overrideTrailLength = value;
                    SaveConfig();
                    QuestSaber::SelectionDefinitive();
                    SaberPreviewViewController* previewController = Object::FindObjectOfType<SaberPreviewViewController*>();//
                    if (previewController) previewController->UpdatePreview();
                    else INFO("Couldn't find preview controller");
                }));
            QuestUI::IncrementSetting* trailLengthSetting = BeatSaberUI::CreateIncrementSetting(container->get_transform(), "Trail Length", 0, 1.0f, config.saberConfig.trailLength, 0.0f, 1000.0f, il2cpp_utils::MakeDelegate<UnityAction_1<float>*>(classof(UnityAction_1<float>*), this, +[](SaberSettingsViewController* view, float value) {
                    if (value >= 0.0f) config.saberConfig.trailLength = value;
                    SaveConfig();
                    QuestSaber::SelectionDefinitive();
                    SaberPreviewViewController* previewController = Object::FindObjectOfType<SaberPreviewViewController*>();//
                    if (previewController) previewController->UpdatePreview();
                    else INFO("Couldn't find preview controller");
                }));

            BeatSaberUI::AddHoverHint(trailLengthSetting->get_gameObject(), "The overridden length of the trail, values below 0 get ignored");
            UnityEngine::UI::Toggle* whiteStepToggle = BeatSaberUI::CreateToggle(container->get_transform(), "Override White Step Value", config.saberConfig.overrideWhiteStep, il2cpp_utils::MakeDelegate<UnityAction_1<bool>*>(classof(UnityAction_1<bool>*), this, +[](SaberSettingsViewController* view, bool value) { 
                    config.saberConfig.overrideWhiteStep = value;
                    SaveConfig();
                    QuestSaber::SelectionDefinitive();
                    SaberPreviewViewController* previewController = Object::FindObjectOfType<SaberPreviewViewController*>();//
                    if (previewController) previewController->UpdatePreview();
                    else INFO("Couldn't find preview controller");
                }));
            QuestUI::IncrementSetting* whiteStepSetting = BeatSaberUI::CreateIncrementSetting(container->get_transform(), "White Step Amount", 2, 0.05f, config.saberConfig.whiteStep, 0.0f, 1.0f, il2cpp_utils::MakeDelegate<UnityAction_1<float>*>(classof(UnityAction_1<float>*), this, +[](SaberSettingsViewController* view, float value) {
                    if (value >= 0.0f && value <= 1.0f) config.saberConfig.whiteStep = value;
                    SaveConfig();
                    QuestSaber::SelectionDefinitive();
                    SaberPreviewViewController* previewController = Object::FindObjectOfType<SaberPreviewViewController*>();//
                    if (previewController) previewController->UpdatePreview();
                    else INFO("Couldn't find preview controller");
                }));
            BeatSaberUI::AddHoverHint(whiteStepSetting->get_gameObject(), "Override the duration of the \"white\" part of the saber trail, set to 0 for off");

            BeatSaberUI::CreateToggle(container->get_transform(), "Enable Menu Pointer", config.saberConfig.enableMenuPointer, il2cpp_utils::MakeDelegate<UnityAction_1<bool>*>(classof(UnityAction_1<bool>*), this, +[](SaberSettingsViewController* view, bool value) { 
                    config.saberConfig.enableMenuPointer = value;
                    SaveConfig();
                    QuestSaber::SelectionDefinitive();
                    SaberPreviewViewController* previewController = Object::FindObjectOfType<SaberPreviewViewController*>();//
                    if (previewController) previewController->UpdatePreview();
                    else INFO("Couldn't find preview controller");
                }));
            QuestUI::IncrementSetting* menuPointerSizeSetting = BeatSaberUI::CreateIncrementSetting(container->get_transform(), "Menu Pointer Size", 2, 0.05f, config.saberConfig.menuPointerSize, 0.0f, 10.0f, il2cpp_utils::MakeDelegate<UnityAction_1<float>*>(classof(UnityAction_1<float>*), this, +[](SaberSettingsViewController* view, float value) {
                    if (value >= 0.0f) config.saberConfig.menuPointerSize = value;
                    SaveConfig();
                    QuestSaber::SelectionDefinitive();
                    SaberPreviewViewController* previewController = Object::FindObjectOfType<SaberPreviewViewController*>();//
                    if (previewController) previewController->UpdatePreview();
                    else INFO("Couldn't find preview controller");
                }));
            BeatSaberUI::AddHoverHint(menuPointerSizeSetting->get_gameObject(), "Size of the menu pointer");

            QuestUI::IncrementSetting* trailTypeSetting = BeatSaberUI::CreateIncrementSetting(container->get_transform(), "Trail Type", 0, 1.0f, (int)config.saberConfig.trailType, 0.0f, 2.0f, il2cpp_utils::MakeDelegate<UnityAction_1<float>*>(classof(UnityAction_1<float>*), this, +[](SaberSettingsViewController* view, float value) {
                    switch ((int)value)
                    {
                        case 0:
                            config.saberConfig.trailType = TrailType::custom;
                            break;
                        case 1:
                            config.saberConfig.trailType = TrailType::baseGame;
                            break;
                        case 2:
                            config.saberConfig.trailType = TrailType::none;
                            break;
                        default:
                            break;
                    }
                    SaveConfig();
                    QuestSaber::SelectionDefinitive();
                    SaberPreviewViewController* previewController = Object::FindObjectOfType<SaberPreviewViewController*>();//
                    if (previewController) previewController->UpdatePreview();
                    else INFO("Couldn't find preview controller");
                }));
            BeatSaberUI::AddHoverHint(trailTypeSetting->get_gameObject(), "Sets the trail type, 0 means the attached trails, 1 means base game, 2 means no trails");
        }
    }
}