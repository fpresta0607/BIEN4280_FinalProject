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
    while (true) {
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

        const char* buffer = sendStr.c_str();
        std::string s = "1";
        char ReadData[256];


        //strcpy_s(SerialBuffer, buffer);
        std::cout << "Here's what I will try to send: " << s.c_str() << std::endl;



        DCB dcbSerialParams = { 0 };  // Initializing DCB structure
        COMMTIMEOUTS timeouts = { 0 };
        BOOL Status;
        DWORD BytesWritten = 0, dwEventMask, NoBytesRead; //32 bit unsigned int


        HANDLE hCOM = CreateFile(L"\\\\.\\COM5",   // com4 is ready to communicate ==> open the port
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (hCOM == INVALID_HANDLE_VALUE) {
            std::cout << "Port can't be opened" << std::endl;
        }
        else {
            //std::cout << "Port opened." << std::endl;
        }

        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

        if (GetCommState(hCOM, &dcbSerialParams) == 0) {
            std::cout << "Error getting device state\n" << std::endl;
            CloseHandle(hCOM);
            return 1;
        }
        dcbSerialParams.BaudRate = CBR_9600;      //BaudRate = 9600
        dcbSerialParams.ByteSize = DATABITS_8;             //ByteSize = 8
        dcbSerialParams.StopBits = ONESTOPBIT;    //StopBits = 1
        dcbSerialParams.Parity = NOPARITY;      //Parity = None

        if (SetCommState(hCOM, &dcbSerialParams) == 0) {
            std::cout << "Error setting device state.\n" << std::endl;
            CloseHandle(hCOM);
            return 1;

        }
        // Set COM port timeout settings
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 50;
        timeouts.WriteTotalTimeoutConstant = 50;
        timeouts.WriteTotalTimeoutMultiplier = 10;
        if (SetCommTimeouts(hCOM, &timeouts) == 0) {
            std::cout << "Error setting device timeouts.\n" << std::endl;
            CloseHandle(hCOM);
            return 1;

        }
        EscapeCommFunction(hCOM, CLRDTR);
        EscapeCommFunction(hCOM, SETDTR);


        if (WriteFile(hCOM,// Handle to the Serialport
            s.c_str(),            // Data to be written to the port
            strlen(s.c_str()),   // No of bytes to write into the port
            &BytesWritten,  // No of bytes written to the port
            NULL) != 0) {
            printf("message received.\n");
        }


       // std::cout << "Number of bytes written: " << BytesWritten << std::endl;

        if (SetCommMask(hCOM, EV_RXCHAR) == 0) {
            std::cout << "Error setting COM mask.\n" << std::endl;
        }
        if (WaitCommEvent(hCOM, &dwEventMask, NULL) == 0) {
            std::cout << "Error waiting for COM event.\n" << std::endl; //Wait for the character to be received
        }

        Status = ReadFile(hCOM, &ReadData, sizeof(ReadData), &NoBytesRead, NULL);

        for (int i = 0; i < sizeof(ReadData); i++) {
            if (ReadData[i] == ';') {
                break;
            }
            std::cout << ReadData[i];
        }

        //std::cout << "\nNumber of bytes recieved: " << NoBytesRead << std::endl;
        CloseHandle(hCOM);
    }
    

    system("pause");
}
