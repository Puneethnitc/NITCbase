#include "StaticBuffer.h"
unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS];
StaticBuffer::StaticBuffer()
{   

    for(int i=0;i<BLOCK_ALLOCATION_MAP_SIZE;i++)
    {
        Disk::readBlock(blockAllocMap+(i*BLOCK_SIZE),i);
    }

    for(int i=0;i<BUFFER_CAPACITY;i++){
        metainfo[i].dirty=false;
        metainfo[i].free=true;
        metainfo[i].timeStamp=-1;
        metainfo[i].blockNum=-1;
    }

}

StaticBuffer::~StaticBuffer(){
    for(int i=0;i<BLOCK_ALLOCATION_MAP_SIZE;i++)
    {
        Disk::writeBlock(blockAllocMap+(i*BLOCK_SIZE),i);
    }
    for(int i=0;i<BUFFER_CAPACITY;i++){
        if(metainfo[i].free==false&&metainfo[i].dirty==true)
        {
            Disk::writeBlock(blocks[i],metainfo[i].blockNum);
        }
    }
}

// int StaticBuffer::getStaticBlockType(int blockNum){
//     if(blockNum<0||blockNum>=DISK_BLOCKS){
//         return E_OUTOFBOUND;
//     }

//     int blockType=(int)blockAllocMap[blockNum];
//     return blockType;
// }

int StaticBuffer::getBufferNum(int blockNum){
    if(blockNum<0||blockNum>=DISK_BLOCKS)
    return E_OUTOFBOUND;

    for(int i=0;i<BUFFER_CAPACITY;i++)
    {
        if(metainfo[i].blockNum==blockNum)
        return i;
    }
    return E_BLOCKNOTINBUFFER;
}
// int StaticBuffer::setDirtyBit(int blockNum){
//    int index=getBufferNum(blockNum);
   
//    if(index==E_OUTOFBOUND)
//    return E_OUTOFBOUND;

//    if(index==E_BLOCKNOTINBUFFER)
//    return E_BLOCKNOTINBUFFER;

//    metainfo[index].dirty=true;
//    return SUCCESS;
// }


int StaticBuffer::getFreeBuffer(int blockNum) {
    if(blockNum<0||blockNum>=DISK_BLOCKS)
    return E_OUTOFBOUND;

    for(int i=0;i<BUFFER_CAPACITY;i++)
    {
        if(metainfo[i].free==false)
        {
            metainfo[i].timeStamp++;
        }
    }

    int buffernum=-1;
    for(int i=0;i<BUFFER_CAPACITY;i++)
    {
        if(metainfo[i].free==true)
        {
            buffernum=i;
            break;
        }
    }

    int lar=-1;
    int index=-1;
    if(buffernum==-1){ 
    for(int i=0;i<BUFFER_CAPACITY;i++)
    {
        if(metainfo[i].free==false&&metainfo[i].timeStamp>lar)
        {
            lar=metainfo[i].timeStamp;
            index=i;
        }
    }

    if(metainfo[index].dirty==true)
    {
        Disk::writeBlock(blocks[index],metainfo[index].blockNum);
    }
    buffernum=index;
}
    metainfo[buffernum].blockNum=blockNum;
    metainfo[buffernum].dirty=false;
    metainfo[buffernum].free=false;
    metainfo[buffernum].timeStamp=0;
    return buffernum;
}

int StaticBuffer::setDirtyBit(int blockNum)
{
    int buffernum=getBufferNum(blockNum);
   if(buffernum==E_OUTOFBOUND)
    return buffernum;

    if(buffernum==E_BLOCKNOTINBUFFER)
    return E_BLOCKNOTINBUFFER;

    metainfo[buffernum].dirty=true;
    return SUCCESS;
}