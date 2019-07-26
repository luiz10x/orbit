#include "Genome.h"
#include "Parameters.h"
#include "ReadAlign.h"
#include "Transcriptome.h"
#include "Variation.h"
#include "orbit.h"

struct Aligner
{
    public:
        Parameters *p;
        ReadAlign *ra;
        Genome *g;
        int isOriginal;

        Aligner(int argInN, char* argIn[])
        {
            isOriginal = 1;
            p = new Parameters();
            p->inputParameters(argInN, argIn);
            g = new Genome(*p);
            g->genomeLoad();
            Transcriptome *mainTranscriptome = nullptr;
            g->Var = new Variation(*p, g->chrStart, g->chrNameIndex);
            ra = new ReadAlign(*p, *g, mainTranscriptome, 0);
        }

        // This constructor is used to construct clones of an existing Aligner
        // This allows multi-threaded alignment without each thread
        // constructing its own genome object
        Aligner(Aligner* og)
        {
            isOriginal = 0;
            p = og->p;
            g = og->g;
            Transcriptome *mainTranscriptome = nullptr;
            ra = new ReadAlign(*p, *g, mainTranscriptome, 0);
        }

        ~Aligner()
        {
            delete ra;
            if(isOriginal)
            {
                delete g;
                delete p;
            }
        }
};


const char* align_read(Aligner* a, char *Read1, char *Qual1, unsigned long long read_length)
{
    a->p->iReadAll++;
    a->ra->iRead++;
    //printf("iRead %llu\n", a->ra->iRead);
    a->p->readNmates = 1;
    a->ra->readNmates = 1;
    a->ra->Read0 = &Read1;
    a->ra->Qual0 = &Qual1;
    a->ra->readName = (char*)malloc(2);
    a->ra->readName[0] = 'a';
    a->ra->readName[1] = '\0';
    //a->ra->Read1 = &Read1;
    //a->ra->Qual1 = &Qual1;
    //a->ra->Lread = read_length;
    a->ra->readLength[0] = read_length;
    a->ra->readLengthOriginal[0] = read_length;
    //a->ra->readLength[1] = read_length;
    
    int readStatus = a->ra->oneRead();
    a->ra->readName[1] = '\0';
    if(readStatus != 0)
    {
        return "";
    }
    const char* str = a->ra->outputAlignments();
    return str;
}

const char* align_read_pair(Aligner* a, char *Read1, char *Qual1, char *Read2, char *Qual2, unsigned long long read_length)
{
    a->p->iReadAll++;
    a->ra->iRead++;
    //printf("iRead %llu\n", a->ra->iRead);
    a->p->readNmates = 2;
    a->ra->readNmates = 2;
    a->ra->Read0 = &Read1;
    a->ra->Qual0 = &Qual1;
    strcpy(a->ra->Read0[1], Read2);
    strcpy(a->ra->Qual0[1], Qual2);
    a->ra->readName = (char*)malloc(2);
    a->ra->readName[0] = 'a';
    a->ra->readName[1] = '\0';
    //a->ra->Read1 = &Read1;
    //a->ra->Qual1 = &Qual1;
    //a->ra->Lread = read_length;
    a->ra->readLength[0] = read_length;
    a->ra->readLengthOriginal[0] = read_length;
    //a->ra->readLength[1] = read_length;
    
    int readStatus = a->ra->oneRead();
    a->ra->readName[1] = '\0';
    if(readStatus != 0)
    {
        return "";
    }
    const char* str = a->ra->outputAlignments();
    return str;
}

Aligner* init_aligner_clone(Aligner* al)
{
    return new Aligner(al);
}

Aligner* init_aligner(int argc, char* argv[])
{
    return new Aligner(argc, argv);
}

void destroy_aligner(Aligner *a)
{
    delete a;
}
/*
int main()
{
    char* arr[] = {
            "STAR", "--genomeDir", "/mnt/opt/refdata_cellranger/GRCh38-3.0.0/star",
            "--outSAMmultNmax", "50",
            "--runThreadN", "1",
            "--readNameSeparator", "space",
            "--outSAMunmapped", "Within", "KeepPairs",
            "--outSAMtype", "SAM",
            "--outStd", "SAM",
            "--outSAMorder", "PairedKeepInputOrder",
    };
    int len = sizeof(arr) / sizeof(arr[0]);
    Aligner* a = init_aligner(len, arr);

    std::string line;
    std::ifstream infile("1.fastq");
    int lineNum = 0;
    char* curRead = (char*)malloc(500*sizeof(char));
    while(std::getline(infile, line))
    {
        if(lineNum%4 == 1)
        {
            strcpy(curRead, line.c_str());
        }
        else if(lineNum%4 == 3)
        {
            char* curQual = (char*)malloc(500*sizeof(char));
            strcpy(curQual, line.c_str());
            printf("read = %s\n", curRead);
            printf("qual = %s\n", curQual);

            const char* bam_line = align_read(a, curRead, curQual, line.length());
            printf("%s", bam_line);
            free(curQual);
        }
        lineNum++;
        //if(lineNum == 100) break;
    }
    free(curRead);
    destroy_aligner(a);
    return 0;
}
*/
