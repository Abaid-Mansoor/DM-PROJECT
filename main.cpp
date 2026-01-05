#include <iostream>
#include <math.h>
#include <conio.h>
#include <fstream>
using namespace std;
void printHeader();
void printMenu();
string generateSerialId(int serial, int shelfNo,int &checkDigit);
int generateCheckDigit(int, int);
void readingDataFromInputSerialId(string serialId, int &shelfNo, int &serialNo, int &checkDigit);
void writingDataInFile(string serialId, string bookName, string author,string serialIntoBinary);
bool verifyInputFormat(string serial);
string readingDataFromField(string record, int fieldNo);
string verifyIfSerialIdExist(string serialId);
int readingLastSerialFromFile();
void writingLastSerialInFile(string);
string concatenateSerialId(string,string);
string intoBinary(int decimal);
string GenerateBinaryFromSerial(int shelfNo,int serial,int checkDigit);
void printBarcode(string);


main()
{
    int currentSerial = 0;
    while (true)
    {
        system("cls");
        printHeader();
        printMenu();
        string choices;
        cin >> choices;
        cin.ignore();
        if (choices == "1")
        {

            int lastSerial = readingLastSerialFromFile();
            if(currentSerial!=lastSerial||lastSerial==0)
                currentSerial=lastSerial+1;

            string bookName, author, serialId;
            int shelfNo,checkDigit;
            cout << "Enter book Name: ";
            getline(cin >> ws, bookName);
            cout << "Enter author Name: ";
            getline(cin >> ws, author);
            cout << "Enter the shelf Number (1-99): ";
            cin >> shelfNo;
            if (!(shelfNo >= 1 && shelfNo <= 99))
            {
                cout<<"Enter a shelf number between 1 and 99"<<endl;
                getch();
                continue;
            }
            serialId = generateSerialId(currentSerial, shelfNo,checkDigit);
            string serialToBianry=GenerateBinaryFromSerial(shelfNo,currentSerial,checkDigit);
            cout<<"------------------------------------------------"<<endl;
            cout <<"Serial Id: "<< serialId << endl;
            cout<<"Barcode:"<<endl;
            printBarcode(serialToBianry);
            cout<<endl;
            writingDataInFile(serialId, bookName, author,serialToBianry);
            writingLastSerialInFile(to_string(currentSerial));
            currentSerial++;
            cout << "Press Any Key to continue" << endl;
            getch();
        }
        else if (choices == "2")
        {
            string serialId, record;
            cout << "Enter the serial of a book: ";
            getline(cin >> ws, serialId);
            if (verifyInputFormat(serialId))
            {
                int shelfNo = 0, serialNo = 0, checkDigit = 0;
                readingDataFromInputSerialId(serialId, shelfNo, serialNo, checkDigit);
                record = verifyIfSerialIdExist(serialId);
                if (record != "")
                {
                    if(generateCheckDigit(serialNo, shelfNo) == checkDigit){
                    cout<<"------------------------------------------------"<<endl;    
                    cout << "Valid Serial ID" << endl;
                    cout << "Book Name: " << readingDataFromField(record, 2) << endl;
                    cout << "Author: " << readingDataFromField(record, 3) << endl;
                    printBarcode(readingDataFromField(record, 4));
                    cout<<endl;
                    }else{
                        cout<<"Invalid Check Digit!!!"<<endl;
                    }
                }else{
                    cout<<"Serial not found"<<endl;
                }
            }
            else
            {
                cout << "Invalid Input!!!" << endl;
            }
        }
        else if (choices == "3")
        {
            break;
        }
        else
        {
            cout << readingDataFromField("L-23-00000-3,Wasif Shuja,Wasif Shuja,", 4) << endl;
            cout << "Invalid Input" << endl;
        }
        getch();
    }
}

void printMenu()
{
    cout << "1. Generate a Serial ID " << endl;
    cout << "2. Validate a Serial ID " << endl;
    cout << "3. Exit" << endl;
}

string generateSerialId(int serial, int shelfNo,int &checkDigit)
{
    checkDigit=(generateCheckDigit(serial, shelfNo));
    string serialId = "";
    serialId += "L-";
    if(shelfNo<10){
        serialId+='0';
    }
    serialId += to_string(shelfNo);
    serialId += "-";
    if (serial < 10000)
    {
        for (int i = 5 - to_string(serial).length(); i > 0; i--)
        {
            serialId += "0";
        }
    }
    serialId += to_string(serial);
    serialId += "-";
    serialId += to_string(checkDigit);
    return serialId;
}


int generateCheckDigit(int serial, int shelfNo)
{
    string numericalPartOfSerialId=concatenateSerialId(to_string(serial),to_string(shelfNo));
    int size = numericalPartOfSerialId.length();
    int tempSerial = stoi(numericalPartOfSerialId);
    
    int sum = 0;
    int divisor;
    for (int i = 0; i < size; i++)
    {
        divisor = pow(10, (size - 1) - i);
        sum += (tempSerial / divisor) * (i + 2);
        
        tempSerial %= divisor;
    }
    return sum % 10;
}

void readingDataFromInputSerialId(string serialId, int &shelfNo, int &serialNo, int &checkDigit)
{
    int hyphenCount = 0;
    string shelfNoInString = "", serialNumberInString = "";
    string checkDigitInString="";
    for (int i = 0; i < serialId.length(); i++)
    {
        if (serialId[i] == '-')
        {
            hyphenCount++;
        }
        else
        {
            if (hyphenCount == 1)
            {
                shelfNoInString += serialId[i];
            }
            else if (hyphenCount == 2)
            {
                serialNumberInString += serialId[i];
            }
            else if (hyphenCount == 3)
            {
                checkDigitInString += serialId[i];
            }
        }
    }
    
    shelfNo = stoi(shelfNoInString);
    serialNo = stoi(serialNumberInString);
    checkDigit = stoi(checkDigitInString);
}

bool verifyInputFormat(string serial)
{
    if(serial.length()==12){
        for (int i = 0; i < serial.length(); i++)
        {
            if (serial[0] != 'L')
            {
                return false;
            }
            if (i == 1 || i == 4 || i == 10)
            {
                if (serial[i] != '-')
                {
                    return false;
                }
            }
            if (i == 2 || i == 3 || (i <= 5 && i >= 9) || i == 11)
            {
                int ascci = int(serial[i]);
            
            
                if (!(ascci >= 48 && ascci <= 57))
                {
                    return false;
                }
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}

void writingDataInFile(string serialId, string bookName, string author,string serialToBinary)
{
    fstream file;
    file.open("data.txt", ios::app);
    file << serialId << "," << bookName << "," << author << ","<<serialToBinary<<",";
    file << endl;
    file.close();
}

string readingDataFromField(string record, int fieldNo)
{
    string field = "";
    int commaCount = 0;
    for (int i = 0; commaCount != fieldNo && i < record.length(); i++)
    {
        if (record[i] == ',')
        {
            commaCount++;
        }
        else if (commaCount == fieldNo - 1)
        {
            field += record[i];
        }
    }
    return field;
}

string verifyIfSerialIdExist(string serialId)
{
    fstream file;
    file.open("data.txt", ios::in);
    string record = "";
    while (getline(file,record))
    {
        if (readingDataFromField(record, 1) == serialId)
        {
            break;
        }
        record = "";
    }
    file.close();
    return record;
}

int readingLastSerialFromFile(){
    string lastSerial;
    fstream file;
    file.open("lastSerial.txt",ios::in);

    if(!(file>>lastSerial)){
        lastSerial='0';
    }
    file.close();
    return stoi(lastSerial);
}

void writingLastSerialInFile(string lastSerial){
    fstream file;
    file.open("lastSerial.txt",ios::out);
    file<<lastSerial;
    file.close();
}

string concatenateSerialId(string serial,string shelfNo){
    string numericalpartofserialId="";
    if(shelfNo.length()==1){
        numericalpartofserialId+='0';
    }
    numericalpartofserialId+=shelfNo;
    for(int i=5-serial.length();i>0;i--){
        numericalpartofserialId+='0';
    }
    numericalpartofserialId+=serial;
    return numericalpartofserialId;
}

void printHeader(){
    cout<<"----------------------------------------------"<<endl;
    cout<<"\tBook Serial Id Generation"<<endl;
    cout<<"\t And Verification System"<<endl;
    cout<<"----------------------------------------------"<<endl;
}

string GenerateBinaryFromSerial(int shelfNo,int serial,int checkDigit){
    string temp="",binaryTemp="";

    if(shelfNo<10)
        temp+='0';
    temp+=(to_string(shelfNo)+to_string(serial)+to_string(checkDigit));
    for(int i=0;i<temp.length();i++){
        if(temp[i]=='0'){
            binaryTemp+="0000";
        }else{
            binaryTemp+=intoBinary(temp[i]-'0');
        }


    }    
    return binaryTemp;
}

string intoBinary(int decimal){
    string binary="",fourBitBinary="";
    while(true){
        binary+=to_string(decimal%2);
        if(decimal<2){
            binary+=decimal;

            break;
        }else{
        decimal/=2;
        }
    }
    for(int i=4-binary.length();i>=0;i--){
        fourBitBinary+='0';
    }
    fourBitBinary+=binary;
    return fourBitBinary;
}

void printBarcode(string binary){
    cout<<"|||";
    for(int i=0;i<binary.length();i++){
        if(binary[i]=='0'){
            cout<<" ";
        }else if(binary[i]=='1'){
            cout<<"|";
        }
    }
    cout<<"|||";
}