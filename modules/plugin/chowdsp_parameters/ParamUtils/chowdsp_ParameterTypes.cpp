#include "chowdsp_ParameterTypes.h"

#include <defs.h>
#include <utility>

namespace chowdsp
{
FloatParameter::FloatParameter (const ParameterID& parameterID,
                                const juce::String& parameterName,
                                const juce::NormalisableRange<float>& valueRange,
                                float defaultFloatValue,
                                std::array<std::atomic<float>**,MAX_NUM_VOICES>& voice_ptrs,
                                const std::function<void ( float)>& setterFunc,
                                const std::function<juce::String (float)>& valueToTextFunction,
                                std::function<float (const juce::String&)>&& textToValueFunction)
#if JUCE_VERSION < 0x070000
    : juce::AudioParameterFloat (
        parameterID,
        parameterName,
        valueRange,
        defaultFloatValue,
        juce::String(),
        AudioProcessorParameter::genericParameter,
        valueToTextFunction == nullptr
            ? std::function<juce::String (float v, int)>()
            : [valueToTextFunction] (float v, int)
            { return valueToTextFunction (v); },
        std::move (textToValueFunction)),
#else
    : juce::AudioParameterFloat (
        parameterID,
        parameterName,
        valueRange,
        defaultFloatValue,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction (
                [valueToTextFunction] (float v, int)
                { return valueToTextFunction (v); })
            .withValueFromStringFunction (std::move (textToValueFunction))),
#endif
      unsnappedDefault (valueRange.convertTo0to1 (defaultFloatValue)),
      normalisableRange (valueRange)
{
    _0to1value = convertTo0to1(defaultFloatValue);
    for (auto& ptr : voice_ptrs) {
        *ptr = &_0to1value;
    }
    setFunc = std::move(setterFunc);
}

void FloatParameter::applyMonophonicModulation (double modulationValue)
{
    modulationAmount = (float) modulationValue;
}

float FloatParameter::getCurrentValue() const noexcept
{
    return normalisableRange.convertFrom0to1 (juce::jlimit (0.0f, 1.0f, normalisableRange.convertTo0to1 (get()) + modulationAmount));
}
} // namespace chowdsp
