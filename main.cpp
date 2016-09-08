/*
 Miles Tuffs - mt511 - 4503223 - CSCI361 - Ass3
 Final
 */

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <time.h>
#include <fstream>
#include <sstream>
#include <cmath>
#include "Infint.h"

unsigned long long genRandom(int );
unsigned long long randomNum();
bool isPrime(unsigned long long );
unsigned long long gcd(unsigned long long a, unsigned long long b);
unsigned long long extendedGcd(unsigned long long a, unsigned long long b);
unsigned long long exponentialModulo(unsigned long long , unsigned long long , unsigned long long );
int keyGen();
int encrypt();
int decrypt();
char* publicFile = "pk.txt";
char* secretFile = "sk.txt";
char* cypherFile = "ct.txt";
char* plainFile = "pt.txt";

#define __uint64 unsigned long long int

int main(int argc, const char * argv[])
{
    srand( time(NULL));
    char command;
    bool exit = false;
    std::cout << "Main menu: " << std::endl;
    while (exit == false)
    {
        std::cout << "(k)eygen (e)ncrypt (d)ecrypt (Q)uit" << std::endl;
        std::cin >> command;
        std::cin.ignore(10000,'\n');
        switch(command) {
            case 'k':
                keyGen();
                break;
            case 'e':
                encrypt();
                break;
            case 'd':
                decrypt();
                break;
            case 'Q':
                exit=true;
                break;
            default:
                std::cout << "Invalid menu item." << std::endl;
                break;
        }
        
    }
    return 0;
}



int keyGen()
{
    int keySize =0;
    unsigned long long lP, lQ;
    lP = 0;
    lQ = 0;
    
    std::cout << "Enter size of P and Q: " <<std::endl;
    std::cin >> keySize;
    std::cin.ignore(10000, '\n');
    if(keySize >32)
    {
        std::cout << "Keysize is too large." << std::endl;
        return -87;
    }
    if(keySize <2)
    {
        std::cout << "Keysize is too small." << std::endl;
        return -86;
    }
    
    lP = genRandom(keySize);
    while(!isPrime(lP))
        lP = genRandom(keySize);
    
    lQ = genRandom(keySize);
    while(!isPrime(lQ))
        lQ = genRandom(keySize);
    
    unsigned long long lN, lM;
    unsigned long long lE, lRand;
    unsigned long long lD;
    unsigned long long lArr[5];
    lN = lP * lQ;
    lM = (lP-1)*(lQ-1);
    for(int i=0; i<5;i++)
    {
        lRand = (randomNum())%lM;
        
        while(gcd(lRand, lM)!=1 || lRand == 1)
        {
            lRand = (randomNum())%lM;
        }
        lArr[i] = lRand;
        //std::cout << "e" << i << ": " << lArr[i] << std::endl;
    }
    // choose 1 of the 5, can be different every time
    lE = lArr[rand()%5];
    
    lD = extendedGcd(lM, lE);
    
    std::ofstream fout;
    fout.open(publicFile);
    fout << "(" << lN << "," << lE << ")";
    fout.close();
    fout.open(secretFile);
    fout << "(" << lN << "," << lD << ")";
    fout.close();
    return 0;
}

int encrypt()
{
    // Read in public key
    unsigned long long lN, lE, lInput, lC;
    std::stringstream ss;
    ss.clear();
    std::string sBuffer;
    std::ifstream fin;
    fin.open(publicFile);
    fin.get();
    getline(fin,sBuffer,',');
    ss << sBuffer;
    ss >> lN;
    getline(fin,sBuffer,')');    ss.clear();
    ss << sBuffer;
    ss >> lE;
    fin.close();
    // Do encrypt
    fin.open(plainFile);
    //    std::cout << "Enter your message: ";
    fin >> lInput;
    fin.close();
    //    std::cin.ignore(10000,'\n');
    if(lInput >= lN) {
        std::cout << "Message is larger than N." << std::endl;
        return -97;
    }
    if(lInput <= 0) {
        std::cout << "Message too short." << std::endl;
        return -97;
    }
    
    lC = exponentialModulo(lInput, lE, lN);
    
    std::ofstream fout;
    fout.open(cypherFile);
    fout << lC;
    fout.close();
    return 0;
}

int decrypt()
{
    // read secret key
    unsigned long long lN, lD, lP, lInput;
    std::stringstream ss;
    ss.clear();
    std::string sBuffer;
    std::ifstream fin;
    fin.open(secretFile);
    fin.get();
    getline(fin,sBuffer,',');
    ss << sBuffer;
    ss >> lN;
    getline(fin,sBuffer,')');
    ss.clear();
    ss << sBuffer;
    ss >> lD;
    fin.close();
    // read in cypher text
    fin.open(cypherFile);
    fin >> lInput;
    fin.close();
    // do decrypt
    lP = exponentialModulo(lInput, lD, lN);
    
    std::ofstream fout;
    fout.open(plainFile);
    fout << lP;
    fout.close();
    return 0;
}

unsigned long long gcd(unsigned long long a,unsigned long long b)
{
    if (b==0){
        return a;
    }
    return gcd(b, a%b);
}

unsigned long long extendedGcd(unsigned long long a, unsigned long long b)
{
    InfInt x1,y1, x2, y2, q, r, temp;
    unsigned long long lM = a;
    
    x1=0;
    y1=1;
    x2=1;
    y2=0;
    while(b!=0) {
        q=a/b;
        r=a%b;
        a=b;
        b=r.toUnsignedLongLong();
        
        temp = x1;
        x1 = x2 - q * x1;
        x2 = temp;
        
        temp = y1;
        y1 = y2 - q * y1;
        y2 = temp;
    }
    if(y2<0){ // if multiplicative inverse is a negative, correct it by moding M
        y2+=lM;
    }
    return y2.toUnsignedLongLong();
}

bool isPrime(unsigned long long lInput)
{
    unsigned long long x;
	if(lInput <= 1)
		return false;
	else if(lInput ==2)
		return true;
	else if( lInput % 2 == 0)
		return false;
	else
	{
        // Fermats little theorum
        for (int i=0;i<150;i++){ // at 18 the confidence of prime is near 99.9%, testing 5 times the ammount
            x = randomNum()%lInput; // to reduce probability of incorrect result, although statistically it could happen.
            if (x==0||x==1)
                x++;
            if (exponentialModulo(x, lInput,lInput)!= x)
                return  false;
        }
        
	}
    return true;
}

unsigned long long exponentialModulo(unsigned long long lM, unsigned long long lE, unsigned long long lN)
{
    /*
     Fast exponential modulo algorithm
     http://discuss.codechef.com/questions/20451/a-tutorial-on-fast-modulo-multiplication-exponential-squaring
     */
    InfInt result = 1;
    InfInt value = lM % lN;
    
    while(lE>0)
    {
        if(lE % 2 == 1)
        {
            result = (value*result) % lN;
        }
        value = (value*value) % lN;
        lE /=2;
    }
    return result.toUnsignedLongLong();
}


unsigned long long randomNum()
{
    /*
     Generate 64 bit random in c++
     http://www.gamedev.net/topic/221409-random-longs-in-cc/
     */
    __uint64 RANDOM;
    RANDOM = ( (__uint64) rand() );
    RANDOM |= ( (__uint64) rand()<<15);
    RANDOM |= (  (__uint64) rand()<<30);
    RANDOM |= ( (__uint64) rand()<<45);
    RANDOM |= ( (__uint64) rand()<<60);
    
    return RANDOM;
}

unsigned long long genRandom(int size)
{
    unsigned long long lSize;
    unsigned long long lOutput;
    lSize=1;
    for(unsigned long long i=0; i<size-1; i++)
    {
        lSize*=2;
    }
    lOutput = randomNum() % lSize;
    if (lOutput < lSize)
    {
        lOutput += lSize-1;
    }
    if(lOutput %2 == 0)
    { // even will never be prime, since min size is 2 bits
        lOutput -=1;
    }
    return lOutput;
}
