#include <iostream> 
#include <stdlib.h>
#include <speechapi_cxx.h>
#include "Serial.h"

using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;

std::string GetEnvironmentVariable(const char* name);

int main()
{
    // This example requires environment variables named "SPEECH_KEY" and "SPEECH_REGION"
    //auto speechKey = GetEnvironmentVariable("SPEECH_KEY");
    //auto speechRegion = GetEnvironmentVariable("SPEECH_REGION");
    auto speechKey = "f0f7d44dcf234952a9154d6a8af3a714";
    auto speechRegion = "eastus";
    if ((speechKey == 0) || (speechRegion == 0)) {
        std::cout << "Please set both SPEECH_KEY and SPEECH_REGION environment variables." << std::endl;
        return -1;
    }

    auto speechConfig = SpeechConfig::FromSubscription(speechKey, speechRegion);

    speechConfig->SetSpeechRecognitionLanguage("en-US");

    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();
    auto recognizer = SpeechRecognizer::FromConfig(speechConfig, audioConfig);

    std::cout << "Speak into your microphone.\n";
    auto result = recognizer->RecognizeOnceAsync().get();

    if (result->Reason == ResultReason::RecognizedSpeech)
    {
        std::cout << "RECOGNIZED: Text: " << result->Text << std::endl;
    }
    else if (result->Reason == ResultReason::NoMatch)
    {
        std::cout << "NOMATCH: Speech could not be recognized." << std::endl;
    }
    else if (result->Reason == ResultReason::Canceled)
    {
        auto cancellation = CancellationDetails::FromResult(result);
        std::cout << "CANCELED: Reason=" << (int)cancellation->Reason << std::endl;

        if (cancellation->Reason == CancellationReason::Error)
        {
            std::cout << "CANCELED: ErrorCode=" << (int)cancellation->ErrorCode << std::endl;
            std::cout << "CANCELED: ErrorDetails=" << cancellation->ErrorDetails << std::endl;
            std::cout << "CANCELED: Did you set the speech resource key and region values?" << std::endl;
        }
    }
    
    auto sendStr = result->Text;
    std::cout << "Here's what I will try to send: " << sendStr << std::endl;
    auto strData = &sendStr;


    

}
