#include "SdioCard.h"

bool SdioCard::begin()
{
}

uint32_t SdioCard::cardCapacity()
{
}

bool SdioCard::erase(uint32_t firstBlock, uint32_t lastBlock)
{
    
}

uint8_t SdioCard::errorCode()
{
    
}

uint32_t SdioCard::errorData()
{
}

uint32_t SdioCard::errorLine()
{
}

bool SdioCard::isBusy()
{
    
}

uint32_t SdioCard::kHzSdClk()
{
}

bool SdioCard::readBlock(uint32_t lba, uint8_t* dst)
{
}

bool SdioCard::SdioCard::readBlocks(uint32_t lba, uint8_t* dst, size_t nb)
{
}

bool SdioCard::readCID(void* cid)
{
}

bool SdioCard::readCSD(void* csd)
{
}

bool SdioCard::readData(uint8_t *dst)
{
}

bool SdioCard::readOCR(uint32_t* ocr)
{
    
}

bool SdioCard::readStart(uint32_t lba)
{
}

bool SdioCard::readStart(uint32_t lba, uint32_t count)
{
}

bool SdioCard::readStop()
{
}

bool SdioCard::syncBlocks()
{
}

uint8_t SdioCard::type()
{
}

bool SdioCard::writeBlock(uint32_t lba, const uint8_t* src)
{
}

bool SdioCard::writeBlocks(uint32_t lba, const uint8_t* src, size_t nb)
{
}

bool SdioCard::writeData(const uint8_t* src)
{
}

bool SdioCard::writeStart(uint32_t lba)
{
}

bool SdioCard::writeStart(uint32_t lba, uint32_t count)
{
}

bool SdioCard::writeStop()
{
    
}
