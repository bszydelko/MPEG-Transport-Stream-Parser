#include "tsCommon.h"
#include "tsTransportStream.h"


int main( int argc, char *argv[ ], char *envp[ ])
{
    const char* filename = "../example_new.ts";
    FILE* TS_fileHandler = fopen(filename, "rb");
    if (!TS_fileHandler)
        return 0;

    uint8_t* TS_PacketBuffer = new uint8_t[xTS::TS_PacketLength];

    xTS_PacketHeader    TS_PacketHeader;
    xTS_AdaptationField TS_AdaptationField;
    
    int32_t TS_PacketId = 0;
    while(!feof(TS_fileHandler))
    {
    
        TS_PacketHeader.Reset();
        TS_AdaptationField.Reset();
        
        fread(TS_PacketBuffer, xTS::TS_PacketLength, 1, TS_fileHandler);

        TS_PacketHeader.Parse(TS_PacketBuffer);

        
         printf("%010d ", TS_PacketId);
         TS_PacketHeader.Print();

         if (TS_PacketHeader.hasAdaptationField())
         {
             TS_AdaptationField.Parse(TS_PacketBuffer, TS_PacketHeader.getAFC());
             TS_AdaptationField.Print();
         }
         printf("\n");
        
         TS_PacketId++;
    }
}