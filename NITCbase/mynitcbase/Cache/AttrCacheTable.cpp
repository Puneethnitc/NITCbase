#include "AttrCacheTable.h"

#include <cstring>
AttrCacheEntry* AttrCacheTable::attrCache[MAX_OPEN];

/* returns the attrOffset-th attribute for the relation corresponding to relId
NOTE: this function expects the caller to allocate memory for `*attrCatBuf`
*/
int AttrCacheTable::getAttrCatEntry(int relId, int attrOffset, AttrCatEntry* attrCatBuf) {
  // check if 0 <= relId < MAX_OPEN and return E_OUTOFBOUND otherwise
    if(relId<0||relId>=MAX_OPEN)
    return E_OUTOFBOUND;


  // check if attrCache[relId] == nullptr and return E_RELNOTOPEN if true
    if(attrCache[relId]==nullptr)
    return E_RELNOTOPEN;


  // traverse the linked list of attribute cache entries
  for (AttrCacheEntry* entry = attrCache[relId]; entry != nullptr; entry = entry->next) {
    if (entry->attrCatEntry.offset == attrOffset) {

      // copy entry->attrCatEntry to *attrCatBuf and return SUCCESS;
        *attrCatBuf=entry->attrCatEntry;
        return SUCCESS;
    }
  }

  // there is no attribute at this offset
  return E_ATTRNOTEXIST;
}
int AttrCacheTable::getAttrCatEntry(int relId, char attrName[ATTR_SIZE], AttrCatEntry *attrCatBuf)
{
  if(relId<0||relId>=MAX_OPEN)
  return E_OUTOFBOUND;

  if(attrCache[relId]==nullptr)
  return E_RELNOTOPEN;

  for(AttrCacheEntry* i=attrCache[relId];i!=nullptr;i=i->next)
  {
    if(strcmp(i->attrCatEntry.attrName,attrName)==0)
    {
      *attrCatBuf=i->attrCatEntry;
      return SUCCESS;
    }
  }
  return E_ATTRNOTEXIST;
}
/* Converts a attribute catalog record to AttrCatEntry struct
    We get the record as Attribute[] from the BlockBuffer.getRecord() function.
    This function will convert that to a struct AttrCatEntry type.
*/
void AttrCacheTable::recordToAttrCatEntry(union Attribute record[ATTRCAT_NO_ATTRS],AttrCatEntry* attrCatEntry) {
  strcpy(attrCatEntry->relName, record[ATTRCAT_REL_NAME_INDEX].sVal);

  strcpy(attrCatEntry->attrName,record[ATTRCAT_ATTR_NAME_INDEX].sVal);

  attrCatEntry->attrType=record[ATTRCAT_ATTR_TYPE_INDEX].nVal;

  attrCatEntry->offset=record[ATTRCAT_OFFSET_INDEX].nVal;

  attrCatEntry->primaryFlag=record[ATTRCAT_PRIMARY_FLAG_INDEX].nVal;

  attrCatEntry->rootBlock=record[ATTRCAT_ROOT_BLOCK_INDEX].nVal;
  // copy the rest of the fields in the record to the attrCacheEntry struct
}



int AttrCacheTable::getSearchIndex(int relId,char attrName[ATTR_SIZE],IndexId *SearchIndex)
{ 
  if(relId<0||relId>=MAX_OPEN)
  {
    return E_OUTOFBOUND;
  }
  if(AttrCacheTable::attrCache[relId]==nullptr)
  {
    return E_RELNOTOPEN;
  }
  AttrCacheEntry *attrCachePtr=attrCache[relId];
  while(attrCachePtr)
  {
    if(strcmp(attrCachePtr->attrCatEntry.attrName,attrName)==0)
    {
      *SearchIndex=attrCachePtr->searchIndex;
      return SUCCESS;
    }
    attrCachePtr=attrCachePtr->next;
  }
  return E_ATTRNOTEXIST;
}

int AttrCacheTable::getSearchIndex(int relId,int attrOffset,IndexId *searchIndex)
{
  if(relId<0||relId>=MAX_OPEN)
  {
    return E_OUTOFBOUND;
  }
  if(AttrCacheTable::attrCache[relId]==nullptr)
  {
    return E_RELNOTOPEN;
  }
  AttrCacheEntry *attrCachePtr=attrCache[relId];
  while(attrCachePtr)
  {
    if(attrCachePtr->attrCatEntry.offset==attrOffset)
    {
      *searchIndex=attrCachePtr->searchIndex;
      return SUCCESS;
    }
    attrCachePtr=attrCachePtr->next;
  }
  return E_ATTRNOTEXIST;
}

int AttrCacheTable::setSearchIndex(int relId,char attrName[ATTR_SIZE],IndexId *searchIndex)
{
  if(relId<0||relId>+MAX_OPEN)
  return E_OUTOFBOUND;

  if(attrCache[relId]==nullptr)
  return E_RELNOTOPEN;

  AttrCacheEntry *attrCachePtr=attrCache[relId];
  while(attrCachePtr)
  {
    if(strcmp(attrCachePtr->attrCatEntry.attrName,attrName)==0)
    {
      attrCachePtr->searchIndex=*searchIndex;
      return SUCCESS;
    }
  }
  return E_ATTRNOTEXIST;
}

int AttrCacheTable::setSearchIndex(int relId,int attrOffset,IndexId *searchIndex)
{
  if(relId<0||relId>+MAX_OPEN)
  return E_OUTOFBOUND;

  if(attrCache[relId]==nullptr)
  return E_RELNOTOPEN;

  AttrCacheEntry *attrCachePtr=attrCache[relId];
  while(attrCachePtr)
  {
    if(attrOffset==attrCachePtr->attrCatEntry.offset)
    {
      attrCachePtr->searchIndex=*searchIndex;
      return SUCCESS;
    }
  }
  return E_ATTRNOTEXIST;
}

int AttrCacheTable::resetSearchIndex(int relId,int attrOffset)
{ 
  IndexId searchIndex={-1,-1};
  int retVal=AttrCacheTable::setSearchIndex(relId,attrOffset,&searchIndex);
  return retVal;
}

int AttrCacheTable::resetSearchIndex(int relId,char attrName[ATTR_SIZE])
{
  IndexId searchIndex={-1,-1};
  int retVal=AttrCacheTable::setSearchIndex(relId,attrName,&searchIndex);
  return retVal;
}

