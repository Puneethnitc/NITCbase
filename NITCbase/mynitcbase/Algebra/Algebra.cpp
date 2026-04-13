#include "Algebra.h"
#include "../counter.h"
#include <cstring>
#include<stdio.h>
#include<cstdlib>
/* used to select all the records that satisfy a condition.
the arguments of the function are
- srcRel - the source relation we want to select from
- targetRel - the relation we want to select into. (ignore for now)
- attr - the attribute that the condition is checking
- op - the operator of the condition
- strVal - the value that we want to compare against (represented as a string)
*/
bool isNumber(char *str);

int Algebra::select(char srcRel[ATTR_SIZE], char targetRel[ATTR_SIZE], char attr[ATTR_SIZE], int op, char strVal[ATTR_SIZE]) {
  int srcRelId = OpenRelTable::getRelId(srcRel);
  if (srcRelId == E_RELNOTOPEN) {
    return E_RELNOTOPEN;
  }

  AttrCatEntry attrCatEntry;
  int retVal = AttrCacheTable::getAttrCatEntry(srcRelId, attr, &attrCatEntry);
  if (retVal != SUCCESS) {
    return retVal;
  }

  Attribute attrVal;
  if (attrCatEntry.attrType == NUMBER) {
    if (isNumber(strVal)) {
      attrVal.nVal = atof(strVal);
    } else {
      return E_ATTRTYPEMISMATCH;
    }
  } else if (attrCatEntry.attrType == STRING) {
    strcpy(attrVal.sVal, strVal);
  }

  RelCatEntry relCatEntry;
  retVal = RelCacheTable::getRelCatEntry(srcRelId, &relCatEntry);
  if (retVal != SUCCESS) {
    return retVal;
  }

  int src_nAttrs = relCatEntry.numAttrs;
  char attr_names[src_nAttrs][ATTR_SIZE];
  int attr_types[src_nAttrs];

  for (int i = 0; i < src_nAttrs; i++) {
    AttrCatEntry attrCatBuf;
    retVal = AttrCacheTable::getAttrCatEntry(srcRelId, i, &attrCatBuf);
    if (retVal != SUCCESS) {
      return retVal;
    }
    strcpy(attr_names[i], attrCatBuf.attrName);
    attr_types[i] = attrCatBuf.attrType;
  }

  retVal = Schema::createRel(targetRel, src_nAttrs, attr_names, attr_types);
  if (retVal != SUCCESS) {
    return retVal;
  }

  int targetRelId = OpenRelTable::openRel(targetRel);
  if (targetRelId < 0) {
    Schema::deleteRel(targetRel);
    return targetRelId;
  }

  Attribute record[src_nAttrs];
  RelCacheTable::resetSearchIndex(srcRelId);
  AttrCacheTable::resetSearchIndex(srcRelId, attr);

  while (BlockAccess::search(srcRelId, record, attr, attrVal, op) == SUCCESS) {
    retVal = BlockAccess::insert(targetRelId, record);
    if (retVal != SUCCESS) {
      Schema::closeRel(targetRel);
      Schema::deleteRel(targetRel);
      return retVal;
    }
  }

  Schema::closeRel(targetRel);
  return SUCCESS;
}


// will return if a string can be parsed as a floating point number
bool isNumber(char *str) {
  int len;
  float ignore;
  /*
    sscanf returns the number of elements read, so if there is no float matching
    the first %f, ret will be 0, else it'll be 1

    %n gets the number of characters read. this scanf sequence will read the
    first float ignoring all the whitespace before and after. and the number of
    characters read that far will be stored in len. if len == strlen(str), then
    the string only contains a float with/without whitespace. else, there's other
    characters.
  */
  int ret = sscanf(str, "%f %n", &ignore, &len);
  return ret == 1 && len == strlen(str);
}
int Algebra::insert(char relName[ATTR_SIZE],int nAttrs,char record[][ATTR_SIZE])
{
  if(strcmp(relName,RELCAT_RELNAME)==0)
  return E_NOTPERMITTED;
  if(strcmp(relName,ATTRCAT_RELNAME)==0)
  return E_NOTPERMITTED;

  int relId=OpenRelTable::getRelId(relName);
  if(relId==E_RELNOTOPEN)
  return relId;

  RelCatEntry relCatEntry;
  RelCacheTable::getRelCatEntry(relId,&relCatEntry);
  if(relCatEntry.numAttrs!=nAttrs)
  return E_NATTRMISMATCH;

  Attribute recordValues[nAttrs];

  for(int i=0;i<nAttrs;i++)
  { AttrCatEntry attrCatBuf;
    AttrCacheTable::getAttrCatEntry(relId,i,&attrCatBuf);
    int type=attrCatBuf.attrType;
    if(type==NUMBER)
    {
      if(isNumber(record[i]))
      {
        recordValues[i].nVal=atof(record[i]);
      }
      else
      return E_ATTRTYPEMISMATCH;
    }
    else if(type==STRING){
        strcpy(recordValues[i].sVal,record[i]);
    }
  }
  int retVal=BlockAccess::insert(relId,recordValues);
  return retVal;
}


int Algebra::project(char srcRel[ATTR_SIZE],char targetRel[ATTR_SIZE]){
  int srcRelId=OpenRelTable::getRelId(srcRel);
  if(srcRelId==E_RELNOTOPEN)
  return E_RELNOTOPEN;

  RelCatEntry relCatBufSrc;
  RelCacheTable::getRelCatEntry(srcRelId,&relCatBufSrc);
  int numAttrsSrc=relCatBufSrc.numAttrs;

  char attrNames[numAttrsSrc][ATTR_SIZE];
  int attrTypes[numAttrsSrc];

  for(int i=0;i<numAttrsSrc;i++)
  { 
    AttrCatEntry attrCatBuf;
    AttrCacheTable::getAttrCatEntry(srcRelId,i,&attrCatBuf);
    strcpy(attrNames[i],attrCatBuf.attrName);
    attrTypes[i]=attrCatBuf.attrType;
  }

 int retVal=Schema::createRel(targetRel,numAttrsSrc,attrNames,attrTypes);
  if(retVal!=SUCCESS)
  return retVal;

  int targetRelId=OpenRelTable::openRel(targetRel);
  if(targetRelId<0)
  {
    Schema::deleteRel(targetRel);
    return targetRelId;
  }

  RelCacheTable::resetSearchIndex(srcRelId);
  Attribute record[numAttrsSrc];
  while(BlockAccess::project(srcRelId,record)==SUCCESS)
  {
    retVal=BlockAccess::insert(targetRelId,record);
    if(retVal!=SUCCESS)
    {
      Schema::closeRel(targetRel);
      Schema::deleteRel(targetRel);
      return retVal;
    }
  }
  Schema::closeRel(targetRel);
  return SUCCESS;
} 

int Algebra::project(char srcRel[ATTR_SIZE],char targetRel[ATTR_SIZE],int tar_nAttrs,char tar_Attrs[][ATTR_SIZE])
{
    int srcRelId=OpenRelTable::getRelId(srcRel);
    if(srcRelId==E_RELNOTOPEN)
    {
      return E_RELNOTOPEN;
    }

    RelCatEntry srcRelCatEntry;
    RelCacheTable::getRelCatEntry(srcRelId,&srcRelCatEntry);
    int srcNumOfAttrs=srcRelCatEntry.numAttrs;

    int attr_off[tar_nAttrs];
    int attr_types[tar_nAttrs];

    for(int i=0;i<tar_nAttrs;i++)
    {
        AttrCatEntry attrCatEntry;
      int retVal =AttrCacheTable::getAttrCatEntry(srcRelId,tar_Attrs[i],&attrCatEntry);
      if(retVal!=SUCCESS)
      return retVal;
      else{
        attr_off[i]=attrCatEntry.offset;
        attr_types[i]=attrCatEntry.attrType;
      }
    }

    int retVal=Schema::createRel(targetRel,tar_nAttrs,tar_Attrs,attr_types);
    if(retVal!=SUCCESS)
    return retVal;

    int tarRelId=OpenRelTable::openRel(targetRel);
    if(tarRelId<0){ 
      Schema::deleteRel(targetRel);
    return tarRelId;
    }

    RelCacheTable::resetSearchIndex(srcRelId);
    Attribute record[srcNumOfAttrs];

    while(BlockAccess::project(srcRelId,record)==SUCCESS)
    {
      Attribute proj_record[tar_nAttrs];
      for(int i=0;i<tar_nAttrs;i++)
      proj_record[i]=record[attr_off[i]];
          retVal=BlockAccess::insert(tarRelId,proj_record);
          if(retVal!=SUCCESS)
          {
            Schema::closeRel(targetRel);
            Schema::deleteRel(targetRel);
            return retVal;
          }
        
    }
    Schema::closeRel(targetRel);
    return SUCCESS;
}


int Algebra::join(char srcRelation1[ATTR_SIZE], char srcRelation2[ATTR_SIZE],
                  char targetRelation[ATTR_SIZE],
                  char attribute1[ATTR_SIZE], char attribute2[ATTR_SIZE]) {

    // get the srcRelation1's rel-id
    int srcRelId1 = OpenRelTable::getRelId(srcRelation1);

    // get the srcRelation2's rel-id
    int srcRelId2 = OpenRelTable::getRelId(srcRelation2);

    // if either relation not open
    if (srcRelId1 == E_RELNOTOPEN || srcRelId2 == E_RELNOTOPEN)
        return E_RELNOTOPEN;

    AttrCatEntry attrCatEntry1, attrCatEntry2;

    // get attribute entries
    int ret = AttrCacheTable::getAttrCatEntry(srcRelId1, attribute1, &attrCatEntry1);
    if (ret != SUCCESS) return E_ATTRNOTEXIST;

    ret = AttrCacheTable::getAttrCatEntry(srcRelId2, attribute2, &attrCatEntry2);
    if (ret != SUCCESS) return E_ATTRNOTEXIST;

    // type check
    if (attrCatEntry1.attrType != attrCatEntry2.attrType)
        return E_ATTRTYPEMISMATCH;

    // get relation catalog entries
    RelCatEntry relCatEntry1, relCatEntry2;

    ret = RelCacheTable::getRelCatEntry(srcRelId1, &relCatEntry1);
    if (ret != SUCCESS) return ret;

    ret = RelCacheTable::getRelCatEntry(srcRelId2, &relCatEntry2);
    if (ret != SUCCESS) return ret;

    int numOfAttributes1 = relCatEntry1.numAttrs;
    int numOfAttributes2 = relCatEntry2.numAttrs;

    // check duplicate attribute names (except join attributes)
    for (int i = 0; i < numOfAttributes1; i++) {
        AttrCatEntry a1;
        AttrCacheTable::getAttrCatEntry(srcRelId1, i, &a1);

        for (int j = 0; j < numOfAttributes2; j++) {
            AttrCatEntry a2;
            AttrCacheTable::getAttrCatEntry(srcRelId2, j, &a2);

            if (strcmp(a1.attrName, a2.attrName) == 0 &&
                strcmp(a1.attrName, attribute1) != 0 &&
                strcmp(a2.attrName, attribute2) != 0) {
                return E_DUPLICATEATTR;
            }
        }
    }

    // if no index on attr2 → create it
    if (attrCatEntry2.rootBlock == -1) {
        ret = BPlusTree::bPlusCreate(srcRelId2, attribute2);
        if (ret != SUCCESS) return ret;
    }

    int numOfAttributesInTarget = numOfAttributes1 + numOfAttributes2 - 1;

    char targetRelAttrNames[numOfAttributesInTarget][ATTR_SIZE];
    int targetRelAttrTypes[numOfAttributesInTarget];

    int k = 0;

    // copy attributes from relation1
    for (int i = 0; i < numOfAttributes1; i++) {
        AttrCatEntry a;
        AttrCacheTable::getAttrCatEntry(srcRelId1, i, &a);

        strcpy(targetRelAttrNames[k], a.attrName);
        targetRelAttrTypes[k] = a.attrType;
        k++;
    }

    // copy attributes from relation2 (except join attribute)
    for (int i = 0; i < numOfAttributes2; i++) {
        AttrCatEntry a;
        AttrCacheTable::getAttrCatEntry(srcRelId2, i, &a);

        if (strcmp(a.attrName, attribute2) == 0)
            continue;

        strcpy(targetRelAttrNames[k], a.attrName);
        targetRelAttrTypes[k] = a.attrType;
        k++;
    }

    // create target relation
    ret = Schema::createRel(targetRelation, numOfAttributesInTarget,
                            targetRelAttrNames, targetRelAttrTypes);
    if (ret != SUCCESS) return ret;

    // open target relation
    int targetRelId = OpenRelTable::openRel(targetRelation);
    if (targetRelId < 0) {
        Schema::deleteRel(targetRelation);
        return targetRelId;
    }

    Attribute record1[numOfAttributes1];
    Attribute record2[numOfAttributes2];
    Attribute targetRecord[numOfAttributesInTarget];

    // reset search index for relation1
    RelCacheTable::resetSearchIndex(srcRelId1);
    while (BlockAccess::project(srcRelId1, record1) == SUCCESS) {

        // reset search index of relation2
        RelCacheTable::resetSearchIndex(srcRelId2);

        // reset search index of attribute2
        AttrCacheTable::resetSearchIndex(srcRelId2, attribute2);
        while (BlockAccess::search(
                   srcRelId2, record2, attribute2,
                   record1[attrCatEntry1.offset], EQ) == SUCCESS) {

            int idx = 0;

            // copy record1
            for (int i = 0; i < numOfAttributes1; i++) {
                targetRecord[idx++] = record1[i];
            }

            // copy record2 except join attribute
            for (int i = 0; i < numOfAttributes2; i++) {
                if (i == attrCatEntry2.offset) continue;
                targetRecord[idx++] = record2[i];
            }

            ret = BlockAccess::insert(targetRelId, targetRecord);

            if (ret != SUCCESS) {
                OpenRelTable::closeRel(targetRelId);
                Schema::deleteRel(targetRelation);
                return E_DISKFULL;
            }
        }
    }
    
    OpenRelTable::closeRel(targetRelId);
    return SUCCESS;
}