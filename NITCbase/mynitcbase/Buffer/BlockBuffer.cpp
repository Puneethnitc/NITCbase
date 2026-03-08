#include "BlockBuffer.h"

#include <cstdlib>
#include <cstring>
BlockBuffer::BlockBuffer(int blockNum){
  this->blockNum=blockNum;
}

RecBuffer::RecBuffer() : BlockBuffer('R'){}
RecBuffer::RecBuffer(int blockNum):BlockBuffer::BlockBuffer(blockNum){

}

int BlockBuffer::getHeader(struct HeadInfo *head){
    

    unsigned char *bufferPtr;
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS)
    return ret;
     // populate the numEntries, numAttrs and numSlots fields in *head
  memcpy(&head->numSlots, bufferPtr + 24, 4);
  memcpy(&head->numEntries,bufferPtr+16, 4);
  memcpy(&head->numAttrs, bufferPtr+20,4);
  memcpy(&head->rblock,  bufferPtr+12,4);
  memcpy(&head->lblock,  bufferPtr+8,4);

  return SUCCESS;
}

int RecBuffer::getRecord(union Attribute *rec,int slotNum){
    struct HeadInfo head;

    unsigned char *bufferPtr;
    int ret=this->getHeader(&head);
    if(ret!=SUCCESS)
    return ret;
    ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS)
    return ret;
    int attrCount=head.numAttrs;
    int slotCount=head.numSlots;


    int recordSize = attrCount * ATTR_SIZE;
  unsigned char *slotPointer = bufferPtr+HEADER_SIZE+slotCount+(recordSize*slotNum);

  memcpy(rec,slotPointer,recordSize);
  return SUCCESS;
}

int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **bufferPtr)
{
    int bufferNum=StaticBuffer::getBufferNum(this->blockNum);
    if(bufferNum==E_OUTOFBOUND)
    {
      return E_OUTOFBOUND;
    }
    if(bufferNum!=E_BLOCKNOTINBUFFER)
    {
    
      for(int i=0;i<BUFFER_CAPACITY;i++)
      {
        if(i!=bufferNum && !StaticBuffer::metainfo[i].free)
        {
          StaticBuffer::metainfo[i].timeStamp++;
        }
      }
      StaticBuffer::metainfo[bufferNum].timeStamp=0;
    }
    else{
      bufferNum=StaticBuffer::getFreeBuffer(this->blockNum);
      if(bufferNum==E_OUTOFBOUND)
      return E_OUTOFBOUND;
      Disk::readBlock(StaticBuffer::blocks[bufferNum],this->blockNum);
    }
    *bufferPtr=StaticBuffer::blocks[bufferNum];
    

    return SUCCESS;
}
int RecBuffer::getSlotMap(unsigned char *slotMap)
{
  unsigned char *bufferPtr;
  int ret=loadBlockAndGetBufferPtr(&bufferPtr);
  if(ret!=SUCCESS)
  return ret;

  struct HeadInfo head;
 ret= getHeader(&head);
  if(ret!=SUCCESS)
  return ret;
  int slotCount=head.numSlots;
  unsigned char *slotMapInBuffer=bufferPtr+HEADER_SIZE;
  memcpy(slotMap,slotMapInBuffer,slotCount);
  return SUCCESS;
}

int compareAttrs( Attribute attr1, Attribute attr2,int attrType){
  double diff;
  if(attrType==STRING)
  {
    diff=strcmp(attr1.sVal,attr2.sVal);
  }
  else{
    diff=attr1.nVal-attr2.nVal;
  }
  if(diff>0)
  return 1;
  if(diff==0)
  return 0;
  return -1;
}

int RecBuffer::setRecord(union Attribute *rec,int slotnum)
{
  unsigned char *bufferptr;
  int ret=loadBlockAndGetBufferPtr(&bufferptr);
  if(ret!=SUCCESS)
  return ret;

  HeadInfo head;
  this->getHeader(&head);
  int numattr=head.numAttrs;
  int numslots=head.numSlots;

  if(slotnum<0||slotnum>=numslots)
  return E_OUTOFBOUND;

  memcpy(bufferptr+HEADER_SIZE+numslots+(slotnum*numattr*ATTR_SIZE),rec,ATTR_SIZE*numattr);

  StaticBuffer::setDirtyBit(this->blockNum);
  return SUCCESS;
}

int BlockBuffer::setHeader(struct HeadInfo *head)
{
  unsigned char *bufferptr;
 int ret= loadBlockAndGetBufferPtr(&bufferptr);
  if(ret!=SUCCESS)
  {
    return ret;
  }
  struct HeadInfo *bufferHeader=(struct HeadInfo *)bufferptr;
  bufferHeader->blockType=head->blockType;
  bufferHeader->lblock=head->lblock;
  bufferHeader->numAttrs=head->numAttrs;
  bufferHeader->numEntries=head->numEntries;
  bufferHeader->numSlots=head->numSlots;
  bufferHeader->pblock=head->pblock;
  bufferHeader->rblock=head->rblock;
  
  ret=StaticBuffer::setDirtyBit(this->blockNum);
  if(ret!=SUCCESS)
  return ret;
  return SUCCESS;
}

int BlockBuffer::setBlockType(int blockType)
{
  unsigned char *bufferPtr;
  int ret=loadBlockAndGetBufferPtr(&bufferPtr);
  if(ret!=SUCCESS)
  return ret;

  *((int32_t *)bufferPtr) = blockType;
  StaticBuffer::blockAllocMap[this->blockNum]=blockType;
  ret=StaticBuffer::setDirtyBit(this->blockNum);
  if(ret!=SUCCESS)
  return ret;
  return SUCCESS;
}

int BlockBuffer::getFreeBlock(int blockType)
{ 
  int free=-1;
  for(int i=0;i<DISK_BLOCKS;i++)
  {
    if(StaticBuffer::blockAllocMap[i]==UNUSED_BLK)
    {
      free=i;
      break;
    }
  }
  if(free==-1)
  return E_DISKFULL;

  this->blockNum=free;
  int bufferNum=StaticBuffer::getFreeBuffer(this->blockNum);
  struct HeadInfo head;
  head.lblock=-1;
  head.rblock=-1;
  head.pblock=-1;
  head.numAttrs=0;
  head.numEntries=0;
  head.numSlots=0;
  int ret=BlockBuffer::setHeader(&head);
  ret=BlockBuffer::setBlockType(blockType);

  return this->blockNum;
}
BlockBuffer::BlockBuffer(char blockType)
{
    int ret=E_DISKFULL;
    if(blockType=='R')
    ret=getFreeBlock(REC);
    else if(blockType=='I')
    ret=getFreeBlock(IND_INTERNAL);
    else if(blockType=='L')
    ret=getFreeBlock(IND_LEAF);

   this->blockNum=ret;
}



int RecBuffer::setSlotMap(unsigned char *slotMap)
{
  unsigned char *bufferPtr;
 int ret=loadBlockAndGetBufferPtr(&bufferPtr);
 if(ret!=SUCCESS)
 return ret;

 struct HeadInfo head;
 getHeader(&head);
 int numSlots=head.numSlots;
 memcpy(bufferPtr+HEADER_SIZE,slotMap,numSlots);
  ret=StaticBuffer::setDirtyBit(this->blockNum);
 if(ret!=SUCCESS)
 return ret;
 return SUCCESS;
}

int BlockBuffer::getBlockNum()
{
  return this->blockNum;
}

void BlockBuffer::releaseBlock()
{
  if(this->blockNum==INVALID_BLOCKNUM)
  return;
  else{
    int bufferNum=StaticBuffer::getBufferNum(this->blockNum);
    if(bufferNum!=E_BLOCKNOTINBUFFER)
    { 
      StaticBuffer::metainfo[bufferNum].free=true;
    }
    StaticBuffer::blockAllocMap[this->blockNum]=UNUSED_BLK;
  
    this->blockNum=INVALID_BLOCKNUM;
  }

}