#include <iostream>
using namespace std;

char ReadData[32];
char WriteData[1];

int stringConversion()
{
    char temp1[32] = "Temperature.", temp2[32] = "Temp.";
    char cstr1[32] = "Degrees C.", cstr2[32] = "See.", cstr3[32] = "Celcius.", cstr4[32] = "Degrees Celcius.", cstr5[32] = "Centigrade.", cstr6[32] = "Degrees centigrade.";
    char fstr1[32] = "Degrees F.", fstr2[32] = "F.", fstr3[32] = "Fahrenheit.", fstr4[32] = "Degrees Fahrenheit.";
    char kstr1[32] = "Degrees, OK.", kstr2[32] = "OK.", kstr3[32] = "Kelvin.", kstr4[32] = "Kelvins.", kstr5[32] = "Degrees Kelvin.";

    char pres[32] = "Pressure.";
    char pastr1[32] = "Pascals.", pastr2[32] = "PA.", pastr3[32] = "K PA.";
    char astr1[32] = "ATM.", astr2[32] = "ATM's.", astr3[32] = "Atmospheres.", astr4[32] = "Atmosphere.";
    char hgstr1[32] = "Millimeters of Mercury.", hgstr2[32] = "Millimeters Mercury.";

    if ((strcmp(ReadData, temp1) == 0) || (strcmp(ReadData, temp2) == 0))
    {
        WriteData[0] = '0';
    }
    else if ((strcmp(ReadData, pres) == 0)) {
        WriteData[0] = '1';
    }

    if ((strcmp(ReadData, cstr1) == 0) || (strcmp(ReadData, cstr2) == 0) || (strcmp(ReadData, cstr3) == 0) || (strcmp(ReadData, cstr4) == 0) || (strcmp(ReadData, cstr5) == 0) || (strcmp(ReadData, cstr6) == 0))
    {
        WriteData[0] = 'A';
    }
    else if ((strcmp(ReadData, fstr1) == 0) || (strcmp(ReadData, fstr2) == 0) || (strcmp(ReadData, fstr3) == 0) || (strcmp(ReadData, fstr4) == 0)) {
        WriteData[0] = 'B';
    }
    else if ((strcmp(ReadData, kstr1) == 0) || (strcmp(ReadData, kstr2) == 0) || (strcmp(ReadData, kstr3) == 0) || (strcmp(ReadData, kstr4) == 0) || (strcmp(ReadData, kstr5) == 0)) {
        WriteData[0] = 'C';
    }

    else if ((strcmp(ReadData, pastr1) == 0) || (strcmp(ReadData, pastr2) == 0) || (strcmp(ReadData, pastr3) == 0)) {
        WriteData[0] = 'A';
    }
    else if ((strcmp(ReadData, astr1) == 0) || (strcmp(ReadData, astr2) == 0) || (strcmp(ReadData, astr3) == 0) || (strcmp(ReadData, astr4) == 0)) {
        WriteData[0] = 'B';
    }
    else if ((strcmp(ReadData, hgstr1) == 0) || (strcmp(ReadData, hgstr2) == 0)) {
        WriteData[0] = 'C';
    }
    else {
        cout << "That is not a valid user input.\nPlease try again.\n";
    }
}