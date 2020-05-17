#include "tsCommon.h"
#include "tsTransportStream.h"
#include "tsPacketHeader.h"
#include "tsAdaptationField.h"
#include "pesPacketHeader.h"
#include "pesAssembler.h"


int main(int argc, char* argv[], char* envp[])
{
    const char* filename = "../example_new.ts";
    FILE* TransportStreamFile = fopen(filename, "rb");
    if (TransportStreamFile == NULL) 
    { 
        printf("wrong file name\n"); 
        return EXIT_FAILURE; 
    }

    uint8_t             TS_PacketBuffer[xTS::TS_PacketLength];
    xTS_PacketHeader    TS_PacketHeader;
    xTS_AdaptationField TS_PacketAdaptationField;
    xPES_Assembler      PES_Assembler;

    PES_Assembler.Init(136);

    int32_t TS_PacketId = 0;

    while (!feof(TransportStreamFile))
    {
        size_t NumRead = fread(TS_PacketBuffer, 1, xTS::TS_PacketLength, TransportStreamFile);

        if (NumRead != xTS::TS_PacketLength) break;


        TS_PacketHeader.Reset();
        TS_PacketHeader.Parse(TS_PacketBuffer);

        TS_PacketAdaptationField.Reset();

        if (TS_PacketHeader.getSyncByte() == 'G' && TS_PacketHeader.getPID() == 136)
        {
            if (TS_PacketHeader.hasAdaptationField())
            {
                TS_PacketAdaptationField.Parse(TS_PacketBuffer, TS_PacketHeader.getAdaptationFieldControl());
            }

            printf("%010d ", TS_PacketId);
            TS_PacketHeader.Print();

            if (TS_PacketHeader.hasAdaptationField()) 
            { 
                TS_PacketAdaptationField.Print(); 
            }

            xPES_Assembler::eResult Result = PES_Assembler.AbsorbPacket(TS_PacketBuffer, &TS_PacketHeader, &TS_PacketAdaptationField);

            switch (Result)
            {
            case xPES_Assembler::eResult::StreamPackedLost: 
            {
                printf("PcktLost ");
                break;
            }
            case xPES_Assembler::eResult::AssemblingStarted: 
            {
                printf("Started  ");
                PES_Assembler.PrintPESH();
                break;
            }
            case xPES_Assembler::eResult::AssemblingContinue: 
            {
                printf("Continue ");
                break;
            }
            case xPES_Assembler::eResult::AssemblingFinished: 
            {
                printf("Finished ");
                printf("PES: Len=%d", PES_Assembler.getNumPacketBytes());
                break;
            }
            default: 
                break;
            }
            printf("\n");
        }
        TS_PacketId++;
    }
}