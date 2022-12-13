#include <iostream>
using namespace std;

int unitConversion()
{
    int outC = TOUT * 0.1;
    int outF = (outC * 9 / 5) + 32, outK = outC + 273.15;
    char cstr1[32] = "degrees C", cstr2[32] = "C", cstr3[32] = "Celcius", cstr4[32] = "degrees Celcius", cstr5[32] = "Centigrade", cstr6[32] = "degrees Centigrade";
    char fstr1[32] = "degrees F", fstr2[32] = "F", fstr3[32] = "Fahrenheit", fstr4[32] = "degrees Fahrenheit";
    char kstr1[32] = "degrees K", kstr2[32] = "K", kstr3[32] = "Kelvin", kstr4[32] = "Kelvins", kstr5[32] = "degrees Kelvin";

    int outPa = p;
    int outA = outPa / 101300, outHg = outPa / 133.3;
    char pastr1[32] = "Pascals", pastr2[32] = "PA", pastr3[32] = "K PA";
    char astr1[32] = "ATM", astr2[32] = "ATMs", astr3[32] = "atmospheres", astr4[32] = "atmosphere";
    char hgstr1[32] = "millimeters of Mercury", hgstr2[32] = "millimeters Mercury";

    if ((strcmp(ReadData, cstr1) == 0) || (strcmp(ReadData, cstr2) == 0) || (strcmp(ReadData, cstr3) == 0) || (strcmp(ReadData, cstr4) == 0) || (strcmp(ReadData, cstr5) == 0) || (strcmp(ReadData, cstr6) == 0))
    {
        cout << "The temperature is currently " << outC << " degrees C.\n";
    }
    else if ((strcmp(ReadData, fstr1) == 0) || (strcmp(ReadData, fstr2) == 0) || (strcmp(ReadData, fstr3) == 0) || (strcmp(ReadData, fstr4) == 0)) {
        cout << "The temperature is currently " << outF << " degrees F.\n";
    }
    else if ((strcmp(ReadData, kstr1) == 0) || (strcmp(ReadData, kstr2) == 0) || (strcmp(ReadData, kstr3) == 0) || (strcmp(ReadData, kstr4) == 0) || (strcmp(ReadData, kstr5) == 0)) {
        cout << "The temperature is currently " << outK << " Kelvin.\n";
    }
    else if ((strcmp(ReadData, pastr1) == 0) || (strcmp(ReadData, pastr2) == 0) || (strcmp(ReadData, pastr3) == 0)) {
        cout << "The pressure is currently " << outP << " kPa.\n";
    }
    else if ((strcmp(ReadData, astr1) == 0) || (strcmp(ReadData, astr2) == 0) || (strcmp(ReadData, astr3) == 0) || (strcmp(ReadData, astr4) == 0)) {
        cout << "The pressure is currently " << outA << " atm.\n";
    }
    else if ((strcmp(ReadData, hgstr1) == 0) || (strcmp(ReadData, hgstr2) == 0)) {
        cout << "The pressure is currently " << outHg << " mmHg.\n";
    }
    else {
        cout << "That is not a valid user input.\nPlease try again.";
    }
}