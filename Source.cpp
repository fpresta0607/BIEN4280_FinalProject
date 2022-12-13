#include <iostream> 
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <speechapi_cxx.h>
#include "Serial.h"
#include <Windows.h>
#include <cstring>

using namespace std;
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
    
    // converts voice text to char array
    auto sendStr = result->Text;
    auto end = ";";
    sendStr = sendStr + end;
    const char* buffer = sendStr.c_str();
    char SerialBuffer[16];
    char temp[1];
    char* ReadData[17];        
    
    for (int i = 0; i < 16; i++) {
        SerialBuffer[i] = ' ';
    }
    strcpy_s(SerialBuffer, buffer);
    std::cout << "Here's what I will try to send: " << SerialBuffer << std::endl;
   


    DCB dcbSerialParams = { 0 };  // Initializing DCB structure
    COMMTIMEOUTS timeouts = { 0 };
    BOOL Status;
    DWORD BytesWritten = 0, dwEventMask,NoBytesRead; //32 bit unsigned int
    
    
    unsigned char loop = 0;
    HANDLE hCOM = CreateFile(L"\\\\.\\COM5",   // com4 is ready to communicate ==> open the port
                            GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL);

    if (hCOM == INVALID_HANDLE_VALUE) {
        std::cout << "Port can't be opened" << std::endl;
    }
    else {
        std::cout << "Port opened." << std::endl;
    }

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if(GetCommState(hCOM, &dcbSerialParams) == 0) {
        std::cout << "Error getting device state\n" << std::endl;
        CloseHandle(hCOM);
        return 1;
    }
    dcbSerialParams.BaudRate = CBR_9600;      //BaudRate = 9600
    dcbSerialParams.ByteSize = 8;             //ByteSize = 8
    dcbSerialParams.StopBits = ONESTOPBIT;    //StopBits = 1
    dcbSerialParams.Parity = NOPARITY;      //Parity = None
    
    if (SetCommState(hCOM, &dcbSerialParams)==0) {
        std::cout << "Error setting device state.\n" << std::endl;
        CloseHandle(hCOM);
        return 1;

    }
    // Set COM port timeout settings
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (SetCommTimeouts(hCOM, &timeouts) == 0) {
        std::cout << "Error setting device timeouts.\n" << std::endl;
        CloseHandle(hCOM);
        return 1;
    
    }

    for (int i = 0; i < 16; i++) {
        temp[0] = SerialBuffer[i];
        
        WriteFile(hCOM,// Handle to the Serialport
                  temp,            // Data to be written to the port
                  1,   // No of bytes to write into the port
                  &BytesWritten,  // No of bytes written to the port
                  NULL);
        if (SetCommMask(hCOM, EV_RXCHAR) == 0) {
            std::cout << "Error setting COM mask.\n" << std::endl;
        }
        if (WaitCommEvent(hCOM, &dwEventMask, NULL) == 0) {
            std::cout << "Error waiting for COM event.\n" << std::endl; //Wait for the character to be received
        } 
    }
    std::cout << "Number of bytes written: " << BytesWritten << std::endl;

    
    
    if (ReadFile(hCOM, ReadData, sizeof(ReadData), &NoBytesRead, NULL))
    {
        for (int i = 0; i < NoBytesRead; i++) {
            printf("%c", ReadData[i]);
        } 
    }
    


    std::cout << "Number of bytes recieved: " << NoBytesRead << std::endl;
    
    CloseHandle(hCOM);

    system("pause");
}
