#include "tsCommon.h"
#include "tsTransportStream.h"
#include "tsPacketHeader.h"
#include "tsAdaptationField.h"
#include "pesPacketHeader.h"
#include "pesAssembler.h"

#define DEBUG 1


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
    xPES_Assembler      PES_Assembler_PID_136;
    xPES_Assembler      PES_Assembler_PID_174;

    const int PID136 = 136;
    const int PID174 = 174;
    int currentPID = 0;

    PES_Assembler_PID_136.Init(PID136, ".mp2");
    PES_Assembler_PID_174.Init(PID174, ".264");

    int32_t TS_PacketId = 0;

    while (!feof(TransportStreamFile))
    {
        size_t NumRead = fread(TS_PacketBuffer, 1, xTS::TS_PacketLength, TransportStreamFile);

        if (NumRead != xTS::TS_PacketLength) break;


        TS_PacketHeader.Reset();
        TS_PacketHeader.Parse(TS_PacketBuffer);

        TS_PacketAdaptationField.Reset();

        if (TS_PacketHeader.getSyncByte() == 'G' && (TS_PacketHeader.getPID() == PID136 || TS_PacketHeader.getPID() == PID174))
        {
            currentPID = TS_PacketHeader.getPID();
            switch (currentPID)
            {
            case (PID136) :
            {
                if (TS_PacketHeader.hasAdaptationField())
                {
                    #if DEBUG 
                    TS_PacketAdaptationField.Parse(TS_PacketBuffer, TS_PacketHeader.getAdaptationFieldControl()); 
                    #endif 
                }

                
                printf("%010d ", TS_PacketId);
                #if DEBUG
                TS_PacketHeader.Print();
                #endif
                if (TS_PacketHeader.hasAdaptationField())
                {
                    #if DEBUG
                    TS_PacketAdaptationField.Print();
                    #endif
                }

                xPES_Assembler::eResult Result = PES_Assembler_PID_136.AbsorbPacket(TS_PacketBuffer, &TS_PacketHeader, &TS_PacketAdaptationField);
                #if DEBUG
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
                    PES_Assembler_PID_136.PrintPESH();
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
                    printf("PES: PcktLen=%d ", PES_Assembler_PID_136.getPacketLength());
                    printf("HeadLen=%d ", PES_Assembler_PID_136.getHeaderLength());
                    printf("DataLen=%d ", PES_Assembler_PID_136.getDataLength());
                    break;
                }
                default:
                    break;
                }
                #endif
                break;
            } 
            case(PID174) :
            {
                if (TS_PacketHeader.hasAdaptationField())
                {
                    TS_PacketAdaptationField.Parse(TS_PacketBuffer, TS_PacketHeader.getAdaptationFieldControl());
                }

                printf("%010d ", TS_PacketId);
                #if DEBUG
                TS_PacketHeader.Print();
                #endif
                if (TS_PacketHeader.hasAdaptationField())
                {
                    #if DEBUG
                    TS_PacketAdaptationField.Print();
                    #endif
                }

                xPES_Assembler::eResult Result = PES_Assembler_PID_174.AbsorbPacket(TS_PacketBuffer, &TS_PacketHeader, &TS_PacketAdaptationField);
                #if DEBUG
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
                    PES_Assembler_PID_174.PrintPESH();
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
                    printf("PES: PcktLen=%d ", PES_Assembler_PID_174.getPacketLength());
                    printf("HeadLen=%d ", PES_Assembler_PID_174.getHeaderLength());
                    printf("DataLen=%d ", PES_Assembler_PID_174.getDataLength());
                    break;
                }
                default:
                    break;
                }
                #endif
                break;
            }
            }
            printf("\n");
        }
        TS_PacketId++;
    }
}