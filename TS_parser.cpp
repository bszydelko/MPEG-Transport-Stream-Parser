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

  int32_t TS_PacketId = 0;
  while(!feof(TS_fileHandler))
  {

      TS_PacketHeader.Reset();
      fread(TS_PacketBuffer, xTS::TS_PacketLength, 1, TS_fileHandler);

    TS_PacketHeader.Parse(TS_PacketBuffer);

    printf("%010d ", TS_PacketId);
    TS_PacketHeader.Print();
    printf("\n");

    TS_PacketId++;
  }
}