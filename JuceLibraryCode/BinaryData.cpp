/* ==================================== JUCER_BINARY_RESOURCE ====================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#include <cstring>

namespace BinaryData
{

//================== small.png ==================
static const unsigned char temp_binary_data_0[] =
{ 137,80,78,71,13,10,26,10,0,0,0,13,73,72,68,82,0,0,0,16,0,0,0,16,8,6,0,0,0,31,243,255,97,0,0,0,1,115,82,71,66,0,174,206,28,233,0,0,0,4,103,65,77,65,0,0,177,143,11,252,97,5,0,0,0,9,112,72,89,115,0,0,18,116,0,0,18,116,1,222,102,31,120,0,0,0,140,73,68,65,
84,56,79,205,142,59,10,192,32,16,5,115,54,123,15,96,239,41,60,128,199,178,247,22,246,22,73,26,193,202,176,194,147,184,152,95,17,112,97,138,253,188,209,101,97,149,115,46,41,165,10,102,232,105,215,95,179,194,161,181,182,66,1,2,61,23,119,117,14,11,33,42,
33,132,10,122,173,117,137,49,142,37,52,164,165,49,166,5,240,50,122,218,221,10,128,247,190,40,165,90,80,74,89,156,115,109,127,41,64,224,137,91,193,182,238,13,4,248,108,114,193,27,254,21,240,239,142,102,147,11,222,112,41,248,2,114,7,163,46,23,165,233,160,
240,149,0,0,0,0,73,69,78,68,174,66,96,130,0,0 };

const char* small_png = (const char*) temp_binary_data_0;


const char* getNamedResource (const char* resourceNameUTF8, int& numBytes);
const char* getNamedResource (const char* resourceNameUTF8, int& numBytes)
{
    unsigned int hash = 0;

    if (resourceNameUTF8 != nullptr)
        while (*resourceNameUTF8 != 0)
            hash = 31 * hash + (unsigned int) *resourceNameUTF8++;

    switch (hash)
    {
        case 0x9f449af1:  numBytes = 247; return small_png;
        default: break;
    }

    numBytes = 0;
    return nullptr;
}

const char* namedResourceList[] =
{
    "small_png"
};

const char* originalFilenames[] =
{
    "small.png"
};

const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8)
{
    for (unsigned int i = 0; i < (sizeof (namedResourceList) / sizeof (namedResourceList[0])); ++i)
        if (strcmp (namedResourceList[i], resourceNameUTF8) == 0)
            return originalFilenames[i];

    return nullptr;
}

}
