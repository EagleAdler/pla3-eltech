#include <string.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <getopt.h>
#include <vector>

using namespace std;
//function for printing help
void printHelp()
{
    printf ("%s\n","$ crypt [options] <key> <source> [<dest>]");
    printf ("%s\n","options:");
    printf ("%s\n","-a, --alphabet=<alphabet>  alphabet — алфавит для работы алгоритма (по умолчанию содержит буквы из латниского алфавита и цифры: AaBbCc..Zz0..9)");
    printf ("%s\n","-t, --type=<type>          type может быть 'encode' или 'decode', по умолчанию — encode");
    printf ("%s\n","-h, --help                 выводит эту справку");
    printf ("%s\n","key:");
    printf ("%s\n","ключ для шифрования/дешифрования");
    printf ("%s\n","source:");
    printf ("%s\n","исходный файл для шифрования/дешифрования");
    printf ("%s\n","dest:");
    printf ("%s\n","файл, куда будет записан новый, зашифрованный текст. Если не указан, то переписывает source");
}
//function printing error by code
void printError(int code)
{
    switch(code)
    {
        case 1: printf("%s\n", "Invalid options"); break;
        case 2: printf("%s\n", "Wrong key, or invalid source");break;
        case 3: printf("%s\n", "File doesnt exist");break;
        default : printf("%\ns", "Error, what are you doing? Try -h"); break;
    }
}

int main(int argc, char *argv[])
{
    //if too many or not too much parameters call error
    if (argc<2||argc>8)
    {
        printError(1);
        return 0;
    }
    //what we do with file:
    //0 - encode, 1 - decode, by default - encode
    bool type = 0;
    //special alphabet, default value is /0
    string alphabet = "/0";
    //next variables, use in getopt_long
    //structure that determine options
    const struct option longOptions[] =
    {
        //long option return his short analogue
        {"alphabet", required_argument, NULL, 'a'},
        {"type", required_argument, NULL, 't'},
        {"help", no_argument, NULL, 'h'},
        {NULL,0,NULL,0}
    };
    //variable determine short options
    const char * shortOptions = "a:ht:";
    int longIndex;
    int option;
    //while all options isnt check, finding options
    while ((option = getopt_long(argc, argv, shortOptions, longOptions, &longIndex)) != -1)
    {
        switch (option)
        {
            //if user need help print it, close program
            case 'h':printHelp();return 0; break;
            case 't':
                        {
                            //if type is decode type = TRUE, else if type isnt encode call error
                            if (strcmp(optarg, "encode")==0||strcmp(optarg, "decode")==0)
                            type = strcmp(optarg, "decode")==0;
                            else {printError(1); return 1;} break;
                        }
            case 'a':
                        {
                            //if using special alphabet put it in alphabet variable
                            if(optarg != NULL) alphabet = optarg;
                            else {printError(1); return 1;} break;
                        }
        }
    }
        //if number of options is 1, and help
        //isnt close program above, close program
        if (argc==2) {printError(1); return 1;}
        //imagine that options contain destination path, source and key
        char * dest = argv[argc-1];
        char * source = argv[argc-2];
        char * key = argv[argc-3];
        //if we have key, call error
        if ((atoi(source)==0)&&(atoi(key)==0))
        {
            printError(1);
            printf("\n");
            printError(2);
            return 2;
        }
   //key == padding
   int padding;
   atoi(key)==0 ? padding = atoi(source):padding = atoi(key);
   //check key is not negative value
   if (padding<0) {printError(2);return 0;}
   //if need decode make key negative
   type == 1 ? padding = padding*(-1):0;

   char *inputName = source;
   char *outputName = dest;
   //if we havent destination save in the same file
   if (atoi(key)==0)
   {
       inputName = outputName;
   }

   FILE *input,*output;
   input = fopen(inputName, "rb");

   if (input == NULL) {printError(3); return 3;}
   char letter;
   //buffer contain input file
   vector <char> buffer;
   //checking alphabet
   //return: 0 - no special alphabe, 1 - use special
   bool specAlphabet = (alphabet != "/0");
   //reading file and apply crypting
   while (!feof(input))
    {
        fread (&letter ,sizeof(char) , 1, input);
        if (feof(input)) break;
        //if using special alphabet, and current letter
        //should not change go to the next
        if (specAlphabet&&alphabet.find(letter,0)==-1) {buffer.push_back(letter); continue;}
        //type is character range by code
        int type = -1;
        // character in 0..9 range
        letter >= 48 ? type++:0;
        //A..Z
        letter >= 65 ? type++:0;
        //a..z
        letter >= 97 ? type++:0;
        //keep only letters or digits and crypt
        switch (type)
        {
            //encode formula:
            // y == (x + k) % n
            //decode formula:
            //x = = (y - k + n) % n
            //where x encrypted letter, n cardinality, k key, y letter after encryption
            case 0: letter <= 57 ? letter =48+(int(letter)-48+padding+10*type)%10 : 0;break;
            case 1: letter <= 90 ? letter =65+(int(letter)-65+padding+26*type)%26 : 0;break;
            case 2: letter <= 122 ? letter =97+(int(letter)-97+padding+26*type)%26 : 0;break;
        }
        buffer.push_back(letter);
    }
    fclose(input);
    output = fopen(outputName, "wb");
    int size = (int) buffer.size();
    for (int i =0;i < size; i++) fwrite(&buffer[i],sizeof(char),1,output);
   return 0;
}