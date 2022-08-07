#include "CryptXor.h"

CryptXor::CryptXor()
{
    std::cout << "Creato oggetto CriptXor" << std::endl;
}
CryptXor::CryptXor(int argc,char* argv[])
{

    manageParams(argc,argv);
}
void CryptXor::printManual()
{
    cout<<"---------------------------------------------------------------------------------------------------\n";

    cout<<"CryptXor : cripta i tuoi file con l'operatore Xor \n";
    cout << "Usage :  criptXor [OPTION]... [FILE]...\n";
    cout << "-c   --conceal                 opzione per nascondere la cifratura\n";
    cout << "-cd  --concealed               opzione per decriptare cifratura nascosta\n";
    cout << "-r   --recursion               opzione per attivare la ricorsivita' all'interno di una cartella\n";
    cout << "-h   --help                    visualizza questo manuale\n";
    cout << "\nExamples :\n";
    cout << "           cryptXor filename                   cripta/decripta un file \n";
    cout << "           cryptXor -c filename                cripta un file e nasconde la cifratura\n";
    cout << "           cryptXor -cd filename               decripta un file con cifratura nascosta \n";
    
    cout << "           cryptXor directoryname              cripta/decripta una cartella\n";
    cout << "           cryptXor -c directoryname           cripta una cartella nascondendo la cifratura\n";
    cout << "           cryptXor -cd directoryname          decripta una cartella con cifratura nascosta\n";

    cout << "           cryptXor -r directoryname           cripta/decripta cartella e sottocartelle\n";
    cout << "           cryptXor -r -c directoryname        cripta cartella e sottocartelle nascondendo la cifratura\n";
    cout << "           cryptXor -r -cd directoryname       decripta cartella e sottocartelle con cifratura nascosta\n";

    cout<<"---------------------------------------------------------------------------------------------------\n";
}

void CryptXor::manageParams(int argc,char* argv[])
{
    string errorMsg="ERRORE IMMISSIONE PARAMETRI";
    if (argc>4)
    {
        cout << errorMsg << endl;
        CryptXor::printManual();
        exit(1);
    }

    for(int c=1;c<argc;c++)
        {
            string arg=string(argv[c]);
            if (arg=="-cd" || arg=="--concealed") {mode[0]=1; mode[1]=1;}
            else if (arg=="-c" || arg=="--conceal"){mode[1]=1;}
            else if (arg=="-r" || arg=="--recursion") {mode[2]=1;}
            else if (arg=="-h" || arg=="--help")
            {
                CryptXor::printManual();
                exit(0);
            }
            else {target=arg;}
        }
    if (target=="")
    {
        cout << errorMsg << " : NO TARGET" <<endl;
        CryptXor::printManual();
        exit(1);
    }
    else
    {   
        if(filesystem::exists(target))
        {
            if (mode[0]==0 && mode[1]==1)
            {
                srand(time(NULL));
                vector<string> img;
                vector<string> video;
                vector<string> audio;
                for(const auto & inputF : filesystem::directory_iterator("input/img"))
                    {
                        img.push_back(inputF.path());
                    }
                for(const auto & inputF : filesystem::directory_iterator("input/video"))
                    {
                        video.push_back(inputF.path());
                    }
                for(const auto & inputF : filesystem::directory_iterator("input/audio"))
                    {
                        audio.push_back(inputF.path());
                    }
                inputFiles.push_back(img);
                inputFiles.push_back(audio);
                inputFiles.push_back(video);
            }
            cout << "password : ";
            cin >> password;

            if (filesystem::is_regular_file(target))
            {
                CryptXor::crypt(target);
            }
            else if (filesystem::is_directory(target))
            {
                if (mode[2]==0)
                {
                    for(const auto & tgt : filesystem::directory_iterator(target))
                    {
                        if (filesystem::is_regular_file(tgt))
                        {
                            CryptXor::crypt(tgt.path());
                        }
                    }
                }
                else
                {
                    for(const auto & tgt : filesystem::recursive_directory_iterator(target))
                    {
                        if (filesystem::is_regular_file(tgt))
                        {
                            CryptXor::crypt(tgt.path());
                        }
                    }
                }
            }
        }
        else
        {
            cout << errorMsg << ": " << target << " NON TROVATO" << endl;
            CryptXor::printManual();
            exit(1);
        }
    }
}

void CryptXor::crypt(string pathTarget)
{
    
    targetFile.open(pathTarget,ios::in | ios::out | ios::binary);
    char ch='\0';
    targetFile.seekg(0,targetFile.end);
    int fileLength=targetFile.tellg();
    targetFile.seekg(0,targetFile.beg);
    char * buffer=new char[fileLength];
    targetFile.read(buffer,fileLength);

    targetFile.clear();

    targetFile.seekg(0,ios::beg);
    string ext=filesystem::path(pathTarget).extension();
    int fileInputType=0;

    if (mode[0]==0 && mode[1]==1)   // -e -c     aggiungere all'inizio del file
    {

        if(ext==".mp3" || ext==".wav" || ext==".m4a"){fileInputType=1;}
        else if(ext==".mp4" || ext==".avi"){fileInputType=2;}
        string pathInput = inputFiles[fileInputType][rand()%inputFiles[fileInputType].size()];

        inputFile.open(pathInput,ios::in | ios::binary);
        inputFile.seekg(0,ios::end);
        int inputLength=inputFile.tellg();
        inputFile.seekg(0,inputFile.beg);
        char * bufferIn=new char[inputLength];
        inputFile.read(bufferIn,inputLength);
        inputFile.clear();

        for (int c=0;c<inputLength;c++)
        {
            targetFile << bufferIn[c];
        }
        for (int c=0;c< maxLengthInput[fileInputType]-inputLength;c++)
        {
            targetFile << '\0';
        }
        inputFile.close();
        
        for (int c=0, d=0;c<fileLength;c++,d++)
        {
            if(d>password.length()){d=0;}
            char bw=buffer[c]^password[d];
            targetFile << bw;
        }
    }
    else if (mode[0]==1 && mode[1]==1) // -d -c   togliere all'inizio del file
    {
        if(ext==".mp3" || ext==".wav" || ext==".m4a"){fileInputType=1;}
        else if(ext==".mp4" || ext==".avi"){fileInputType=2;}

        targetFile.seekg(0,ios::beg);
        for(int c=maxLengthInput[fileInputType],d=0;c<fileLength;c++,d++)
        {
            if(d>password.length()){d=0;}
            char bw=buffer[c]^password[d];
            targetFile << bw;
        }
        targetFile.close();
        filesystem::resize_file(pathTarget,fileLength-maxLengthInput[fileInputType]);
    }


    else   //prova temporanea
    {
        for (int c=0, d=0;c<fileLength;c++,d++)
        {
            if(d>password.length()){d=0;}
            char bw=buffer[c]^password[d];
            targetFile << bw;
        }
    }

    targetFile.close();
}

void CryptXor::testAverageLength()
{
    // for (string s : inputFiles)
    //     {
    //         inputFile.open(s,ios::in| ios::binary);
    //         inputFile.seekg(0,ios::end);
    //         cout << inputFile.tellg() << endl;
    //         inputFile.close();
    //         inputFile.clear();
    //     }
}
