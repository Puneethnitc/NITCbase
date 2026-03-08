#include "OpenRelTable.h"
#include <cstring>
#include <cstdlib>
#include <stdio.h>

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable()
{

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i)
  {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
    tableMetaInfo[i].free=FREE;
  }
  
  /************ Setting up Relation Cache entries ************/
  // (we need to populate relation cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Relation Cache Table****/
  RecBuffer relCatBlock(RELCAT_BLOCK);

  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);

  struct RelCacheEntry relCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;
  relCacheEntry.dirty=false;
  // allocate this on the heap because we want it to persist outside this function
  RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry *)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;
  /**** setting up Attribute Catalog relation in the Relation Cache Table ****/

  // set up the relation cache entry for the attribute catalog similarly
  // from the record at RELCAT_SLOTNUM_FOR_ATTRCAT

  // set the value at RelCacheTable::relCache[ATTRCAT_RELID]

  RecBuffer attrCatBlock(ATTRCAT_BLOCK);

  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  relCatBlock.getRecord(attrCatRecord, RELCAT_SLOTNUM_FOR_ATTRCAT);
  struct RelCacheEntry attrCacheEntry;
  RelCacheTable::recordToRelCatEntry(attrCatRecord, &attrCacheEntry.relCatEntry);
  attrCacheEntry.recId.block = RELCAT_BLOCK;
  attrCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_ATTRCAT;
  attrCacheEntry.dirty=false;
  RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry *)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[ATTRCAT_RELID]) = attrCacheEntry;

  /************ Setting up Attribute cache entries ************/
  // (we need to populate attribute cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Attribute Cache Table ****/
  AttrCacheTable::attrCache[RELCAT_RELID] = nullptr;
  AttrCacheEntry *relPtr = nullptr;
  for (int i = 0; i <= 5; i++)
  {
    AttrCacheEntry *temp = (struct AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
    Attribute attrRecord[ATTRCAT_NO_ATTRS];
    attrCatBlock.getRecord(attrRecord, i);
    AttrCacheTable::recordToAttrCatEntry(attrRecord, &temp->attrCatEntry);
    temp->next = nullptr;
    temp->recId.block = ATTRCAT_BLOCK;
    temp->recId.slot = i;
    if (AttrCacheTable::attrCache[RELCAT_RELID] == nullptr)
    {
      (AttrCacheTable::attrCache[RELCAT_RELID]) = temp;
      relPtr = temp;
    }
    else
    {
      relPtr->next = temp;
      relPtr = temp;
    }
  }

  // iterate through all the attributes of the relation catalog and create a linked
  // list of AttrCacheEntry (slots 0 to 5)
  // for each of the entries, set
  //    attrCacheEntry.recId.block = ATTRCAT_BLOCK;
  //    attrCacheEntry.recId.slot = i   (0 to 5)
  //    and attrCacheEntry.next appropriately
  // NOTE: allocate each entry dynamically using malloc

  // set the next field in the last entry to nullptr

  // AttrCacheTable::attrCache[RELCAT_RELID] = /* head of the linked list */;

  /**** setting up Attribute Catalog relation in the Attribute Cache Table ****/

  // set up the attributes of the attribute cache similarly.
  // read slots 6-11 from attrCatBlock and initialise recId appropriately

  // set the value at AttrCacheTable::attrCache[ATTRCAT_RELID]
  AttrCacheTable::attrCache[ATTRCAT_RELID] = nullptr;
  AttrCacheEntry *attrPtr = nullptr;
  for (int i = 6; i <= 11; i++)
  {
    AttrCacheEntry *temp = (AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
    Attribute attrRecord[ATTRCAT_NO_ATTRS];
    attrCatBlock.getRecord(attrRecord, i);
    AttrCacheTable::recordToAttrCatEntry(attrRecord, &temp->attrCatEntry);
    temp->next = nullptr;
    temp->recId.block = ATTRCAT_BLOCK;
    temp->recId.slot = i;
    if (AttrCacheTable::attrCache[ATTRCAT_RELID] == nullptr)
    {
      (AttrCacheTable::attrCache[ATTRCAT_RELID]) = temp;
      attrPtr = temp;
    }
    else
    {
      attrPtr->next = temp;
      attrPtr = temp;
    }
  }

  tableMetaInfo[RELCAT_RELID]={OCCUPIED,RELCAT_RELNAME};
  tableMetaInfo[ATTRCAT_RELID]={OCCUPIED,ATTRCAT_RELNAME};
//   HeadInfo relCatHead;
//   relCatBlock.getHeader(&relCatHead);
//   bool inserted = false;
//   for (int k = 0; k < relCatHead.numEntries && !inserted; k++)
//   {
//     Attribute record[RELCAT_NO_ATTRS];
//     relCatBlock.getRecord(record, k);
//     if (strcmp(record[RELCAT_REL_NAME_INDEX].sVal, "Students") == 0)
//     {
//       for (int i = 2; i < MAX_OPEN; i++)
//       {
//         if (RelCacheTable::relCache[i] == nullptr)
//         {
//           struct RelCacheEntry relCacheEntryForStudent;
//           RelCacheTable::recordToRelCatEntry(record, &relCacheEntryForStudent.relCatEntry);
//           relCacheEntryForStudent.recId.block = RELCAT_BLOCK;
//           relCacheEntryForStudent.recId.slot = k;
//           RelCacheTable::relCache[i] = (struct RelCacheEntry *)malloc(sizeof(RelCacheEntry));
//           *(RelCacheTable::relCache[i]) = relCacheEntryForStudent;

//           AttrCacheTable::attrCache[i] = nullptr;
//           AttrCacheEntry *relPtr = nullptr;
//           HeadInfo attrCatHead;
//           attrCatBlock.getHeader(&attrCatHead);
//           for (int j = 0; j < attrCatHead.numEntries; j++)
//           {
//             Attribute catRecord[ATTRCAT_NO_ATTRS];
//             attrCatBlock.getRecord(catRecord, j);
//             if (strcmp(catRecord[ATTRCAT_REL_NAME_INDEX].sVal, "Students") == 0)
//             {
//               AttrCacheEntry *temp = (struct AttrCacheEntry *)malloc(sizeof(AttrCacheEntry));
//               AttrCacheTable::recordToAttrCatEntry(catRecord, &temp->attrCatEntry);
//               temp->next = nullptr;
//               temp->recId.block = ATTRCAT_BLOCK;
//               temp->recId.slot = j;
//               if (AttrCacheTable::attrCache[i] == nullptr)
//               {
//                 AttrCacheTable::attrCache[i] = temp;
//                 relPtr = temp;
//               }
//               else
//               {
//                 relPtr->next = temp;
//                 relPtr = temp;
//               }
//             }
//           }
//           inserted = true;
//           break;
//         }
//       }
//     }
//   }
// }
}
OpenRelTable::~OpenRelTable()
{
  // free all the memory that you allocated in the constructor
  for(int i=2;i<MAX_OPEN;i++)
  {
    if(tableMetaInfo[i].free==OCCUPIED)
    {
      OpenRelTable::closeRel(i);
    }
  }
  if(RelCacheTable::relCache[ATTRCAT_RELID]->dirty==true)
  {
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(ATTRCAT_RELID,&relCatEntry);
    Attribute record[RELCAT_NO_ATTRS];
    RelCacheTable::relCatEntryToRecord(&relCatEntry,record);
    RecBuffer relCatBlock(RelCacheTable::relCache[ATTRCAT_RELID]->recId.block);
    relCatBlock.setRecord(record,RelCacheTable::relCache[ATTRCAT_RELID]->recId.slot);
  }
 
      free(RelCacheTable::relCache[ATTRCAT_RELID]);
      RelCacheTable::relCache[ATTRCAT_RELID] = nullptr;
 
  if(RelCacheTable::relCache[RELCAT_RELID]->dirty==true)
  {
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(RELCAT_RELID,&relCatEntry);
    Attribute record[RELCAT_NO_ATTRS];
    RelCacheTable::relCatEntryToRecord(&relCatEntry,record);

    RecBuffer relCatBlock(RelCacheTable::relCache[RELCAT_RELID]->recId.block);
    relCatBlock.setRecord(record,RelCacheTable::relCache[RELCAT_RELID]->recId.slot);
  }
  free(RelCacheTable::relCache[RELCAT_RELID]);
  RelCacheTable::relCache[RELCAT_RELID]=nullptr;

  for (int i = 0; i <= ATTRCAT_RELID; i++)
  {
    if (AttrCacheTable::attrCache[i] != nullptr)
    {
      AttrCacheEntry *ptr = AttrCacheTable::attrCache[i];
      while (ptr)
      {
        AttrCacheEntry *temp = ptr;
        ptr = ptr->next;
        free(temp);
      }
      AttrCacheTable::attrCache[i] = nullptr;
    }
  }
}

// int OpenRelTable::getRelId(char relName[ATTR_SIZE])
// {

//    if(strcmp(relName,RELCAT_RELNAME)==0)
//    return RELCAT_RELID;
//     // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID
//     if(strcmp(relName,ATTRCAT_RELNAME)==0)
//     return ATTRCAT_RELID;
//   // for (int i = 0; i < MAX_OPEN; i++)
//   // {
//   //   if (RelCacheTable::relCache[i] != nullptr && strcmp(RelCacheTable::relCache[i]->relCatEntry.relName, relName) == 0)
//   //   {
//   //     return i;
//   //   }
//   // }
//   return E_RELNOTOPEN;
// }

int OpenRelTable::closeRel(int relId)
{
  if(relId==RELCAT_RELID||relId==ATTRCAT_RELID)
  return E_NOTPERMITTED;

  if(!(relId>=2&&relId<MAX_OPEN))
  return E_OUTOFBOUND;

  if(tableMetaInfo[relId].free==FREE)
  return E_RELNOTOPEN;
  
  if(RelCacheTable::relCache[relId]->dirty==true)
  {
    RelCatEntry relCatEntry;
    RelCacheTable::getRelCatEntry(relId,&relCatEntry);
    Attribute record[RELCAT_NO_ATTRS];
    RelCacheTable::relCatEntryToRecord(&relCatEntry,record);

    RecBuffer relCatBuffer(RelCacheTable::relCache[relId]->recId.block);
    relCatBuffer.setRecord(record,RelCacheTable::relCache[relId]->recId.slot);
    
  }
  free(RelCacheTable::relCache[relId]);
  RelCacheTable::relCache[relId]=nullptr;

 AttrCacheEntry* ptr=AttrCacheTable::attrCache[relId];
  while(ptr)
  {
    AttrCacheEntry *temp=ptr;
    ptr=ptr->next;
    free(temp);
    temp=nullptr;
  }
  AttrCacheTable::attrCache[relId]=nullptr;
  tableMetaInfo[relId].free=FREE;
  tableMetaInfo[relId].relName[0]='\0';

  return SUCCESS;
}

int OpenRelTable::getFreeOpenRelTableEntry()
{
  for(int i=2;i<MAX_OPEN;i++)
  {
    if(tableMetaInfo[i].free==FREE)
    return i;
  }
  return E_CACHEFULL;
}

int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

  /* traverse through the tableMetaInfo array,
    find the entry in the Open Relation Table corresponding to relName.*/

  // if found return the relation id, else indicate that the relation do not
  // have an entry in the Open Relation Table.
  for(int i=0;i<MAX_OPEN;i++)
  {
    if(tableMetaInfo[i].free==OCCUPIED&&strcmp(relName,tableMetaInfo[i].relName)==0)
    return i;
  }
  return E_RELNOTOPEN;
}

int OpenRelTable::openRel(char relName[ATTR_SIZE])
{
    if(getRelId(relName)!=E_RELNOTOPEN)
    {
      return getRelId(relName);
    }

    int relId=getFreeOpenRelTableEntry();
    if(relId==E_CACHEFULL)
    return E_CACHEFULL;
    int ret=RelCacheTable::resetSearchIndex(RELCAT_RELID);
    if(ret!=SUCCESS)
    {
      return ret;
    }
    Attribute rel;
    strcpy(rel.sVal,relName);
    RecId recId=BlockAccess::linearSearch(RELCAT_RELID,RELCAT_ATTR_RELNAME,rel,EQ);
    if(recId.block==-1 && recId.slot==-1)
    {
      return E_RELNOTEXIST;
    }

    RecBuffer relCatBuffer(recId.block);

    Attribute record[RELCAT_NO_ATTRS];
    relCatBuffer.getRecord(record,recId.slot);

    RelCatEntry recordCatEntry;
    RelCacheTable::recordToRelCatEntry(record,&recordCatEntry);

    RelCacheEntry recordCacheEntry;
    recordCacheEntry.relCatEntry=recordCatEntry;
    recordCacheEntry.recId=recId;
    recordCacheEntry.dirty=false;

    RelCacheTable::relCache[relId]=(RelCacheEntry *)malloc(sizeof(RelCacheEntry));
    *(RelCacheTable::relCache[relId])=recordCacheEntry;
    AttrCacheEntry* listhead=nullptr,*ptr=nullptr;
    RelCacheTable::resetSearchIndex(ATTRCAT_RELID);
    while(1)
    { 
      RecId attrCatRecordId;
        attrCatRecordId=BlockAccess::linearSearch(ATTRCAT_RELID,ATTRCAT_ATTR_RELNAME,rel,EQ);
        if(attrCatRecordId.block==-1&&attrCatRecordId.slot==-1)
        {
          break;
        }
        RecBuffer attrCatRecBuffer(attrCatRecordId.block);
        Attribute attrRecord[ATTRCAT_NO_ATTRS];
        attrCatRecBuffer.getRecord(attrRecord,attrCatRecordId.slot);

        AttrCacheEntry *temp=(AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
        AttrCacheTable::recordToAttrCatEntry(attrRecord,&temp->attrCatEntry);
        temp->recId=attrCatRecordId;
        temp->next=nullptr;
        temp->searchIndex.block=-1;
        temp->searchIndex.index=-1;
        if(listhead==nullptr)
        {
          listhead=temp;
          ptr=temp;
        }else{
          ptr->next=temp;
          ptr=temp;
        }

    }
    AttrCacheTable::attrCache[relId]=listhead;

    tableMetaInfo[relId].free=OCCUPIED;
    strcpy(tableMetaInfo[relId].relName,relName);
    return relId;
}